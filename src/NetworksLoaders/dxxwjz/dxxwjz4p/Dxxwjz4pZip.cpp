#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pZip.h"

#include <zlib.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <istream>
#include <memory>
#include <numeric>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

namespace
{

// the signatures of the records
constexpr uint32_t LOCAL_SIGNATURE = 0x04034B50U;
constexpr uint32_t CENTRAL_SIGNATURE = 0x02014B50U;
constexpr uint32_t END_SIGNATURE = 0x06054B50U;
constexpr uint32_t ZIP64_END_SIGNATURE = 0x06064B50U;
constexpr uint32_t ZIP64_LOCATOR_SIGNATURE = 0x07064B50U;

// the sizes of the fixed parts of the records
constexpr std::size_t LOCAL_SIZE = 30U;
constexpr std::size_t CENTRAL_SIZE = 46U;
constexpr std::size_t END_SIZE = 22U;
constexpr std::size_t ZIP64_END_SIZE = 56U;
constexpr std::size_t ZIP64_LOCATOR_SIZE = 20U;
/// the longest comment of the end record
constexpr std::size_t COMMENT_LIMIT = 0xFFFFU;

/// the header ID of the Zip64 extended information extra field
constexpr uint16_t ZIP64_EXTRA = 0x0001U;
/// the size of the Zip64 extra field of the local header: the size and the
/// compressed size
constexpr uint16_t ZIP64_LOCAL_EXTRA_SIZE = 20U;

// the compression methods
constexpr uint16_t STORED = 0U;
constexpr uint16_t DEFLATED = 8U;

// the general purpose flags
constexpr uint16_t ENCRYPTED_FLAG = 0x0001U;
constexpr uint16_t STRONG_ENCRYPTION_FLAG = 0x0040U;
constexpr uint16_t UTF8_FLAG = 0x0800U;
constexpr uint16_t MASKED_HEADER_FLAG = 0x2000U;

// the versions needed to extract: the deflate, the Zip64 ones
constexpr uint16_t DEFLATE_VERSION = 20U;
constexpr uint16_t ZIP64_VERSION = 45U;

// the hosts of the version made by, the Unix attributes of the APPNOTE 6.3
// the writer makes
constexpr uint16_t UNIX_HOST = 3U;
constexpr uint16_t DARWIN_HOST = 19U;
constexpr uint16_t MADE_BY = (UNIX_HOST << 8U) | 63U;

// the values of the fields of no room for the Zip64 ones
constexpr uint32_t ZIP64_MARK = 0xFFFFFFFFU;
constexpr uint16_t ZIP64_COUNT_MARK = 0xFFFFU;

/// the size of the file members the local header gets the Zip64 sizes of:
/// the deflate grows the data it can't compress by the 0.03 % at most
constexpr uint64_t ZIP64_LOCAL_SIZE = 0xF0000000ULL;

// the Unix file types and the permissions of the external attributes
constexpr uint32_t UNIX_TYPES = 0170000U;
constexpr uint32_t UNIX_FILE = 0100000U;
constexpr uint32_t UNIX_DIRECTORY = 040000U;
constexpr uint32_t UNIX_LINK = 0120000U;
constexpr uint32_t FILE_ATTRIBUTES = (UNIX_FILE | 0644U) << 16U;
/// the MS-DOS directory attribute too
constexpr uint32_t DIRECTORY_ATTRIBUTES =
    ((UNIX_DIRECTORY | 0755U) << 16U) | 0x10U;

/// the raw deflate data of no zlib or gzip wrapper
constexpr int RAW_WINDOW_BITS = -15;
constexpr int DEFLATE_MEMORY_LEVEL = 8;

/// the size of the data chunks, the zlib 32 bits counts keep it
constexpr std::size_t CHUNK = 1U << 16U;

void put16(std::string& out, const uint16_t value)
{
  out += static_cast<char>(value & 0xFFU);
  out += static_cast<char>(value >> 8U);
}

void put32(std::string& out, const uint32_t value)
{
  put16(out, static_cast<uint16_t>(value & 0xFFFFU));
  put16(out, static_cast<uint16_t>(value >> 16U));
}

void put64(std::string& out, const uint64_t value)
{
  put32(out, static_cast<uint32_t>(value & 0xFFFFFFFFU));
  put32(out, static_cast<uint32_t>(value >> 32U));
}

uint16_t get16(const char* const at)
{
  return static_cast<uint16_t>(static_cast<unsigned char>(at[0]) |
                               (static_cast<unsigned char>(at[1]) << 8U));
}

uint32_t get32(const char* const at)
{
  return static_cast<uint32_t>(get16(at)) |
         (static_cast<uint32_t>(get16(at + 2)) << 16U);
}

uint64_t get64(const char* const at)
{
  return static_cast<uint64_t>(get32(at)) |
         (static_cast<uint64_t>(get32(at + 4)) << 32U);
}

/// @brief The CRC-32 of the bytes going on from the crc of the bytes before.
uint32_t crc_of(uint32_t crc, const char* bytes, std::size_t count)
{
  while (count > 0U) {
    const std::size_t part = std::min(count, CHUNK);

    crc = static_cast<uint32_t>(crc32(
        crc, reinterpret_cast<const Bytef*>(bytes), static_cast<uInt>(part)));
    bytes += part;
    count -= part;
  }

  return crc;
}

/// @brief The MS-DOS time and date of the local time of the moment, the
/// 1980-01-01 of the earlier ones.
void dos_time_of(const std::time_t moment, uint16_t& time, uint16_t& date)
{
  std::tm local{};

#if defined(_WIN32)
  const bool converted = localtime_s(&local, &moment) == 0;
#else
  const bool converted = localtime_r(&moment, &local) != nullptr;
#endif

  if (!converted || local.tm_year < 80) {
    time = 0U;
    date = static_cast<uint16_t>((1U << 5U) | 1U);
    return;
  }

  // the 7 bits of the years since the 1980 keep them up to the 2107
  const int years = std::min(local.tm_year - 80, 127);

  time = static_cast<uint16_t>((local.tm_hour << 11) | (local.tm_min << 5) |
                               (local.tm_sec / 2));
  date = static_cast<uint16_t>((years << 9) | ((local.tm_mon + 1) << 5) |
                               local.tm_mday);
}

/// @brief Tells the name of the non-ASCII characters is the UTF-8 text.
bool utf8_of(const std::string& name)
{
  bool ascii = true;
  std::size_t at = 0U;

  while (at < name.size()) {
    const unsigned lead = static_cast<unsigned char>(name[at]);

    if (lead < 0x80U) {
      ++at;
      continue;
    }

    ascii = false;

    // the count of the continuation bytes and the range of the first one,
    // no overlong forms nor surrogates
    std::size_t count = 0U;
    unsigned low = 0x80U;
    unsigned high = 0xBFU;

    if (lead >= 0xC2U && lead <= 0xDFU) {
      count = 1U;
    } else if (lead >= 0xE0U && lead <= 0xEFU) {
      count = 2U;
      low = lead == 0xE0U ? 0xA0U : 0x80U;
      high = lead == 0xEDU ? 0x9FU : 0xBFU;
    } else if (lead >= 0xF0U && lead <= 0xF4U) {
      count = 3U;
      low = lead == 0xF0U ? 0x90U : 0x80U;
      high = lead == 0xF4U ? 0x8FU : 0xBFU;
    } else {
      return false;
    }

    if (at + count >= name.size()) {
      return false;
    }

    for (std::size_t next = 1U; next <= count; ++next) {
      const unsigned byte = static_cast<unsigned char>(name[at + next]);

      if (byte < (next == 1U ? low : 0x80U) ||
          byte > (next == 1U ? high : 0xBFU)) {
        return false;
      }
    }

    at += count + 1U;
  }

  return !ascii;
}

std::string zlib_reason(const z_stream& zs, const int code)
{
  return zs.msg != nullptr ? std::string{zs.msg}
                           : "the zlib error " + std::to_string(code);
}

}  // namespace

