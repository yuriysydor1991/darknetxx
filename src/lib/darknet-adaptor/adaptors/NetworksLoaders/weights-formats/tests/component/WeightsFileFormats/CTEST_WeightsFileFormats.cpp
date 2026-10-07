#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"

using darknet_adaptor::loaders::NetworkSource;
using darknet_adaptor::loaders::WeightsFileFormats;

TEST(CTEST_WeightsFileFormats, formats_are_of_the_help_message_order)
{
  std::vector<std::string> names;

  for (const auto& format : WeightsFileFormats::all()) {
    names.push_back(format->name());
  }

  EXPECT_EQ(names, (std::vector<std::string>{"weights", "dxxwjz1", "dxxwjz2"}));
}

TEST(CTEST_WeightsFileFormats, names_find_the_formats)
{
  for (const auto& format : WeightsFileFormats::all()) {
    EXPECT_EQ(WeightsFileFormats::named(format->name()), format);
    EXPECT_FALSE(format->description().empty());
    EXPECT_NE(format->create_reader(), nullptr);
    EXPECT_NE(format->create_writer(), nullptr);
  }

  for (const std::string& name : std::vector<std::string>{
           "", "Weights", "dxxwjz", "dxxwjz3", " dxxwjz2", "json"}) {
    EXPECT_EQ(WeightsFileFormats::named(name), nullptr) << name;
  }
}

TEST(CTEST_WeightsFileFormats, extensions_tell_the_formats_of_the_files)
{
  EXPECT_EQ(WeightsFileFormats::of_file("dir/net.dxxwjz2")->name(), "dxxwjz2");
  EXPECT_EQ(WeightsFileFormats::of_file("net_last.dxxwjz1")->name(), "dxxwjz1");
  EXPECT_EQ(WeightsFileFormats::of_file("yolo.weights")->name(), "weights");

  // the original darknet names it's files of any extension
  EXPECT_EQ(WeightsFileFormats::of_file("yolov4.conv.137")->name(), "weights");
  EXPECT_EQ(WeightsFileFormats::of_file("net")->name(), "weights");
}

TEST(CTEST_WeightsFileFormats, the_dxxwjz2_files_alone_keep_the_network)
{
  for (const auto& format : WeightsFileFormats::all()) {
    EXPECT_EQ(format->keeps_network(), format->name() == "dxxwjz2")
        << format->name();
    EXPECT_EQ(format->keeps_structure(), format->name() != "weights")
        << format->name();
  }

  EXPECT_TRUE(NetworkSource::describes("net.dxxwjz2"));
  EXPECT_FALSE(NetworkSource::describes("net.cfg"));
  EXPECT_FALSE(NetworkSource::describes("net.dxxwjz1"));
}

TEST(CTEST_WeightsFileFormats, network_files_name_the_training_files)
{
  EXPECT_EQ(NetworkSource::base_of("cfg/net.cfg", 400), "net");
  EXPECT_EQ(NetworkSource::base_of("net_last.cfg", 400), "net_last");
  EXPECT_EQ(NetworkSource::base_of("backup/net_last.dxxwjz2", 400), "net");
  EXPECT_EQ(NetworkSource::base_of("net_final.dxxwjz2", 0), "net");
  EXPECT_EQ(NetworkSource::base_of("net_400.dxxwjz2", 400), "net");

  // the number other than the iteration of the weights is of the name
  EXPECT_EQ(NetworkSource::base_of("yolov4_416.dxxwjz2", 400), "yolov4_416");
  EXPECT_EQ(NetworkSource::base_of("_last.dxxwjz2", 0), "_last");
}
