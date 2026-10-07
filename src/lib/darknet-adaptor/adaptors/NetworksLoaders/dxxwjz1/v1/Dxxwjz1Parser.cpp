#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Parser.h"

#if defined(__unix__)
// the newlocale() and the uselocale() of the POSIX 2008
#include <locale.h>
#endif

#include <istream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1SaxHandler.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/lib/zlib/ZlibController.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

namespace
{

using json = nlohmann::json;

/**
 * @brief Runs the thread in the "C" numeric locale for the life of the
 * object.
 *
 * The JSON lexer puts the first byte of the decimal point of the current
 * locale in place of the '.' of the number texts and converts them by the
 * strtod() of the locale, which the multibyte decimal points break (the
 * U+066B one of the ps_AF locale, for example): the strtod() stops at the
 * byte after the first one, which the lexer asserts against. The locale of
 * the thread alone is changed, so the global one of the application and the
 * other threads are kept. Where the thread locales aren't there, the
 * decimal point the lexer puts in the number texts is replaced back (see the
 * Dxxwjz1FloatCodec::decode_number()).
 */
class ClassicNumericLocale
{
 public:
  ClassicNumericLocale()
  {
#if defined(__unix__)
    locale_t base = duplocale(uselocale(locale_t{}));

    if (base == locale_t{}) {
      return;
    }

    mlocale = newlocale(LC_NUMERIC_MASK, "C", base);

    if (mlocale == locale_t{}) {
      freelocale(base);
      return;
    }

    mprevious = uselocale(mlocale);
#endif
  }

  ~ClassicNumericLocale()
  {
#if defined(__unix__)
    if (mlocale != locale_t{}) {
      uselocale(mprevious);
      freelocale(mlocale);
    }
#endif
  }

  ClassicNumericLocale(const ClassicNumericLocale&) = delete;
  ClassicNumericLocale& operator=(const ClassicNumericLocale&) = delete;

 private:
#if defined(__unix__)
  locale_t mlocale{};
  locale_t mprevious{};
#endif
};

}  // namespace

bool Dxxwjz1Parser::parse(std::istream& json, WeightsDocument& document,
                          WeightsReport& report)
{
  document = WeightsDocument{};

  Dxxwjz1SaxHandler handler{document, report};

  return parse(json, handler, document, report);
}

bool Dxxwjz1Parser::parse(std::istream& json, Dxxwjz1SaxHandler& handler,
                          WeightsDocument& document, WeightsReport& report)
{
  // the number texts of the JSON lexer of any locale of the application
  const ClassicNumericLocale classic;

  const bool parsed =
      json::sax_parse(json, &handler, json::input_format_t::json, true, false);

  if (!handler.complete(parsed)) {
    report.error(handler.error().empty() ? "the JSON text isn't complete"
                                         : handler.error());
    document = WeightsDocument{};
    return false;
  }

  document.normalize();

  return true;
}

bool Dxxwjz1Parser::parse_file(const std::string& filename,
                               WeightsDocument& document, WeightsReport& report)
{
  return parse_file(
      filename,
      [](WeightsDocument& parsed, WeightsReport& findings) {
        return std::make_unique<Dxxwjz1SaxHandler>(parsed, findings);
      },
      document, report);
}

bool Dxxwjz1Parser::parse_file(const std::string& filename,
                               const HandlerFactory& factory,
                               WeightsDocument& document, WeightsReport& report)
{
  std::unique_ptr<zlibi::GzipIStream> file =
      zlibi::ZlibController::create()->create_file_reader(filename);

  if (file == nullptr) {
    report.error("the file can't be opened");
    return false;
  }

  // the plain file of anything but the JSON text (the original weights file,
  // for example) fails the parse at its first byte
  if (file->direct()) {
    *file >> std::ws;

    if (file->peek() != '{') {
      report.error(
          "the file is neither the gzip compressed nor the plain "
          "JSON text (the original weights file?)");
      return false;
    }
  }

  document = WeightsDocument{};

  const std::unique_ptr<Dxxwjz1SaxHandler> handler = factory(document, report);
  const bool parsed = parse(*file, *handler, document, report);

  // the gzip failure is the reason of the JSON one it leads to
  if (file->failed()) {
    report = WeightsReport{};
    report.error("the gzip data is corrupted or truncated: " + file->error());
    document = WeightsDocument{};
    return false;
  }

  if (file->direct()) {
    report.warning(
        "the file isn't gzip compressed, it's read as the plain "
        "JSON text");
  }

  return parsed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