struct Dxxwjz4pZipWriter::Deflater
{
  Deflater() = default;
  Deflater(const Deflater&) = delete;
  Deflater& operator=(const Deflater&) = delete;

  ~Deflater()
  {
    if (ready) {
      deflateEnd(&zs);
    }
  }

  z_stream zs{};
  bool ready{false};
};

Dxxwjz4pZipWriter::~Dxxwjz4pZipWriter() = default;

Dxxwjz4pZipWriter::Dxxwjz4pZipWriter(std::ostream& out, const int level,
                                     const bool zip64)
    : mout{out},
      mlevel{level},
      mzip64{zip64},
      mdeflater{std::make_unique<Deflater>()},
      mbuffer(CHUNK)
{
  dos_time_of(std::time(nullptr), mtime, mdate);
}

bool Dxxwjz4pZipWriter::file(const std::string& name, const uint64_t size)
{
  if (!close()) {
    return false;
  }

  Record record;

  record.name = name;
  record.size = size;
  record.method = mlevel == 0 || size == 0U ? STORED : DEFLATED;
  record.zip64 = mzip64 || size >= ZIP64_LOCAL_SIZE;

  if (!start(record)) {
    return false;
  }

  if (record.method == DEFLATED) {
    z_stream& zs = mdeflater->zs;
    const int ret =
        mdeflater->ready
            ? deflateReset(&zs)
            : deflateInit2(&zs,
                           mlevel < 0 ? Z_DEFAULT_COMPRESSION
                                      : std::min(mlevel, Z_BEST_COMPRESSION),
                           Z_DEFLATED, RAW_WINDOW_BITS, DEFLATE_MEMORY_LEVEL,
                           Z_DEFAULT_STRATEGY);

    if (ret != Z_OK) {
      mfailed = true;
      return false;
    }

    mdeflater->ready = true;
  }

  mcurrent = std::move(record);
  mopen = true;
  mleft = size;

  return true;
}

