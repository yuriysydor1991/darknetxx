#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_ZLIBCONTROLLER_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_ZLIBCONTROLLER_CLASS_H

#include <memory>
#include <string>
#include <vector>

#include "src/lib/zlib/GzipFileStreams.h"

/**
 * @brief The zlib compression adaptor subsystem namespace.
 */
namespace zlibi
{

/**
 * @brief A small zlib wrapper that hides the streaming boilerplate behind a few
 * simple compress / uncompress methods (in memory and to / from gzip files).
 *
 * The in-memory methods use the zlib (deflate) wrapper; the file methods use
 * the gzip (.gz) container. All methods log and return an empty result / false
 * on a zlib error instead of throwing, so they are trivial to call.
 *
 * The darknetxx adaptation of the cpp-app-template (the appZlib branch)
 * controller: the buffers are handed to the zlib in parts, so their size
 * isn't limited by the 32 bits zlib counters, and the gzip files may be
 * written and read a chunk at a time through the standard streams (see the
 * create_file_writer() and the create_file_reader() methods), which the
 * network weights files of the hundreds of megabytes need.
 */
class ZlibController
{
 public:
  using buffer = std::vector<unsigned char>;
  using ZlibControllerPtr = std::shared_ptr<ZlibController>;

  /// @brief The zlib default compression level (the Z_DEFAULT_COMPRESSION).
  static constexpr const int DEFAULT_LEVEL = -1;
  /// @brief The fastest compression level (the Z_BEST_SPEED).
  static constexpr const int BEST_SPEED = 1;
  /// @brief The smallest output compression level (the Z_BEST_COMPRESSION).
  static constexpr const int BEST_COMPRESSION = 9;

  virtual ~ZlibController() = default;
  ZlibController() = default;
  ZlibController(const ZlibController&) = delete;
  ZlibController& operator=(const ZlibController&) = delete;

  /**
   * @brief Compresses a buffer in memory with the zlib (deflate) wrapper.
   *
   * @param input The data to compress (may be empty).
   *
   * @return The compressed bytes, or an empty buffer on a zlib error.
   */
  virtual buffer compress(const buffer& input);

  /**
   * @brief Decompresses a buffer produced by compress().
   *
   * @param input The compressed data.
   *
   * @return The original bytes, or an empty buffer on a zlib error, truncated
   * or empty input.
   */
  virtual buffer uncompress(const buffer& input);

  /**
   * @brief Compresses a buffer and stores it as a gzip (.gz) file.
   *
   * @param path The destination file path.
   * @param input The data to compress.
   *
   * @return Returns true on success and false on any I/O or zlib error.
   */
  virtual bool compress_to_file(const std::string& path, const buffer& input);

  /**
   * @brief Reads and decompresses a gzip (.gz) file.
   *
   * @param path The source file path.
   *
   * @return The decompressed bytes, or an empty buffer on any I/O or zlib
   * error.
   */
  virtual buffer uncompress_from_file(const std::string& path);

  /**
   * @brief Opens the gzip (.gz) file for the write through the output stream,
   * which compresses the data a chunk at a time.
   *
   * @param path The destination file path, truncated.
   * @param level The compression level: the DEFAULT_LEVEL, the BEST_SPEED, the
   * BEST_COMPRESSION or any of the zlib 0 up to 9 ones.
   *
   * @return The open stream, the close() of which tells if the file is
   * complete, or a nullptr for the file unable to open.
   */
  virtual std::unique_ptr<GzipOStream> create_file_writer(
      const std::string& path, int level = DEFAULT_LEVEL);

  /**
   * @brief Opens the gzip (.gz) file for the read through the input stream,
   * which decompresses the data a chunk at a time.
   *
   * @param path The source file path.
   *
   * @return The open stream, the failed() of which tells the corrupted or the
   * truncated compressed data, or a nullptr for the file unable to open.
   */
  virtual std::unique_ptr<GzipIStream> create_file_reader(
      const std::string& path);

  /**
   * @brief Tells the gzip file by the magic bytes it starts with.
   *
   * @param path The file path.
   *
   * @return True for the existing gzip file and false otherwise.
   */
  static bool is_gzip_file(const std::string& path);

  /**
   * @brief Creates a ready to use controller instance.
   *
   * @return The created controller.
   */
  static ZlibControllerPtr create();
};

using ZlibControllerPtr = ZlibController::ZlibControllerPtr;

}  // namespace zlibi

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_ZLIBCONTROLLER_CLASS_H
