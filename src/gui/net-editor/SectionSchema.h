#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_SECTIONSCHEMA_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_SECTIONSCHEMA_CLASS_H

#include <string>
#include <vector>

#include "src/gui/net-editor/AOption.h"
#include "src/gui/net-editor/NetworkSection.h"

namespace net_editor
{

/**
 * @brief What the editor knows of a cfg section type: what the section does,
 * the options it shows always, the further options it describes when the
 * section has them and the options of the new layer of the type.
 */
class SectionSchema
{
 public:
  SectionSchema(std::string nabout, std::vector<AOptionPtr> noptions,
                NetworkSection::Options ninitial = {},
                std::vector<AOptionPtr> nextras = {});

  const std::string& about() const;
  const std::vector<AOptionPtr>& options() const;
  const NetworkSection::Options& initial() const;

  /// @brief The option of the key among the shown and the further ones,
  /// nullptr for none.
  AOptionPtr find(const std::string& key) const;

 private:
  std::string mabout;
  std::vector<AOptionPtr> moptions;
  NetworkSection::Options minitial;
  std::vector<AOptionPtr> mextras;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_SECTIONSCHEMA_CLASS_H
