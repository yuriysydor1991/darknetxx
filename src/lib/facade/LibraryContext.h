#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_LIBRARYCONTEXT_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_LIBRARYCONTEXT_CLASS_H

/**
 * @brief The facade interface namespace for the library.
 * Designed to give access to the library functionality.
 */
namespace templatelib0
{

/**
 * @brief The library context class designed to pass data to
 * the library underlying implementation.
 *
 * Introduce some necessary fields for the particular newly
 * introduced code.
 *
 * Newly fields my be transferred from the ApplicationContext
 * class instance to a LibraryContext instance through
 * the ApplicationContext2LibraryContext converter (conversion
 * is performed inside the default Application class implementation).
 *
 * Current file is a target for the library header installation.
 */
class LibraryContext
{
 public:
  virtual ~LibraryContext() = default;
  LibraryContext() = default;

  // Introduce here the library required data fields.
  // Add some fields to pass data from it's users.
  // Register new fields inside the ApplicationContext2LibraryContext class.
};

}  // namespace templatelib0

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_LIBRARYCONTEXT_CLASS_H
