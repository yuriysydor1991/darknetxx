#include "WeightsFile.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace weights_factory
{

namespace
{

constexpr const uint64_t CHUNK_FLOATS = 1U << 16U;

uint64_t splitmix64(uint64_t value)
{
  value += 0x9E3779B97F4A7C15ULL;
  value = (value ^ (value >> 30U)) * 0xBF58476D1CE4E5B9ULL;
  value = (value ^ (value >> 27U)) * 0x94D049BB133111EBULL;
  return value ^ (value >> 31U);
}

PatternValues block_pattern(uint64_t seed, const Block& block)
{
  uint64_t key = splitmix64(seed);
  key = splitmix64(key ^
                   static_cast<uint64_t>(static_cast<uint32_t>(block.layer)));
  key = splitmix64(key ^ (static_cast<uint64_t>(block.sub) << 8U) ^
                   static_cast<uint64_t>(block.array));

  return PatternValues{key, block.array == ArrayKind::RollingVariance};
}

template <typename T>
void write_value(std::ofstream& file, const T& value)
{
  file.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename T>
bool read_value(std::ifstream& file, T& value)
{
  file.read(reinterpret_cast<char*>(&value), sizeof(T));
  return file.gcount() == static_cast<std::streamsize>(sizeof(T));
}

std::string hex(uint32_t value)
{
  std::ostringstream text;
  text << "0x" << std::hex << std::setw(8) << std::setfill('0') << value;
  return text.str();
}

}  // namespace

bool WeightsHeader::seen64() const
{
  return static_cast<int64_t>(major) * 10 + minor >= 2;
}

bool WeightsHeader::transposed() const { return major > 1000 || minor > 1000; }

uint64_t WeightsHeader::size() const
{
  return 3U * sizeof(int32_t) +
         (seen64() ? sizeof(uint64_t) : sizeof(uint32_t));
}

uint64_t WeightsHeader::stored_seen() const
{
  return seen64() ? seen : static_cast<uint32_t>(seen);
}

std::string WeightsHeader::describe() const
{
  std::ostringstream text;
  text << major << "." << minor << "." << revision << " seen " << seen;
  return text.str();
}

float PatternValues::operator()(uint64_t index) const
{
  // 24 random bits
  const auto random = static_cast<uint32_t>(
      splitmix64(key + index * 0xD1B54A32D192ED03ULL) >> 40U);

  if (positive) {
    // the multiples of 2^-23 in the [0.5, 1) range
    return static_cast<float>((random >> 2U) | (1U << 22U)) * (1.f / 8388608.f);
  }

  // the multiples of 2^-23 in the [-1, 1) range
  return (static_cast<float>(random) - 8388608.f) * (1.f / 8388608.f);
}

Pattern::Pattern(uint64_t seed) : mseed{seed} {}

PatternValues Pattern::of(const Block& block) const
{
  return block_pattern(mseed, block);
}

float Pattern::value(const Block& block, uint64_t index) const
{
  return block_pattern(mseed, block)(index);
}

FileModel::FileModel(const Layout& layout, const Pattern& pattern)
    : mlayout{layout}, mpattern{pattern}
{
  moffsets.reserve(mlayout.size());

  for (const Block& block : mlayout) {
    moffsets.push_back(mfloats);
    mfloats += block.count;
  }
}

uint64_t FileModel::floats() const { return mfloats; }

float FileModel::value(uint64_t offset) const
{
  // the last block starting at the offset or before it, the empty blocks
  // share their offsets with the next ones
  const auto next = std::upper_bound(moffsets.begin(), moffsets.end(), offset);
  const auto index = static_cast<size_t>(next - moffsets.begin()) - 1U;

  return mpattern.value(mlayout[index], offset - moffsets[index]);
}

void FileModel::read(uint64_t offset, float* out, uint64_t count) const
{
  const auto next = std::upper_bound(moffsets.begin(), moffsets.end(), offset);
  auto index = static_cast<size_t>(next - moffsets.begin()) - 1U;

  while (count > 0U && index < mlayout.size()) {
    const Block& block = mlayout[index];
    const uint64_t start = offset - moffsets[index];
    const uint64_t taken = std::min(count, block.count - start);
    const PatternValues values = mpattern.of(block);

    for (uint64_t element = 0U; element < taken; ++element) {
      out[element] = values(start + element);
    }

    out += taken;
    offset += taken;
    count -= taken;
    ++index;
  }
}

const std::vector<uint64_t>& FileModel::offsets() const { return moffsets; }

const Layout& FileModel::layout() const { return mlayout; }

bool write_weights_file(const std::string& path, const WeightsHeader& header,
                        const Layout& layout, const Pattern& pattern)
{
  std::ofstream file(path, std::ios::binary | std::ios::trunc);

  if (!file.is_open()) {
    return false;
  }

  write_value(file, header.major);
  write_value(file, header.minor);
  write_value(file, header.revision);

  if (header.seen64()) {
    write_value(file, header.seen);
  } else {
    write_value(file, static_cast<uint32_t>(header.seen));
  }

  std::vector<float> chunk(CHUNK_FLOATS);

  for (const Block& block : layout) {
    const PatternValues values = pattern.of(block);

    for (uint64_t start = 0U; start < block.count; start += CHUNK_FLOATS) {
      const uint64_t count = std::min(CHUNK_FLOATS, block.count - start);

      for (uint64_t index = 0U; index < count; ++index) {
        chunk[index] = values(start + index);
      }

      // cppcheck-suppress invalidPointerCast
      file.write(reinterpret_cast<const char*>(chunk.data()),
                 static_cast<std::streamsize>(count * sizeof(float)));
    }
  }

  file.flush();

  return file.good();
}

::testing::AssertionResult file_matches(const std::string& path,
                                        const WeightsHeader& header,
                                        const Layout& layout,
                                        const Pattern& pattern)
{
  const uint64_t expected_size =
      header.size() + count_floats(layout) * sizeof(float);
  const uint64_t actual_size = file_size(path);

  if (actual_size != expected_size) {
    return ::testing::AssertionFailure()
           << path << " holds " << actual_size << " bytes instead of "
           << expected_size << " (" << layout.size() << " arrays of "
           << count_floats(layout) << " floats after the " << header.size()
           << " bytes header)";
  }

  std::ifstream file(path, std::ios::binary);

  WeightsHeader actual;

  if (!read_value(file, actual.major) || !read_value(file, actual.minor) ||
      !read_value(file, actual.revision)) {
    return ::testing::AssertionFailure() << "short header of " << path;
  }

  if (header.seen64()) {
    read_value(file, actual.seen);
  } else {
    uint32_t seen32 = 0U;
    read_value(file, seen32);
    actual.seen = seen32;
  }

  if (actual.major != header.major || actual.minor != header.minor ||
      actual.revision != header.revision || actual.seen != header.seen) {
    return ::testing::AssertionFailure()
           << "the header of " << path << " is " << actual.describe()
           << " instead of " << header.describe();
  }

  std::vector<float> chunk(CHUNK_FLOATS);

  for (size_t index = 0U; index < layout.size(); ++index) {
    const Block& block = layout[index];
    const PatternValues values = pattern.of(block);

    for (uint64_t start = 0U; start < block.count; start += CHUNK_FLOATS) {
      const uint64_t count = std::min(CHUNK_FLOATS, block.count - start);

      // cppcheck-suppress invalidPointerCast
      file.read(reinterpret_cast<char*>(chunk.data()),
                static_cast<std::streamsize>(count * sizeof(float)));

      if (file.gcount() !=
          static_cast<std::streamsize>(count * sizeof(float))) {
        return ::testing::AssertionFailure()
               << "the " << path << " ends inside the array #" << index << " ("
               << describe(block) << ")";
      }

      for (uint64_t element = 0U; element < count; ++element) {
        const float expected = values(start + element);

        if (bits(chunk[element]) != bits(expected)) {
          return ::testing::AssertionFailure()
                 << "the array #" << index << " (" << describe(block) << ") of "
                 << path << " holds " << chunk[element] << " ("
                 << hex(bits(chunk[element])) << ") at the index "
                 << start + element << " instead of " << expected << " ("
                 << hex(bits(expected)) << ")";
        }
      }
    }
  }

  return ::testing::AssertionSuccess();
}

bool read_header(const std::string& path, WeightsHeader& header)
{
  std::ifstream file(path, std::ios::binary);

  if (!read_value(file, header.major) || !read_value(file, header.minor) ||
      !read_value(file, header.revision)) {
    return false;
  }

  if (header.seen64()) {
    return read_value(file, header.seen);
  }

  uint32_t seen32 = 0U;

  if (!read_value(file, seen32)) {
    return false;
  }

  header.seen = seen32;

  return true;
}

::testing::AssertionResult files_equal(const std::string& first,
                                       const std::string& second)
{
  const uint64_t first_size = file_size(first);
  const uint64_t second_size = file_size(second);

  if (first_size != second_size) {
    return ::testing::AssertionFailure()
           << first << " holds " << first_size << " bytes, while " << second
           << " holds " << second_size;
  }

  std::ifstream first_file(first, std::ios::binary);
  std::ifstream second_file(second, std::ios::binary);

  std::vector<char> first_chunk(CHUNK_FLOATS * sizeof(float));
  std::vector<char> second_chunk(first_chunk.size());

  for (uint64_t offset = 0U; offset < first_size;
       offset += first_chunk.size()) {
    const auto count = static_cast<std::streamsize>(
        std::min<uint64_t>(first_chunk.size(), first_size - offset));

    first_file.read(first_chunk.data(), count);
    second_file.read(second_chunk.data(), count);

    if (first_file.gcount() != count || second_file.gcount() != count) {
      return ::testing::AssertionFailure()
             << "failed to read " << first << " or " << second;
    }

    const auto mismatch = std::mismatch(
        first_chunk.begin(), first_chunk.begin() + count, second_chunk.begin());

    if (mismatch.first != first_chunk.begin() + count) {
      return ::testing::AssertionFailure()
             << first << " and " << second << " differ at the byte "
             << offset +
                    static_cast<uint64_t>(mismatch.first - first_chunk.begin());
    }
  }

  return ::testing::AssertionSuccess();
}

uint64_t file_size(const std::string& path)
{
  std::error_code error;
  const uint64_t size = std::filesystem::file_size(path, error);

  return error ? 0U : size;
}

bool truncate_file(const std::string& path, uint64_t size)
{
  std::error_code error;
  std::filesystem::resize_file(path, size, error);

  return !error;
}

bool append_bytes(const std::string& path, uint64_t count)
{
  std::ofstream file(path, std::ios::binary | std::ios::app);
  const std::vector<char> bytes(count, static_cast<char>(0x5A));

  file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));

  return file.good();
}

uint32_t bits(float value)
{
  uint32_t result = 0U;
  std::memcpy(&result, &value, sizeof(result));
  return result;
}

}  // namespace weights_factory
