#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_CONVERSIONFORMATS_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_CONVERSIONFORMATS_CLASS_H

#include <string>
#include <vector>

/**
 * @brief The namespace of the command line driven network weights files
 * format conversion, which runs no training nor detection.
 */
namespace app::converter
{

/**
 * @brief The network weights files formats a conversion may name on either of
 * its sides.
 */
enum class ConversionFormat
{
  /// the original darknet weights file
  weights,
  /// the darknetxx weights JSON zipped file of the first version
  dxxwjz1
};

/**
 * @brief The command line spellings of the conversion formats.
 */
class ConversionFormats
{
 public:
  virtual ~ConversionFormats() = default;
  ConversionFormats() = default;

  /**
   * @brief Resolves the command line spelling of a weights file format.
   *
   * @param name The spelling given after a --convert-from or a --convert-to
   * command line flag.
   * @param format Out: the resolved format, untouched for an unknown spelling.
   *
   * @return Returns true when the spelling names a format and false otherwise.
   */
  static bool parse(const std::string& name, ConversionFormat& format);

  /**
   * @brief The accepted spellings, in the order the help message prints them.
   *
   * @return Returns the accepted command line spellings.
   */
  static const std::vector<std::string>& names();

  /**
   * @brief The help message description of the format of the spelling.
   *
   * @return Returns the description, the empty one for an unknown spelling.
   */
  static const std::string& description(const std::string& name);
};

}  // namespace app::converter

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_CONVERSIONFORMATS_CLASS_H
