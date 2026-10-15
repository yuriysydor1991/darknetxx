#ifndef DARKNETXX_PROJECT_GUI_QT6_OPTIONVALIDATOR_CLASS_H
#define DARKNETXX_PROJECT_GUI_QT6_OPTIONVALIDATOR_CLASS_H

#include <QValidator>

#include "src/gui/net-editor/OptionPresenter.h"

namespace darknetxxQt6
{

/// @brief Keeps the characters of no value of the option out of it's entry
/// (see the OptionPresenter::typable()), the row tells the wrong values.
class OptionValidator : public QValidator
{
 public:
  OptionValidator(net_editor::OptionPresenterPtr npresenter, QObject* parent);

  virtual State validate(QString& input, int& pos) const override;

 private:
  const net_editor::OptionPresenterPtr presenter;
};

}  // namespace darknetxxQt6

#endif  // DARKNETXX_PROJECT_GUI_QT6_OPTIONVALIDATOR_CLASS_H