bool Dxxwjz4pZipWriter::directory(const std::string& name)
{
  if (!close()) {
    return false;
  }

  Record record;

  record.name = name.empty() || name.back() == '/' ? name : name + "/";
  record.directory = true;
  record.zip64 = mzip64;

  if (!start(record)) {
    return false;
  }

  mrecords.push_back(std::move(record));

  return true;
}

bool Dxxwjz4pZipWriter::data(const char* const bytes, const std::size_t count)
{
  if (!mopen || mfailed || count > mleft) {
    return false;
  }

  if (count == 0U) {
    return true;
  }

  mcurrent.crc = crc_of(mcurrent.crc, bytes, count);
  mleft -= count;

  if (mcurrent.method == DEFLATED) {
    return compress(bytes, count, false);
  }

  mout.write(bytes, static_cast<std::streamsize>(count));
  mcurrent.compressed += count;
  mfailed = mfailed || !mout;

  return !mfailed;
}

bool Dxxwjz4pZipWriter::data(std::istream& in)
{
  std::vector<char> chunk(CHUNK);

  while (in) {
    in.read(chunk.data(), static_cast<std::streamsize>(chunk.size()));

    const auto count = static_cast<std::size_t>(in.gcount());

    if (count > 0U && !data(chunk.data(), count)) {
      return false;
    }
  }

  return in.eof() && !in.bad();
}

bool Dxxwjz4pZipWriter::complete() const { return mleft == 0U; }

bool Dxxwjz4pZipWriter::finish()
{
  if (!close()) {
    return false;
  }

  const uint64_t offset = position();

  for (const Record& record : mrecords) {
    if (!central(record)) {
      return false;
    }
  }

  const uint64_t end = position();
  const uint64_t size = end - offset;
  const uint64_t count = mrecords.size();
  std::string records;

  if (mzip64 || count >= ZIP64_COUNT_MARK || offset >= ZIP64_MARK ||
      size >= ZIP64_MARK) {
    // the Zip64 end of the central directory record and the locator of it
    put32(records, ZIP64_END_SIGNATURE);
    put64(records, ZIP64_END_SIZE - 12U);
    put16(records, MADE_BY);
    put16(records, ZIP64_VERSION);
    // the disk of the record and the one the central directory starts on
    put32(records, 0U);
    put32(records, 0U);
    put64(records, count);
    put64(records, count);
    put64(records, size);
    put64(records, offset);
    put32(records, ZIP64_LOCATOR_SIGNATURE);
    put32(records, 0U);
    put64(records, end);
    // the count of the disks
    put32(records, 1U);
  }

  const auto count16 =
      static_cast<uint16_t>(std::min<uint64_t>(count, ZIP64_COUNT_MARK));

  put32(records, END_SIGNATURE);
  put16(records, 0U);
  put16(records, 0U);
  put16(records, count16);
  put16(records, count16);
  put32(records, static_cast<uint32_t>(std::min<uint64_t>(size, ZIP64_MARK)));
  put32(records, static_cast<uint32_t>(std::min<uint64_t>(offset, ZIP64_MARK)));
  // the comment of none
  put16(records, 0U);

  if (!write(records)) {
    return false;
  }

  mout.flush();
  mfailed = mfailed || !mout;

  return !mfailed;
}

