#ifndef DARKNETXX_PROJECT_CTEST_GROUPING_NUMPUNCT_CLASS_H
#define DARKNETXX_PROJECT_CTEST_GROUPING_NUMPUNCT_CLASS_H

#include <locale>
#include <string>

/**
 * @brief The numbers punctuation of the locales grouping the digits by three,
 * like the user's one the GTKmm installs globally on most hosts (the
 * std::locale::global(std::locale("")) of the Glib::init()).
 */
class GroupingNumpunct : public std::numpunct<char>
{
 protected:
  char do_thousands_sep() const override;
  std::string do_grouping() const override;
};

#endif  // DARKNETXX_PROJECT_CTEST_GROUPING_NUMPUNCT_CLASS_H
