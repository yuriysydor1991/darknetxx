#ifndef DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_WRITER_CLASS_H

#include <filesystem>
#include <map>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsWriter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::yolo4
{

/**
 * @brief Writes the document of the dxxwjz2p project into the YOLO4 training
 * folder (see the Yolo4FolderLayout) of the name of the folder (up to it's
 * first dot): the cfg/<name>.cfg network of the [net] keys of the folder
 * files, the data/obj.names classes, the data/obj.data of the original
 * darknet, the copies of the images in the data/obj/ folder of the names of
 * their own with their label files, the lists of the training and of the
 * validation images and the backup/<name>_last.dxxwjz3 weights of the
 * training record.
 */
class Yolo4FolderWriter : public IWeightsWriter
{
 public:
  using IWeightsWriter::save;

  virtual ~Yolo4FolderWriter() = default;
  Yolo4FolderWriter() = default;

  /// @brief None, the folder is converted alone.
  virtual bool save(network& net, const std::string& filename, int cutoff,
                    bool save_ema) override;

  virtual bool write(const WeightsDocument& document,
                     const std::string& filename) override;

 private:
  /// @brief Copies the image into the folder of the name of it's own of the
  /// case blind file systems as well, appends it's path to the list.
  ///
  /// @param taken The images of the lower case stems of the names.
  static bool copy(const Yolo4Image& image, const std::filesystem::path& root,
                   std::map<std::string, std::string>& taken,
                   std::string& list);

  static bool write_text(const std::filesystem::path& file,
                         const std::string& text);

  /// @brief The cfg text of the [net] keys of the values: the first lines of
  /// the keys are replaced, the rest of the keys end the section.
  static std::string net_keys(const std::string& cfg,
                              const std::map<std::string, std::string>& keys);
};

}  // namespace darknet_adaptor::loaders::yolo4

#endif  // DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_WRITER_CLASS_H
