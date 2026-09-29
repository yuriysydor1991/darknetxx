#ifndef DARKNETXX_PROJECT_WEIGHTS_FACTORY_FILE_H
#define DARKNETXX_PROJECT_WEIGHTS_FACTORY_FILE_H

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

#include "WeightsNetSpec.h"

namespace weights_factory
{

/**
 * @brief The weights file header: the version triple and the count of the
 * images the network has seen, stored as 64 bits since the 0.2 version.
 */
struct WeightsHeader
{
  int32_t major{0};
  int32_t minor{2};
  int32_t revision{5};
  uint64_t seen{0U};

  /// @brief The (major * 10 + minor) >= 2 files keep the 64 bits seen.
  bool seen64() const;

  /// @brief The loader transposes the connected weights of the major or
  /// minor > 1000 files.
  bool transposed() const;

  /// @brief The header size in bytes.
  uint64_t size() const;

  /// @brief The seen value the loader gets: the 32 bits old files truncate it.
  uint64_t stored_seen() const;

  std::string describe() const;
};

/**
 * @brief The pattern values of one array. The hot loops of the big arrays
 * reuse it instead of the Pattern::value calls.
 */
struct PatternValues
{
  uint64_t key{0U};
  bool positive{false};

  float operator()(uint64_t index) const;
};

/**
 * @brief The deterministic values of the weights file arrays. Every value
 * depends on the seed, the array and the element index, so the misplaced,
 * the swapped and the shifted arrays don't match. The values are the exact
 * multiples of 2^-23 in the [-1, 1) range, the rolling variances get the
 * positive [0.5, 1) ones, which keeps the batch normalization fusion finite.
 */
class Pattern
{
 public:
  explicit Pattern(uint64_t seed);

  PatternValues of(const Block& block) const;

  float value(const Block& block, uint64_t index) const;

 private:
  uint64_t mseed;
};

/**
 * @brief The content of a weights file written by the given layout: tells the
 * value of every float of the payload.
 */
class FileModel
{
 public:
  FileModel(const Layout& layout, const Pattern& pattern);

  /// @brief The count of the payload floats.
  uint64_t floats() const;

  /// @brief The value of the float at the given payload float offset.
  float value(uint64_t offset) const;

  /// @brief Copies the count of the payload floats from the given offset.
  void read(uint64_t offset, float* out, uint64_t count) const;

  /// @brief The payload float offset of every block of the layout.
  const std::vector<uint64_t>& offsets() const;

  const Layout& layout() const;

 private:
  Layout mlayout;
  Pattern mpattern;
  std::vector<uint64_t> moffsets;
  uint64_t mfloats{0U};
};

/**
 * @brief Writes the weights file independently of the darknet code: the
 * header followed by the pattern values of the layout arrays. Returns false
 * on the write failure.
 */
bool write_weights_file(const std::string& path, const WeightsHeader& header,
                        const Layout& layout, const Pattern& pattern);

/**
 * @brief Checks the whole file content (the header, every float of every
 * array bit by bit, no trailing bytes) independently of the darknet code.
 */
::testing::AssertionResult file_matches(const std::string& path,
                                        const WeightsHeader& header,
                                        const Layout& layout,
                                        const Pattern& pattern);

/// @brief Reads the header of the file, false for the short file.
bool read_header(const std::string& path, WeightsHeader& header);

::testing::AssertionResult files_equal(const std::string& first,
                                       const std::string& second);

uint64_t file_size(const std::string& path);

/// @brief Cuts the file down to the given size.
bool truncate_file(const std::string& path, uint64_t size);

/// @brief Appends the given count of the bytes to the file.
bool append_bytes(const std::string& path, uint64_t count);

uint32_t bits(float value);

}  // namespace weights_factory

#endif  // DARKNETXX_PROJECT_WEIGHTS_FACTORY_FILE_H
