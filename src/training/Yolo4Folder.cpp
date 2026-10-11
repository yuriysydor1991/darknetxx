#include "Yolo4Folder.h"

#include "src/NetworksLoaders/yolo4-folder/Yolo4FolderLayout.h"

namespace darknetxx011
{

using darknet_adaptor::loaders::yolo4::Yolo4FolderLayout;

Yolo4Folder::Yolo4Folder(const std::string& npath)
    : ATrainingProject{absolute_of(npath)}
{
  const std::string cfg = Yolo4FolderLayout::cfg(path());

  if (!cfg.empty()) {
    train(cfg, Yolo4FolderLayout::latest_weights(path(), cfg));
  }
}

std::string Yolo4Folder::names() const
{
  return Yolo4FolderLayout::names(path());
}

}  // namespace darknetxx011