bool Dxxwjz4pZipWriter::start(Record& record)
{
  if (mfailed || record.name.empty() || record.name.size() > 0xFFFFU) {
    return false;
  }

  record.flags = utf8_of(record.name) ? UTF8_FLAG : uint16_t{0U};
  record.offset = position();

  std::string header;

  put32(header, LOCAL_SIGNATURE);
  put16(header, record.zip64 ? ZIP64_VERSION : DEFLATE_VERSION);
  put16(header, record.flags);
  put16(header, record.method);
  put16(header, mtime);
  put16(header, mdate);
  // the CRC-32 and the compressed size once the data is complete (close())
  put32(header, 0U);
  put32(header, record.zip64 ? ZIP64_MARK : 0U);
  put32(header, record.zip64 ? ZIP64_MARK : static_cast<uint32_t>(record.size));
  put16(header, static_cast<uint16_t>(record.name.size()));
  put16(header, record.zip64 ? ZIP64_LOCAL_EXTRA_SIZE : uint16_t{0U});
  header += record.name;

  if (record.zip64) {
    put16(header, ZIP64_EXTRA);
    put16(header, static_cast<uint16_t>(ZIP64_LOCAL_EXTRA_SIZE - 4U));
    put64(header, record.size);
    put64(header, 0U);
  }

  return write(header);
}

bool Dxxwjz4pZipWriter::close()
{
  if (!mopen) {
    return !mfailed;
  }

  if (mfailed || mleft != 0U ||
      (mcurrent.method == DEFLATED && !compress(nullptr, 0U, true))) {
    return false;
  }

  mopen = false;

  if (!mcurrent.zip64 && mcurrent.compressed >= ZIP64_MARK) {
    mfailed = true;
    return false;
  }

  const uint64_t end = position();
  std::string fields;
  std::string compressed;

  put32(fields, mcurrent.crc);
  put32(fields, mcurrent.zip64 ? ZIP64_MARK
                               : static_cast<uint32_t>(mcurrent.compressed));
  put64(compressed, mcurrent.compressed);

  // the CRC-32 and the compressed size fields of the local header, the
  // compressed size of it's Zip64 extra field after the name too
  mout.seekp(static_cast<std::streamoff>(mcurrent.offset + 14U));

  if (!write(fields)) {
    return false;
  }

  if (mcurrent.zip64) {
    mout.seekp(static_cast<std::streamoff>(mcurrent.offset + LOCAL_SIZE +
                                           mcurrent.name.size() + 12U));

    if (!write(compressed)) {
      return false;
    }
  }

  mout.seekp(static_cast<std::streamoff>(end));
  mfailed = mfailed || !mout;

  if (mfailed) {
    return false;
  }

  mrecords.push_back(std::move(mcurrent));
  mcurrent = Record{};

  return true;
}

bool Dxxwjz4pZipWriter::compress(const char* bytes, std::size_t count,
                                 const bool last)
{
  z_stream& zs = mdeflater->zs;

  do {
    const std::size_t part = std::min(count, CHUNK);
    const bool finishing = last && part == count;
    int ret = Z_OK;

    // the zlib reads the input alone, the old zlib versions take no const
    // one
    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(bytes));
    zs.avail_in = static_cast<uInt>(part);

    if (bytes != nullptr) {
      bytes += part;
    }

    count -= part;

    do {
      zs.next_out = reinterpret_cast<Bytef*>(mbuffer.data());
      zs.avail_out = static_cast<uInt>(mbuffer.size());
      ret = ::deflate(&zs, finishing ? Z_FINISH : Z_NO_FLUSH);

      const std::size_t produced = mbuffer.size() - zs.avail_out;

      if ((ret != Z_OK && ret != Z_BUF_ERROR && ret != Z_STREAM_END) ||
          (ret == Z_BUF_ERROR && finishing && produced == 0U)) {
        mfailed = true;
        return false;
      }

      mout.write(mbuffer.data(), static_cast<std::streamsize>(produced));
      mcurrent.compressed += produced;

      if (!mout) {
        mfailed = true;
        return false;
      }
    } while (finishing ? ret != Z_STREAM_END : zs.avail_out == 0U);
  } while (count > 0U);

  return true;
}

