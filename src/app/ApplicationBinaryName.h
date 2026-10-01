#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONBINARYNAME_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONBINARYNAME_H

#include <string>

#include "project-global-decls.h"

namespace app
{

/**
 * @brief The name of the binary being run.
 *
 * The project builds one and the same app component into the main application
 * and into the network weights files converter only binary beside it, so
 * whatever names the running program - the usage line of the help message
 * and the version message - asks here instead of assuming the project name.
 *
 * The DARKNETXX_CONVERTER_ONLY definition this branches on is given to the
 * converter target alone (see src/darknetxxConverter-target.cmake), and only
 * the sources of the app component are compiled once per application target,
 * so this header belongs to that component and is included from nowhere else.
 *
 * @return Returns the name of the binary this translation unit is compiled
 * into.
 */
inline const std::string& application_binary_name()
{
#ifdef DARKNETXX_CONVERTER_ONLY
  return project_decls::PROJECT_CONVERTER_NAME;
#else
  return project_decls::PROJECT_NAME;
#endif  // DARKNETXX_CONVERTER_ONLY
}

}  // namespace app

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONBINARYNAME_H
