#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_ZIP_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_ZIP_CLASS_H

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/// @brief A member of the ZIP archive.
struct Dxxwjz4pZipEntry
{
  /// the path of the member inside of the archive, the '/' separated one
  std::string name;
  /// the size of the uncompressed data of the member
  uint64_t size{0U};
  /// the regular file, the directory otherwise
  bool file{true};
};

/**
 * @brief Writes the ZIP archive (the one of the PKWARE APPNOTE.TXT) into the
 * seekable stream: the local header of a member, then it's data, deflated or
 * stored, and the central directory of the members at the end of the
 * archive. The sizes, the offsets and the counts the classic records can't
 * keep (the members and the archives of 4 GiB and more, the 65535 members
 * and more) go into the Zip64 ones, so any zip tool (the unzip, the 7-Zip,
 * the bsdtar, the archive managers of the desktops) lists and extracts the
 * members as the files of their own.
 *
 * The CRC-32 and the compressed size of the member go into it's local header
 * once the data is complete, the stream is seeked back for them, so the
 * archive has no data descriptors. The names of the non-ASCII UTF-8 text are
 * flagged the UTF-8 ones, the members get the 0644 permissions of the
 * regular files and the 0755 ones of the directories and the time of the
 * writer creation.
 */
class Dxxwjz4pZipWriter
{
 public:
  virtual ~Dxxwjz4pZipWriter();

  /**
   * @param out The seekable binary stream of the archive (the file one),
   * written from it's current position.
   * @param level The zlib compression level of the files: the 0 stores them,
   * the 1 (the fastest) up to the 9 (the smallest) and a negative one (the
   * zlib default) deflate them; the empty files are stored.
   * @param zip64 The Zip64 records of every member and of the end of the
   * archive, the ones of the big members and archives alone otherwise.
   */
  Dxxwjz4pZipWriter(std::ostream& out, int level, bool zip64 = false);

  Dxxwjz4pZipWriter(const Dxxwjz4pZipWriter&) = delete;
  Dxxwjz4pZipWriter& operator=(const Dxxwjz4pZipWriter&) = delete;

  /// @brief The local header of the regular file of the size, the data() of
  /// the size follows.
  bool file(const std::string& name, uint64_t size);

  /// @brief The directory member.
  bool directory(const std::string& name);

  /// @brief The data of the file member.
  bool data(const char* bytes, std::size_t count);

  /// @brief The data of the file member of the stream, up to it's end.
  bool data(std::istream& in);

  /// @brief Tells the data of the member is of the size of it's header.
  bool complete() const;

  /// @brief Ends the archive: the last member completed, the central
  /// directory and the end records of it.
  bool finish();

 private:
  /// @brief The member written: the record of the central directory.
  struct Record
  {
    std::string name;
    /// the position of the local header in the stream
    uint64_t offset{0U};
    uint64_t size{0U};
    uint64_t compressed{0U};
    uint32_t crc{0U};
    uint16_t method{0U};
    uint16_t flags{0U};
    bool directory{false};
    /// the local header of the Zip64 sizes
    bool zip64{false};
  };

  /// @brief The zlib deflate stream, kept out of the header along with the
  /// zlib.h.
  struct Deflater;

  /// @brief Writes the local header of the record: the flags and the
  /// position of the record set.
  bool start(Record& record);
  /// @brief Completes the data of the file member written and writes it's
  /// CRC-32 and compressed size into the local header.
  bool close();
  /// @brief Deflates the bytes into the stream, the last ones end the
  /// deflate data of the member.
  bool compress(const char* bytes, std::size_t count, bool last);
  bool write(const std::string& bytes);
  /// @brief Writes the central directory header of the record.
  bool central(const Record& record);
  uint64_t position();

  std::ostream& mout;
  int mlevel;
  bool mzip64;
  /// the MS-DOS time and date of the members
  uint16_t mtime{0U};
  uint16_t mdate{0U};
  std::unique_ptr<Deflater> mdeflater;
  std::vector<char> mbuffer;
  std::vector<Record> mrecords;
  /// the member of the data written
  Record mcurrent;
  bool mopen{false};
  /// the bytes of the member left to write
  uint64_t mleft{0U};
  bool mfailed{false};
};

/**
 * @brief Reads the ZIP archive of the seekable stream by it's central
 * directory: the stored and the deflated members of the regular files and
 * of the directories, the Zip64 ones included, the CRC-32 and the size of
 * every member checked.
 *
 * The encrypted members, the ones of the other compression methods (the
 * Deflate64, the bzip2, the LZMA, the Zstandard ones), the links and the
 * other members of the Unix types other than the regular files and the
 * directories, the members of the overlapping data and the archives split
 * into many files are the errors.
 */
class Dxxwjz4pZipReader
{
 public:
  virtual ~Dxxwjz4pZipReader();

  /// @param in The seekable binary stream of the archive (the file one).
  explicit Dxxwjz4pZipReader(std::istream& in);

  Dxxwjz4pZipReader(const Dxxwjz4pZipReader&) = delete;
  Dxxwjz4pZipReader& operator=(const Dxxwjz4pZipReader&) = delete;

  /**
   * @brief The next member of the central directory, the first call reads
   * the directory.
   *
   * @return False at the end of the archive and on an error (see the
   * error()).
   */
  bool next(Dxxwjz4pZipEntry& entry);

  /// @brief Writes the data of the current member into the stream.
  bool read(std::ostream& out);

  /// @brief The error of the archive, empty for none.
  const std::string& error() const;

 private:
  /// @brief The member of the central directory.
  struct Member
  {
    std::string name;
    /// the position of the local header
    uint64_t offset{0U};
    /// the position the data of the member ends before at most: the local
    /// header of the next member, the central directory
    uint64_t limit{0U};
    uint64_t size{0U};
    uint64_t compressed{0U};
    uint32_t crc{0U};
    uint32_t attributes{0U};
    uint16_t creator{0U};
    uint16_t flags{0U};
    uint16_t method{0U};
  };

  /// @brief The zlib inflate stream, kept out of the header along with the
  /// zlib.h.
  struct Inflater;

  /// @brief Reads the end records and the central directory.
  bool load();
  /// @brief Reads the members of the central directory and the limits of
  /// their data.
  bool central(uint64_t offset, uint64_t size, uint64_t count);
  /// @brief Takes the Zip64 values of the fields of the member of no room
  /// for them out of the extra field.
  bool extended(const std::string& extra, uint64_t& disk, Member& member);
  /// @brief Checks the member and seeks it's data.
  bool open(const Member& member);
  /// @brief Inflates the data of the member into the stream.
  bool decompress(const Member& member, std::ostream& out, uint32_t& crc);
  bool seek(uint64_t offset);
  bool bytes(char* data, std::size_t count);
  bool fail(const std::string& message);

  std::istream& min;
  std::unique_ptr<Inflater> minflater;
  std::vector<Member> mmembers;
  /// the index of the next member
  std::size_t mnext{0U};
  /// the current member of the data to read, the size of none
  std::size_t mcurrent{0U};
  bool mloaded{false};
  std::string merror;
};

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_ZIP_CLASS_H