bool Dxxwjz4pZipWriter::write(const std::string& bytes)
{
  mout.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
  mfailed = mfailed || !mout;

  return !mfailed;
}

bool Dxxwjz4pZipWriter::central(const Record& record)
{
  const bool sizes64 = record.zip64 || record.size >= ZIP64_MARK ||
                       record.compressed >= ZIP64_MARK;
  const bool offset64 = mzip64 || record.offset >= ZIP64_MARK;
  std::string extra;

  // the Zip64 fields of the ones marked alone, in their order
  if (sizes64 || offset64) {
    put16(extra, ZIP64_EXTRA);
    put16(extra,
          static_cast<uint16_t>((sizes64 ? 16U : 0U) + (offset64 ? 8U : 0U)));

    if (sizes64) {
      put64(extra, record.size);
      put64(extra, record.compressed);
    }

    if (offset64) {
      put64(extra, record.offset);
    }
  }

  std::string header;

  put32(header, CENTRAL_SIGNATURE);
  put16(header, MADE_BY);
  put16(header, sizes64 || offset64 ? ZIP64_VERSION : DEFLATE_VERSION);
  put16(header, record.flags);
  put16(header, record.method);
  put16(header, mtime);
  put16(header, mdate);
  put32(header, record.crc);
  put32(header,
        sizes64 ? ZIP64_MARK : static_cast<uint32_t>(record.compressed));
  put32(header, sizes64 ? ZIP64_MARK : static_cast<uint32_t>(record.size));
  put16(header, static_cast<uint16_t>(record.name.size()));
  put16(header, static_cast<uint16_t>(extra.size()));
  // the comment of none, the first disk and the internal attributes
  put16(header, 0U);
  put16(header, 0U);
  put16(header, 0U);
  put32(header, record.directory ? DIRECTORY_ATTRIBUTES : FILE_ATTRIBUTES);
  put32(header, offset64 ? ZIP64_MARK : static_cast<uint32_t>(record.offset));
  header += record.name;
  header += extra;

  return write(header);
}

uint64_t Dxxwjz4pZipWriter::position()
{
  const std::streamoff at = mout.tellp();

  if (at < 0) {
    mfailed = true;
    return 0U;
  }

  return static_cast<uint64_t>(at);
}

struct Dxxwjz4pZipReader::Inflater
{
  Inflater() = default;
  Inflater(const Inflater&) = delete;
  Inflater& operator=(const Inflater&) = delete;

  ~Inflater()
  {
    if (ready) {
      inflateEnd(&zs);
    }
  }

  z_stream zs{};
  bool ready{false};
};

Dxxwjz4pZipReader::~Dxxwjz4pZipReader() = default;

Dxxwjz4pZipReader::Dxxwjz4pZipReader(std::istream& in)
    : min{in}, minflater{std::make_unique<Inflater>()}
{
}

bool Dxxwjz4pZipReader::next(Dxxwjz4pZipEntry& entry)
{
  if (!mloaded) {
    mloaded = true;

    if (!load()) {
      mmembers.clear();
      return false;
    }
  }

  mcurrent = mmembers.size();

  if (!merror.empty() || mnext >= mmembers.size()) {
    return false;
  }

  const Member& member = mmembers[mnext];
  const bool directory = !member.name.empty() && member.name.back() == '/';

  if (!open(member)) {
    return false;
  }

  entry.name = member.name;
  entry.file = !directory;
  entry.size = directory ? 0U : member.size;
  mcurrent = directory ? mmembers.size() : mnext;
  ++mnext;

  return true;
}

bool Dxxwjz4pZipReader::read(std::ostream& out)
{
  if (!merror.empty() || mcurrent >= mmembers.size()) {
    return false;
  }

  const Member& member = mmembers[mcurrent];
  uint32_t crc = 0U;

  mcurrent = mmembers.size();

  if (member.method == DEFLATED) {
    if (!decompress(member, out, crc)) {
      return false;
    }
  } else {
    if (member.compressed != member.size) {
      return fail("the sizes of the stored archive member " + member.name +
                  " differ, the archive is corrupted");
    }

    std::vector<char> chunk(CHUNK);

    for (uint64_t left = member.size; left > 0U;) {
      const auto count =
          static_cast<std::size_t>(std::min<uint64_t>(left, chunk.size()));

      if (!bytes(chunk.data(), count)) {
        return false;
      }

      crc = crc_of(crc, chunk.data(), count);
      out.write(chunk.data(), static_cast<std::streamsize>(count));

      if (!out) {
        return fail("the archive member " + member.name + " can't be written");
      }

      left -= count;
    }
  }

  if (crc != member.crc) {
    return fail("the CRC-32 of the archive member " + member.name +
                " is wrong, the archive is corrupted");
  }

  return true;
}

