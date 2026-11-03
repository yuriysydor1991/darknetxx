#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_WRITER_CLASS_H

#include <filesystem>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pWriter.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pArchive.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/Yolo4Annotations.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/**
 * @brief The dxxwjz4p project writer (see the Dxxwjz4pFormat): writes the
 * JSON document the way the Dxxwjz3pWriter does, of the description text
 * and of the annotations of the copies of the images in the data folder of
 * the archive, then packs the document and the images into the ZIP archive.
 *
 * The images get the names of their files in the data folder, the _<n>
 * suffix of the repeated name (of the case blind file systems too). The
 * document is written into the uncompressed <file>.json-staging file beside
 * the file first, which is removed once the archive is packed. The images of
 * no file aren't written.
 */
class Dxxwjz4pWriter : public dxxwjz3p::v1::Dxxwjz3pWriter
{
 public:
  using Dxxwjz4pWriterPtr = std::shared_ptr<Dxxwjz4pWriter>;
  using Dxxwjz3pWriter::save;

  virtual ~Dxxwjz4pWriter() = default;

  /// @param level The zlib compression level of the archive members, the 0
  /// stores them.
  explicit Dxxwjz4pWriter(int level = DEFAULT_LEVEL);

  static Dxxwjz4pWriterPtr create();

  virtual bool save(network& net, const std::string& filename, int cutoff,
                    bool save_ema, const WeightsDocument& rest) override;

  virtual bool write(const WeightsDocument& document,
                     const std::string& filename) override;

 protected:
  virtual std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> create_emitter(
      std::ostream& out, const WeightsDocument& rest,
      const std::string& filename) const override;

  /// @brief The Dxxwjz3pWriter::check() of the images of the files.
  virtual bool check(const WeightsDocument& rest,
                     const std::string& filename) const override;

 private:
  /// @brief The archive members of the images of the annotations and the
  /// annotations of the member paths.
  void plan(const Yolo4Annotations& annotations, const std::string& filename);

  /// @brief Packs the staged document and the images into the file, the
  /// staged document removed.
  bool pack(bool staged, const std::string& staging,
            const std::string& filename) const;

  int mlevel;
  /// the root of the archive of the member paths of the annotations
  std::filesystem::path mroot;
  Yolo4Annotations mmembers;
  std::vector<Dxxwjz4pArchive::Member> mimages;
};

using Dxxwjz4pWriterPtr = Dxxwjz4pWriter::Dxxwjz4pWriterPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_WRITER_CLASS_H
