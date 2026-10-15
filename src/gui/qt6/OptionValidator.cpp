#include "src/gui/qt6/OptionValidator.h"

namespace darknetxxQt6
{

OptionValidator::OptionValidator(net_editor::OptionPresenterPtr npresenter,
                                 QObject* parent)
    : QValidator{parent}, presenter{std::move(npresenter)}
{
}

QValidator::State OptionValidator::validate(QString& input, int& pos) const
{
  (void)pos;
  return presenter->typable(input.toStdString()) ? Acceptable : Invalid;
}

}  // namespace darknetxxQt6