const std::string& Dxxwjz4pZipReader::error() const { return merror; }

bool Dxxwjz4pZipReader::load()
{
  min.clear();
  min.seekg(0, std::ios::end);

  const std::streamoff end = min.tellg();

  if (!min || end < 0) {
    return fail("the archive can't be read");
  }

  const auto size = static_cast<uint64_t>(end);

  if (size < END_SIZE) {
    return fail("the file is too short to be the ZIP archive");
  }

  // the end record of the comment up to the 64 KiB ends the archive
  const uint64_t tail = std::min<uint64_t>(size, END_SIZE + COMMENT_LIMIT);
  std::string data(static_cast<std::size_t>(tail), '\0');

  if (!seek(size - tail) || !bytes(data.data(), data.size())) {
    return false;
  }

  std::size_t found = std::string::npos;

  for (std::size_t at = data.size() - END_SIZE + 1U; at-- > 0U;) {
    const std::size_t comment = get16(data.data() + at + 20U);

    if (get32(data.data() + at) == END_SIGNATURE &&
        at + END_SIZE + comment <= data.size()) {
      found = at;
      break;
    }
  }

  if (found == std::string::npos) {
    return fail(
        "the file has no end of the central directory of the ZIP archive, "
        "it isn't the ZIP archive or it's truncated");
  }

  const char* const record = data.data() + found;
  const uint64_t position = size - tail + found;
  uint64_t disk = get16(record + 4U);
  uint64_t first_disk = get16(record + 6U);
  uint64_t disk_count = get16(record + 8U);
  uint64_t count = get16(record + 10U);
  uint64_t central_size = get32(record + 12U);
  uint64_t central_offset = get32(record + 16U);
  // the central directory ends before the end records
  uint64_t limit = position;

  // the Zip64 end record of the locator right before the end record
  if (position >= ZIP64_LOCATOR_SIZE) {
    std::array<char, ZIP64_LOCATOR_SIZE> locator{};

    if (!seek(position - ZIP64_LOCATOR_SIZE) ||
        !bytes(locator.data(), locator.size())) {
      return false;
    }

    if (get32(locator.data()) == ZIP64_LOCATOR_SIGNATURE) {
      const uint64_t zip64 = get64(locator.data() + 8U);
      std::array<char, ZIP64_END_SIZE> end64{};

      if (position - ZIP64_LOCATOR_SIZE < ZIP64_END_SIZE ||
          zip64 > position - ZIP64_LOCATOR_SIZE - ZIP64_END_SIZE ||
          !seek(zip64) || !bytes(end64.data(), end64.size()) ||
          get32(end64.data()) != ZIP64_END_SIGNATURE) {
        merror.clear();
        return fail("the Zip64 end record of the archive is corrupted");
      }

      disk = get32(end64.data() + 16U) | get32(locator.data() + 4U);
      first_disk = get32(end64.data() + 20U);
      disk_count = get64(end64.data() + 24U);
      count = get64(end64.data() + 32U);
      central_size = get64(end64.data() + 40U);
      central_offset = get64(end64.data() + 48U);
      limit = zip64;
    }
  }

  if (disk != 0U || first_disk != 0U || disk_count != count) {
    return fail("the archive is split into many files, which isn't read");
  }

  if (central_offset > limit || central_size > limit - central_offset ||
      count > central_size / CENTRAL_SIZE) {
    return fail(
        "the central directory of the archive is corrupted or out of the "
        "file");
  }

  return central(central_offset, central_size, count);
}

