#include "src/gui/net-editor/OptionContext.h"

namespace net_editor
{

OptionContext::OptionContext(const NetworkModel& nmodel,
                             const NetworkSection& nsection)
    : model{nmodel}, section{nsection}
{
}

int OptionContext::index() const { return model.index_of(section); }

}  // namespace net_editor
