#ifndef DARKNET_CXX_PORT_PROJECT_CFG_SECTION_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CFG_SECTION_CLASS_H

#include <string>
#include <vector>

namespace darknet_adaptor::loaders
{

/**
 * @brief One [section] of the darknet network cfg file with it's key=value
 * options: the C++ port of the read_cfg() of the original darknet parser.c
 * and of the option_find*() functions of it's option_list.c.
 *
 * The lookups behave the way the originals did: the first option of the key
 * wins, a found option is marked used, the default values of the not quiet
 * lookups are logged and unused() logs the options no lookup asked for.
 */
class CfgSection
{
 public:
  /// @brief The sections of the cfg file in the file order.
  using CfgSections = std::vector<CfgSection>;

  explicit CfgSection(std::string ntype);

  /**
   * @brief Reads the sections of the cfg file the way read_cfg() did: every
   * whitespace of a line is removed, the empty lines and the ones starting
   * with the '#' or the ';' are skipped, a line starting with the '[' begins
   * the next section and the rest are the key=value options of the current
   * one. The option line without the value is logged and skipped.
   *
   * Throws NetworkError for the file unable to open and for an option before
   * the first section.
   */
  static CfgSections read_cfg(const std::string& filename);

  /// @brief The section header line, e.g. "[convolutional]".
  const std::string& type() const;

  /**
   * @brief Adds the option of the key=value cfg line, as read_option() did:
   * the line without the '=' is the key of the NULL value, while the line
   * without the value after the '=' is rejected (false).
   */
  bool add(const std::string& line);

  /**
   * @brief The value of the key or the NULL for none, the option_find(). The
   * pointer stays valid for as long as the section does and no option is
   * added to it.
   */
  const char* find(const std::string& key);

  const char* find_str(const std::string& key, const char* def);
  const char* find_str_quiet(const std::string& key, const char* def);
  int find_int(const std::string& key, int def);
  int find_int_quiet(const std::string& key, int def);
  float find_float(const std::string& key, float def);
  float find_float_quiet(const std::string& key, float def);

  /// @brief Logs the options no lookup asked for, the option_unused().
  void unused() const;

 private:
  struct Option
  {
    std::string key;
    std::string val;
    /// the key=value line without the '=', which option_find() gave the NULL
    bool has_val{true};
    bool used{false};
  };

  std::string mtype;
  std::vector<Option> moptions;
};

using CfgSections = CfgSection::CfgSections;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_CFG_SECTION_CLASS_H