bool Dxxwjz4pZipReader::central(const uint64_t offset, const uint64_t size,
                                const uint64_t count)
{
  if (!seek(offset)) {
    return false;
  }

  mmembers.clear();
  mmembers.reserve(static_cast<std::size_t>(count));

  std::array<char, CENTRAL_SIZE> header{};
  uint64_t taken = 0U;

  for (uint64_t index = 0U; index < count; ++index) {
    if (size - taken < CENTRAL_SIZE || !bytes(header.data(), header.size()) ||
        get32(header.data()) != CENTRAL_SIGNATURE) {
      merror.clear();
      return fail("the central directory of the archive is corrupted");
    }

    Member member;
    const std::size_t name = get16(header.data() + 28U);
    const std::size_t extra = get16(header.data() + 30U);
    const std::size_t comment = get16(header.data() + 32U);
    uint64_t disk = get16(header.data() + 34U);

    member.creator = get16(header.data() + 4U);
    member.flags = get16(header.data() + 8U);
    member.method = get16(header.data() + 10U);
    member.crc = get32(header.data() + 16U);
    member.compressed = get32(header.data() + 20U);
    member.size = get32(header.data() + 24U);
    member.attributes = get32(header.data() + 38U);
    member.offset = get32(header.data() + 42U);
    taken += CENTRAL_SIZE + name + extra + comment;

    std::string rest(name + extra + comment, '\0');

    if (taken > size || !bytes(rest.data(), rest.size())) {
      merror.clear();
      return fail("the central directory of the archive is corrupted");
    }

    member.name = rest.substr(0U, name);

    if (!extended(rest.substr(name, extra), disk, member)) {
      return false;
    }

    if (disk != 0U) {
      return fail("the archive is split into many files, which isn't read");
    }

    mmembers.push_back(std::move(member));
  }

  // the data of a member ends before the local header of the next one
  std::vector<std::size_t> order(mmembers.size());

  std::iota(order.begin(), order.end(), std::size_t{0U});
  std::sort(order.begin(), order.end(),
            [this](const std::size_t one, const std::size_t other) {
              return mmembers[one].offset < mmembers[other].offset;
            });

  for (std::size_t at = 0U; at < order.size(); ++at) {
    Member& member = mmembers[order[at]];

    member.limit =
        at + 1U < order.size() ? mmembers[order[at + 1U]].offset : offset;

    if (member.limit <= member.offset) {
      return fail("the archive member " + member.name +
                  " overlaps another one, the archive is corrupted");
    }
  }

  return true;
}

bool Dxxwjz4pZipReader::extended(const std::string& extra, uint64_t& disk,
                                 Member& member)
{
  const bool size = member.size == ZIP64_MARK;
  const bool compressed = member.compressed == ZIP64_MARK;
  const bool offset = member.offset == ZIP64_MARK;
  const bool disked = disk == ZIP64_COUNT_MARK;

  if (!size && !compressed && !offset && !disked) {
    return true;
  }

  for (std::size_t at = 0U; at + 4U <= extra.size();) {
    const uint16_t id = get16(extra.data() + at);
    const std::size_t length = get16(extra.data() + at + 2U);

    if (at + 4U + length > extra.size()) {
      break;
    }

    if (id == ZIP64_EXTRA) {
      const char* field = extra.data() + at + 4U;
      const std::size_t needed = (size ? 8U : 0U) + (compressed ? 8U : 0U) +
                                 (offset ? 8U : 0U) + (disked ? 4U : 0U);

      if (length < needed) {
        return fail("the Zip64 extra field of the archive member " +
                    member.name + " is corrupted");
      }

      if (size) {
        member.size = get64(field);
        field += 8;
      }

      if (compressed) {
        member.compressed = get64(field);
        field += 8;
      }

      if (offset) {
        member.offset = get64(field);
        field += 8;
      }

      if (disked) {
        disk = get32(field);
      }

      return true;
    }

    at += 4U + length;
  }

  // the values of no Zip64 field are the values themselves
  return true;
}

