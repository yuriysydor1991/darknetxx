#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_GZIPFILESTREAMS_CLASSES_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_GZIPFILESTREAMS_CLASSES_H

#include <istream>
#include <memory>
#include <ostream>
#include <streambuf>
#include <string>
#include <vector>

namespace zlibi
{

/**
 * @brief The stream buffer of a gzip (.gz) file: the compressed file written
 * or read through the standard streams, a chunk at a time.
 *
 * Neither the whole compressed nor the whole uncompressed content is kept in
 * the memory, which is what the big files (the network weights of the
 * hundreds of megabytes, for example) need.
 *
 * The buffer runs the zlib deflate and inflate of the gzip wrapper (RFC 1952)
 * itself instead of the zlib gzread(), which takes the file truncated right
 * after the compressed data for a complete one when the data ends at the
 * boundary of the read. The read checks the whole gzip stream: the header,
 * the compressed data and the CRC and the size of the trailer, the
 * concatenated gzip members included, and fails on the truncated or the
 * corrupted file and on the data after the gzip stream. A file which doesn't
 * start with the gzip magic bytes is read as it is.
 *
 * The errors don't throw: the stream gets the failure (the eof on the read and
 * the badbit on the write), while the failed() and the error() methods tell
 * the reason.
 */
class GzipStreamBuf : public std::streambuf
{
 public:
  enum class Mode
  {
    Read,
    Write
  };

  /// @brief The size of the uncompressed data chunk the buffer keeps.
  static constexpr const std::size_t CHUNK = 128U * 1024U;

  virtual ~GzipStreamBuf();
  GzipStreamBuf();
  GzipStreamBuf(const GzipStreamBuf&) = delete;
  GzipStreamBuf& operator=(const GzipStreamBuf&) = delete;

  /**
   * @brief Opens the file of the path.
   *
   * @param path The gzip file path.
   * @param mode Read the existing file or write (truncate) the file.
   * @param level The compression level of the write: the zlib 0 (none) up to
   * the 9 (the best) one, a negative value for the zlib default one.
   *
   * @return False for the file unable to open (or the buffer already open).
   */
  bool open(const std::string& path, Mode mode, int level);

  /**
   * @brief Writes the rest of the data and closes the file: the write
   * completes (the gzip trailer included) with the successful close only.
   *
   * @return False on any error during the file's life.
   */
  bool close();

  bool is_open() const;

  /// @brief True since the first zlib or I/O error.
  bool failed() const;

  /// @brief The description of the first error, empty for none.
  const std::string& error() const;

  /// @brief True for the read of a file which isn't a gzip one.
  bool direct() const;

 protected:
  virtual int_type overflow(int_type ch) override;
  virtual int sync() override;
  virtual int_type underflow() override;

 private:
  /// @brief The file and the zlib stream of it, kept out of the header along
  /// with the zlib.h.
  struct Codec;

  /// @brief Compresses the put area into the file.
  bool write_buffer();

  /// @brief Decompresses the next chunk of the file into the get area,
  /// returns the count of the bytes of it.
  std::size_t read_buffer();

  /// @brief Remembers the first error with the reason of it and logs it.
  void fail(const std::string& what, const std::string& reason);

  std::unique_ptr<Codec> mcodec;
  Mode mmode{Mode::Read};
  std::vector<char> mbuffer;
  bool mfailed{false};
  std::string merror;
  std::string mpath;
};

/**
 * @brief The output stream of a gzip file. The close() tells if the whole
 * file is written.
 */
class GzipOStream : public std::ostream
{
 public:
  virtual ~GzipOStream() = default;
  GzipOStream();

  /// @brief See GzipStreamBuf::open(), false for the file unable to open.
  bool open(const std::string& path, int level);

  /// @brief Completes the file, false on any error during the stream's life.
  bool close();

  bool is_open() const;

  /// @brief The description of the first error, empty for none.
  const std::string& error() const;

 private:
  GzipStreamBuf mbuffer;
};

/**
 * @brief The input stream of a gzip file. The failed() tells a corrupted or a
 * truncated compressed data from the end of the file.
 */
class GzipIStream : public std::istream
{
 public:
  virtual ~GzipIStream() = default;
  GzipIStream();

  /// @brief See GzipStreamBuf::open(), false for the file unable to open.
  bool open(const std::string& path);

  bool close();

  bool is_open() const;

  /// @brief True for the corrupted or the truncated compressed data.
  bool failed() const;

  /// @brief The description of the first error, empty for none.
  const std::string& error() const;

  /// @brief True for the file, which isn't a gzip one (read as it is).
  bool direct() const;

 private:
  GzipStreamBuf mbuffer;
};

}  // namespace zlibi

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_GZIPFILESTREAMS_CLASSES_H
