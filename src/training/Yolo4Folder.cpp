#include "Yolo4Folder.h"

#include "src/NetworksLoaders/DataDriversAliases.h"

namespace darknetxx012
{

using dxxdd::yolo4::Yolo4FolderLayout;

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

}  // namespace darknetxx012