bool Dxxwjz4pZipReader::open(const Member& member)
{
  if ((member.flags &
       (ENCRYPTED_FLAG | STRONG_ENCRYPTION_FLAG | MASKED_HEADER_FLAG)) != 0U) {
    return fail("the archive member " + member.name +
                " is encrypted, the encrypted members aren't read");
  }

  if (member.method != STORED && member.method != DEFLATED) {
    return fail("the archive member " + member.name +
                " is compressed by the method " +
                std::to_string(member.method) +
                ", the stored and the deflated members alone are read");
  }

  const auto host = static_cast<uint16_t>(member.creator >> 8U);
  const uint32_t type = (member.attributes >> 16U) & UNIX_TYPES;

  if ((host == UNIX_HOST || host == DARWIN_HOST) && type != 0U &&
      type != UNIX_FILE && type != UNIX_DIRECTORY) {
    return fail("the archive member " + member.name +
                (type == UNIX_LINK
                     ? " is a link, neither the regular file nor the "
                       "directory"
                     : " is neither the regular file nor the directory (a "
                       "device?)"));
  }

  std::array<char, LOCAL_SIZE> header{};

  if (member.limit - member.offset < LOCAL_SIZE || !seek(member.offset) ||
      !bytes(header.data(), header.size()) ||
      get32(header.data()) != LOCAL_SIGNATURE) {
    merror.clear();
    return fail("the local header of the archive member " + member.name +
                " is corrupted");
  }

  const std::size_t name = get16(header.data() + 26U);
  const std::size_t extra = get16(header.data() + 28U);
  const uint64_t start = member.offset + LOCAL_SIZE + name + extra;

  if (start > member.limit || member.compressed > member.limit - start) {
    return fail("the data of the archive member " + member.name +
                " overlaps the next member or the central directory, the "
                "archive is corrupted");
  }

  std::string local(name, '\0');

  if (!bytes(local.data(), local.size())) {
    return false;
  }

  if (local != member.name) {
    return fail("the local header of the archive member " + member.name +
                " names another one, " + local + ", the archive is corrupted");
  }

  return seek(start);
}

bool Dxxwjz4pZipReader::decompress(const Member& member, std::ostream& out,
                                   uint32_t& crc)
{
  z_stream& zs = minflater->zs;

  zs.next_in = nullptr;
  zs.avail_in = 0U;

  const int ready =
      minflater->ready ? inflateReset(&zs) : inflateInit2(&zs, RAW_WINDOW_BITS);

  if (ready != Z_OK) {
    return fail("the inflate of the archive member " + member.name +
                " can't start: " + zlib_reason(zs, ready));
  }

  minflater->ready = true;

  std::vector<char> input(CHUNK);
  std::vector<char> output(CHUNK);
  uint64_t left = member.compressed;
  uint64_t written = 0U;
  int ret = Z_OK;

  while (ret != Z_STREAM_END) {
    // the inflate of no input left still writes the output it keeps, the
    // Z_BUF_ERROR of no progress tells the end of the data then
    if (zs.avail_in == 0U && left > 0U) {
      const auto count =
          static_cast<std::size_t>(std::min<uint64_t>(left, input.size()));

      if (!bytes(input.data(), count)) {
        return false;
      }

      left -= count;
      zs.next_in = reinterpret_cast<Bytef*>(input.data());
      zs.avail_in = static_cast<uInt>(count);
    }

    zs.next_out = reinterpret_cast<Bytef*>(output.data());
    zs.avail_out = static_cast<uInt>(output.size());
    ret = ::inflate(&zs, Z_NO_FLUSH);

    if (ret == Z_BUF_ERROR) {
      return fail("the deflated data of the archive member " + member.name +
                  " is truncated, the archive is corrupted");
    }

    if (ret != Z_OK && ret != Z_STREAM_END) {
      return fail("the deflated data of the archive member " + member.name +
                  " is corrupted: " + zlib_reason(zs, ret));
    }

    const std::size_t produced = output.size() - zs.avail_out;

    written += produced;

    if (written > member.size) {
      return fail("the archive member " + member.name +
                  " is bigger than it's size in the central directory, the "
                  "archive is corrupted");
    }

    crc = crc_of(crc, output.data(), produced);
    out.write(output.data(), static_cast<std::streamsize>(produced));

    if (!out) {
      return fail("the archive member " + member.name + " can't be written");
    }
  }

  if (left != 0U || zs.avail_in != 0U || written != member.size) {
    return fail("the deflated data of the archive member " + member.name +
                " isn't of it's sizes in the central directory, the archive "
                "is corrupted");
  }

  return true;
}

bool Dxxwjz4pZipReader::seek(const uint64_t offset)
{
  min.clear();
  min.seekg(static_cast<std::streamoff>(offset));

  if (!min) {
    return fail("the archive can't be read");
  }

  return true;
}

bool Dxxwjz4pZipReader::bytes(char* const data, const std::size_t count)
{
  min.read(data, static_cast<std::streamsize>(count));

  if (static_cast<std::size_t>(min.gcount()) != count) {
    return fail("the archive is truncated");
  }

  return true;
}

bool Dxxwjz4pZipReader::fail(const std::string& message)
{
  if (merror.empty()) {
    merror = message;
  }

  return false;
}

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1
