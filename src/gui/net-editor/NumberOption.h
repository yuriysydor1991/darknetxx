#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_NUMBEROPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_NUMBEROPTION_CLASS_H

#include <limits>
#include <utility>

#include "src/gui/net-editor/AOption.h"

namespace net_editor
{

/**
 * @brief The option of a number or of the comma separated list of them: the
 * integers or the real numbers of a range, which the descendants make depend
 * on the network and further rule.
 */
class NumberOption : public AOption
{
 public:
  /// @brief The lowest and the highest value.
  using Range = std::pair<double, double>;

  static constexpr const double NONE = std::numeric_limits<double>::infinity();

  /**
   * @param nintegers The integers alone, the real numbers otherwise.
   * @param nlist The comma separated list of the values, a single value
   * otherwise.
   */
  NumberOption(std::string nkey, std::string ndef, std::string nabout,
               bool nintegers, bool nlist, Range nrange = {-NONE, NONE});

  virtual bool typable(const std::string& text) const override;
  virtual std::string limits(const OptionContext& ctx) const override;
  virtual std::string error(const std::string& value,
                            const OptionContext& ctx) const override;

  /// @brief The text of the value, the integer one of no fraction.
  static std::string text_of(double value);

 protected:
  /// @brief The range of every value in the context.
  virtual Range range(const OptionContext& ctx) const;

  /// @brief The further rule of the values in the context, empty for none.
  virtual std::string rule(const OptionContext& ctx) const;

  /// @brief The reason the values break the further rule, empty for none.
  virtual std::string check(const std::vector<double>& values,
                            const OptionContext& ctx) const;

 private:
  /// @brief The text of the range, empty for the unbounded one.
  static std::string range_text(const Range& bounds);

  const bool mintegers;
  const bool mlist;
  const Range mrange;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_NUMBEROPTION_CLASS_H
