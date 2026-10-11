#include "WeightsChecks.h"

#include <algorithm>
#include <iterator>
#include <limits>
#include <random>
#include <set>

#include "NetworkChecks.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsWriter.h"

namespace weights_test
{

using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::OrigWeightsWriter;

namespace
{

/// @brief The loaded arrays are the saved ones: the loader skips nothing and
/// transposes nothing, so the saved file loads and saves the same bytes.
bool reloads_same(const Layout& saved, const Layout& loaded)
{
  if (saved.size() != loaded.size()) {
    return false;
  }

  for (size_t index = 0U; index < saved.size(); ++index) {
    if (saved[index].layer != loaded[index].layer ||
        saved[index].sub != loaded[index].sub ||
        saved[index].array != loaded[index].array ||
        saved[index].count != loaded[index].count || loaded[index].rows != 0U) {
      return false;
    }
  }

  return true;
}

LayoutOptions reading(bool transposed, int cutoff)
{
  LayoutOptions options;
  options.loading = true;
  options.transposed = transposed;
  options.cutoff = cutoff;
  return options;
}

LayoutOptions saving(int cutoff, bool ema)
{
  LayoutOptions options;
  options.cutoff = cutoff;
  options.ema = ema;
  return options;
}

Pattern before_pattern(const WeightsVariant& variant)
{
  return Pattern{~variant.seed};
}

}  // namespace

void save(network& net, const std::string& path)
{
  EXPECT_TRUE(OrigWeightsWriter{}.save(net, path)) << "the save of " << path;
}

void save_upto(network& net, const std::string& path, int cutoff, bool ema)
{
  EXPECT_TRUE(OrigWeightsWriter{}.save(net, path, cutoff, ema))
      << "the save of " << path;
}

void load(network& net, const std::string& path)
{
  EXPECT_TRUE(OrigWeightsReader{}.load(net, path)) << "the load of " << path;
}

void load_upto(network& net, const std::string& path, int cutoff)
{
  EXPECT_TRUE(OrigWeightsReader{}.load(net, path, cutoff))
      << "the load of " << path;
}

void check_save(const WeightsVariant& variant,
                const WeightsWorkspace& workspace)
{
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string weights = workspace.file(variant.name + ".weights");
  const Layout layout = variant.spec.layout();
  const Pattern pattern{variant.seed};

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  ASSERT_TRUE(matches_spec(net, variant.spec, variant.mode));
  ASSERT_TRUE(darknet.fill(layout, pattern));

  *net.cur_iteration = variant.iteration;

  save(net, weights);

  const uint64_t seen =
      saved_seen(variant.spec, variant.mode, variant.iteration);

  EXPECT_EQ(*net.seen, seen);
  EXPECT_TRUE(file_matches(weights, saved_header(seen), layout, pattern));
  // the save leaves the arrays as they are
  EXPECT_TRUE(darknet.holds(layout, pattern));
}

void check_load(const WeightsVariant& variant,
                const WeightsWorkspace& workspace)
{
  const WeightsHeader& header = variant.header;
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string weights = workspace.file(variant.name + ".weights");
  const Layout memory = variant.spec.layout();
  // the file made for the network: no dontload layers, no dontloadscales
  // batch normalization arrays
  const Layout loaded = variant.spec.layout(
      reading(header.transposed(), std::numeric_limits<int>::max()));
  const Pattern pattern{variant.seed};
  const Pattern before = before_pattern(variant);

  ASSERT_TRUE(write_weights_file(weights, header, loaded, pattern));

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  ASSERT_TRUE(matches_spec(net, variant.spec, variant.mode));
  ASSERT_TRUE(darknet.fill(memory, before));

  load(net, weights);

  EXPECT_EQ(*net.seen, header.stored_seen());
  EXPECT_EQ(*net.cur_iteration,
            loaded_iteration(variant.spec, variant.mode, header.stored_seen()));

  const FileModel file{loaded, pattern};

  EXPECT_TRUE(darknet.holds_loaded(memory, before, loaded, file,
                                   file.floats() * sizeof(float)));
}

void check_round_trip(const WeightsVariant& variant,
                      const WeightsWorkspace& workspace)
{
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string first = workspace.file(variant.name + ".weights");
  const std::string second = workspace.file(variant.name + ".resaved.weights");
  const Layout saved = variant.spec.layout();
  const Layout loaded =
      variant.spec.layout(reading(false, std::numeric_limits<int>::max()));
  const Pattern pattern{variant.seed};
  const Pattern before = before_pattern(variant);

  {
    DarknetNetwork darknet{cfg, variant.mode};

    ASSERT_TRUE(darknet.fill(saved, pattern));

    *darknet.net().cur_iteration = variant.iteration;

    save(darknet.net(), first);
  }

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  ASSERT_TRUE(darknet.fill(saved, before));

  load(net, first);

  EXPECT_EQ(*net.seen,
            saved_seen(variant.spec, variant.mode, variant.iteration));
  EXPECT_EQ(*net.cur_iteration, variant.iteration);

  const FileModel file{saved, pattern};

  EXPECT_TRUE(darknet.holds_loaded(saved, before, loaded, file,
                                   file.floats() * sizeof(float)));

  if (reloads_same(saved, loaded)) {
    save(net, second);

    EXPECT_TRUE(files_equal(first, second));
  }
}

void check_truncated(const WeightsVariant& variant,
                     const WeightsWorkspace& workspace)
{
  const WeightsHeader& header = variant.header;
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string weights = workspace.file(variant.name + ".weights");
  const Layout memory = variant.spec.layout();
  const Layout loaded = variant.spec.layout(
      reading(header.transposed(), std::numeric_limits<int>::max()));
  const Pattern pattern{variant.seed};
  const Pattern before = before_pattern(variant);
  const FileModel file{loaded, pattern};

  ASSERT_TRUE(write_weights_file(weights, header, loaded, pattern));

  const uint64_t full = file_size(weights);

  ASSERT_EQ(full, header.size() + file.floats() * sizeof(float));

  if (full == header.size()) {
    // no payload to cut
    return;
  }

  // the cut points: the empty payload, the parts of the first float, the
  // arrays edges, the middle and the tail of the file and the random ones
  std::set<uint64_t> cuts{
      header.size(),      header.size() + 1U,
      header.size() + 5U, header.size() + (full - header.size()) / 2U,
      full - 1U,          full - sizeof(float)};

  const std::vector<uint64_t>& offsets = file.offsets();

  for (size_t index = 1U; index < std::min<size_t>(offsets.size(), 4U);
       ++index) {
    const uint64_t edge = header.size() + offsets[index] * sizeof(float);
    cuts.insert(edge);
    cuts.insert(edge - 2U);
  }

  std::mt19937_64 rng{variant.seed};

  for (int random_cut = 0; random_cut < 2; ++random_cut) {
    cuts.insert(header.size() + rng() % (full - header.size()));
  }

  // the big networks check a part of the cuts
  const size_t limit = count_floats(memory) > 4U * 1024U * 1024U ? 3U : 12U;

  std::vector<uint64_t> ordered;

  std::copy_if(cuts.begin(), cuts.end(), std::back_inserter(ordered),
               [&header, full](uint64_t cut) {
                 return cut >= header.size() && cut < full;
               });

  if (ordered.size() > limit) {
    // the empty payload cut and the evenly spaced part of the rest
    std::vector<uint64_t> part{ordered.front()};
    for (size_t index = 1U; part.size() < limit; ++index) {
      part.push_back(ordered[index * ordered.size() / limit]);
    }
    ordered = part;
  }

  // every next cut makes the same file shorter
  std::sort(ordered.rbegin(), ordered.rend());

  for (const uint64_t cut : ordered) {
    SCOPED_TRACE("the file cut at the " + std::to_string(cut) + " of " +
                 std::to_string(full) + " bytes");

    ASSERT_TRUE(truncate_file(weights, cut));

    DarknetNetwork darknet{cfg, variant.mode};
    network& net = darknet.net();

    ASSERT_TRUE(darknet.fill(memory, before));

    load(net, weights);

    EXPECT_EQ(*net.seen, header.stored_seen());
    EXPECT_TRUE(darknet.holds_loaded(memory, before, loaded, file,
                                     cut - header.size()));
  }
}

void check_cutoff(const WeightsVariant& variant,
                  const WeightsWorkspace& workspace)
{
  const int n = static_cast<int>(variant.spec.layers.size());
  const std::string cfg = workspace.write_cfg(variant.spec);
  const Layout memory = variant.spec.layout();
  const Pattern pattern{variant.seed};
  const Pattern before = before_pattern(variant);
  const uint64_t seen =
      saved_seen(variant.spec, variant.mode, variant.iteration);
  const std::set<int> cutoffs{-1, 0, 1, n / 2, n - 1, n, n + 3};
  const std::string full = workspace.file(variant.name + ".weights");

  {
    DarknetNetwork darknet{cfg, variant.mode};

    ASSERT_TRUE(darknet.fill(memory, pattern));

    *darknet.net().cur_iteration = variant.iteration;

    save(darknet.net(), full);

    for (const int cutoff : cutoffs) {
      SCOPED_TRACE("the save cutoff " + std::to_string(cutoff));

      const std::string cut =
          workspace.file(variant.name + ".upto" + std::to_string(cutoff));

      save_upto(darknet.net(), cut, cutoff, false);

      EXPECT_TRUE(file_matches(cut, saved_header(seen),
                               variant.spec.layout(saving(cutoff, false)),
                               pattern));
    }
  }

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();
  const FileModel full_file{memory, pattern};

  for (const int cutoff : cutoffs) {
    SCOPED_TRACE("the load cutoff " + std::to_string(cutoff));

    ASSERT_TRUE(darknet.fill(memory, before));

    load_upto(net, full, cutoff);

    EXPECT_EQ(*net.seen, seen);
    EXPECT_TRUE(darknet.holds_loaded(
        memory, before, variant.spec.layout(reading(false, cutoff)), full_file,
        full_file.floats() * sizeof(float)));

    // the whole network load of the file saved up to the cutoff
    const FileModel cut_file{variant.spec.layout(saving(cutoff, false)),
                             pattern};

    ASSERT_TRUE(darknet.fill(memory, before));

    load(net, workspace.file(variant.name + ".upto" + std::to_string(cutoff)));

    EXPECT_TRUE(darknet.holds_loaded(
        memory, before,
        variant.spec.layout(reading(false, std::numeric_limits<int>::max())),
        cut_file, cut_file.floats() * sizeof(float)));
  }
}

void check_ema(const WeightsVariant& variant, const WeightsWorkspace& workspace)
{
  ASSERT_EQ(variant.mode, ParseMode::Train)
      << "the inference networks have no EMA arrays";

  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string ema_weights = workspace.file(variant.name + ".ema.weights");
  const std::string weights = workspace.file(variant.name + ".weights");
  const Layout memory = variant.spec.layout();
  const Layout ema =
      variant.spec.layout(saving(std::numeric_limits<int>::max(), true));
  const Pattern pattern{variant.seed};

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  ASSERT_TRUE(darknet.fill(memory, pattern));
  ASSERT_TRUE(darknet.fill(ema, pattern));

  *net.cur_iteration = variant.iteration;

  save_upto(net, ema_weights, net.n, true);
  save(net, weights);

  const uint64_t seen =
      saved_seen(variant.spec, variant.mode, variant.iteration);

  EXPECT_TRUE(file_matches(ema_weights, saved_header(seen), ema, pattern));
  EXPECT_TRUE(file_matches(weights, saved_header(seen), memory, pattern));
}

}  // namespace weights_test
