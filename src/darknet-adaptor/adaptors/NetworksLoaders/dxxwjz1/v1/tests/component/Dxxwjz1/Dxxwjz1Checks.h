#ifndef DARKNETXX_PROJECT_CTEST_DXXWJZ1_CHECKS_H
#define DARKNETXX_PROJECT_CTEST_DXXWJZ1_CHECKS_H

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "DarknetNetwork.h"
#include "NetworkChecks.h"
#include "WeightsFile.h"
#include "WeightsNetSpec.h"
#include "WeightsVariants.h"
#include "WeightsWorkspace.h"

namespace dxxwjz1_test
{

// the generated networks and weights files of the weights factory
using namespace weights_factory;

/**
 * @brief The JSON of the float numbers: the lexer reads the number text with
 * the strtof(), which gives the float the text is the shortest decimal of
 * exactly, instead of the double rounding of the double numbers.
 */
using fjson = nlohmann::basic_json<std::map, std::vector, std::string, bool,
                                   std::int64_t, std::uint64_t, float>;

/// @brief The array of the layer or of the sub-layer.
using ArrayKey = std::tuple<int, SubLayer, ArrayKind>;

/// @brief The count of the floats the document keeps of every array, the
/// absent arrays left out.
using DocumentCounts = std::map<ArrayKey, uint64_t>;

/// @brief The floats of every array of the layout.
constexpr const uint64_t ALL_FLOATS = std::numeric_limits<uint64_t>::max();

/*
 * The dxxwjz1 names of the generated networks, independent of the darknetxx
 * WeightsSchema: the cfg sections of the layer kinds, the layer fields of the
 * sub-layers and of the arrays (the EMA arrays are kept in the places of the
 * regular ones).
 */
std::string type_name(LayerKind kind);
std::string sub_name(SubLayer sub);
std::string array_name(ArrayKind array);

/// @brief The JSON text of the float: the number or the "0x" bit pattern
/// string of the non finite one.
fjson float_json(float value);

/**
 * @brief The dxxwjz1 document of the network made independently of the
 * darknetxx code: every layer of the network (before the cutoff), the arrays
 * of the layout with the pattern values, the first floats of the arrays in
 * the layout order alone for the limit given (the document of the truncated
 * file).
 */
fjson document_of(const NetSpec& spec, const WeightsHeader& header,
                  const Layout& layout, const Pattern& pattern,
                  uint64_t floats = ALL_FLOATS,
                  int cutoff = std::numeric_limits<int>::max());

/// @brief The counts of the floats of the document_of() the same limit.
DocumentCounts counts_of(const Layout& layout, uint64_t floats = ALL_FLOATS);

/// @brief Writes the document: the gzip compressed text (the zlib gzwrite())
/// or the plain one.
bool write_document(const std::string& path, const fjson& document,
                    bool compressed = true);

/// @brief Writes the text as it is: the gzip compressed or the plain one.
bool write_text(const std::string& path, const std::string& text,
                bool compressed = true);

/// @brief Reads the text of the gzip compressed file (the zlib gzread()).
bool read_text(const std::string& path, std::string& text);

/// @brief Reads the document of the dxxwjz1 file, false for the invalid one.
bool read_document(const std::string& path, fjson& document);

/**
 * @brief Checks the whole document independently of the darknetxx code: the
 * format, the version, the header, every layer of the network before the
 * cutoff with its type, the arrays of the layout (the names, the order of
 * the sub-layers, the counts) and every float bit by bit.
 */
::testing::AssertionResult document_matches(
    const fjson& document, const NetSpec& spec, const WeightsHeader& header,
    const Layout& layout, const Pattern& pattern,
    int cutoff = std::numeric_limits<int>::max());

/**
 * @brief Checks the network arrays after the load of the document of the
 * pattern values.
 *
 * @param memory The arrays of the network, which held the before values.
 * @param before The values of the arrays before the load.
 * @param loaded The arrays the loader reads with their transpositions.
 * @param counts The floats the document keeps of every array.
 *
 * The loaded arrays the document keeps get its floats at their beginning
 * (the rest keeps the before values), transposed if the loader transposes
 * them, the rest of the arrays keeps the before values.
 */
::testing::AssertionResult holds_document(
    DarknetNetwork& darknet, const Layout& memory, const Pattern& before,
    const Layout& loaded, const Pattern& pattern, const DocumentCounts& counts);

/// @brief The Dxxwjz1Writer save() and the Dxxwjz1Reader load() of the
/// network, expected to succeed.
void save(network& net, const std::string& path);
void save_upto(network& net, const std::string& path, int cutoff, bool ema);
void load(network& net, const std::string& path);
void load_upto(network& net, const std::string& path, int cutoff);

/// @brief The dxxwjz1 file of the pattern filled network holds the document
/// of its arrays, which match the model bit by bit.
void check_save(const WeightsVariant& variant,
                const WeightsWorkspace& workspace);

/// @brief The load of the independently written document of every header
/// version puts every value to its array, the loader transpositions applied,
/// and leaves the dontload layers and the dontloadscales batch normalization
/// arrays intact.
void check_load(const WeightsVariant& variant,
                const WeightsWorkspace& workspace);

/// @brief The saved network loads bit exact into a new one, which saves the
/// same bytes again.
void check_round_trip(const WeightsVariant& variant,
                      const WeightsWorkspace& workspace);

/// @brief The document of the first floats of the arrays loads them and
/// leaves the rest of the arrays intact.
void check_truncated(const WeightsVariant& variant,
                     const WeightsWorkspace& workspace);

/// @brief The save up to the cutoff and the load up to the cutoff.
void check_cutoff(const WeightsVariant& variant,
                  const WeightsWorkspace& workspace);

/// @brief The save_ema file holds the EMA arrays of the convolutional
/// layers and the regular arrays of the rest.
void check_ema(const WeightsVariant& variant,
               const WeightsWorkspace& workspace);

/**
 * @brief The original weights file of the network converts to the dxxwjz1
 * one of the same arrays and back to the same bytes: the file darknet saves
 * and the independently written one of the variant header, the dxxwjz1 file
 * loads the network the original file does.
 */
void check_conversion(const WeightsVariant& variant,
                      const WeightsWorkspace& workspace);

}  // namespace dxxwjz1_test

#endif  // DARKNETXX_PROJECT_CTEST_DXXWJZ1_CHECKS_H
