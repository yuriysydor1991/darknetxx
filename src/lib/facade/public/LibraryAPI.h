#ifndef DARKNETXX_011_LIBRARYAPI_H
#define DARKNETXX_011_LIBRARYAPI_H

/**
 * @brief Marks the classes of the installable library interface, the only
 * ones the library, built with the hidden symbols visibility, exports.
 */
#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef DARKNETXX_LIB_BUILDING
#define DARKNETXX_LIB_API __declspec(dllexport)
#else
#define DARKNETXX_LIB_API __declspec(dllimport)
#endif  // DARKNETXX_LIB_BUILDING
#else
#define DARKNETXX_LIB_API __attribute__((visibility("default")))
#endif  // _WIN32

#endif  // DARKNETXX_011_LIBRARYAPI_H
