#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "LibraryContext.h"
#include "Yolo4Folder.h"

using darknetxx011::Yolo4Folder;

namespace fs = std::filesystem;

class CTEST_Yolo4Folder : public testing::Test
{
 public:
  CTEST_Yolo4Folder()
      : root{fs::path{CTEST_Yolo4Folder_DATA_DIR} /
             testing::UnitTest::GetInstance()->current_test_info()->name()},
        workingDir{fs::current_path()}
  {
    fs::remove_all(root);
    fs::create_directories(root / "cfg");
  }

  ~CTEST_Yolo4Folder() override
  {
    fs::current_path(workingDir);
    fs::remove_all(root);
  }

  void write(const fs::path& file, const std::string& text = "")
  {
    fs::create_directories((root / file).parent_path());
    std::ofstream{root / file} << text;
  }

  /// @brief Writes the file the given seconds older than the others.
  void write_older(const fs::path& file, const int seconds)
  {
    write(file);
    fs::last_write_time(root / file, fs::last_write_time(root / file) -
                                         std::chrono::seconds{seconds});
  }

  fs::path root;
  fs::path workingDir;
};

TEST_F(CTEST_Yolo4Folder, no_cfg_is_no_training)
{
  write("data/obj.names");

  EXPECT_FALSE(Yolo4Folder{root.string()}.valid());
  EXPECT_FALSE(Yolo4Folder{""}.valid());
}

TEST_F(CTEST_Yolo4Folder, the_first_cfg_by_the_name_is_trained)
{
  write("cfg/b.cfg", "[net]\n");
  write("cfg/a.cfg", "[net]\n");
  write("cfg/notes.txt");

  const Yolo4Folder folder{root.string()};

  ASSERT_TRUE(folder.valid());
  EXPECT_EQ(folder.cfg(), (root / "cfg" / "a.cfg").string());
  EXPECT_TRUE(folder.latest_weights().empty());
  EXPECT_TRUE(folder.chart().empty());
}

TEST_F(CTEST_Yolo4Folder, the_last_weights_are_resumed)
{
  write("cfg/net.cfg", "[net]\n");
  write_older("backup/net_last.weights", 60);
  write("backup/net_1000.weights");

  EXPECT_EQ(Yolo4Folder{root.string()}.latest_weights(),
            (root / "backup" / "net_last.weights").string());
}

TEST_F(CTEST_Yolo4Folder, the_newest_last_weights_of_any_format_are_resumed)
{
  write("cfg/net.cfg", "[net]\n");
  write_older("backup/net_last.weights", 120);
  write_older("backup/net_last.dxxwjz2", 60);
  write("backup/net_2000.dxxwjz2");

  EXPECT_EQ(Yolo4Folder{root.string()}.latest_weights(),
            (root / "backup" / "net_last.dxxwjz2").string());
}

TEST_F(CTEST_Yolo4Folder, the_chart_of_the_latest_training_is_the_chart)
{
  write("cfg/net.cfg", "[net]\n");
  write("chart_net.png");
  write("chart_net_2026-02-01--10-00-00.png");
  write("chart_net_2026-01-01--10-00-00.png");
  write("chart_net2_2027-01-01--10-00-00.png");
  // the charts of the other cfg, which the base of this one begins
  write("chart_net_tiny_2027-01-01--10-00-00.png");
  write("chart_net_2027-01-01.png");
  write("chart.png");

  EXPECT_EQ(Yolo4Folder{root.string()}.chart(),
            (root / "chart_net_2026-02-01--10-00-00.png").string());
}

TEST_F(CTEST_Yolo4Folder, the_newest_weights_of_the_cfg_without_the_last)
{
  write("cfg/net.v2.cfg", "[net]\n");
  write_older("backup/net_1000.weights", 60);
  write("backup/net_final.weights");
  write("backup/other_last.weights");
  write("backup/net_2000.txt");

  EXPECT_EQ(Yolo4Folder{root.string()}.latest_weights(),
            (root / "backup" / "net_final.weights").string());
}

TEST_F(CTEST_Yolo4Folder, the_backup_key_of_the_cfg_names_the_weights_folder)
{
  write("cfg/net.cfg", "[net]\nbackup=saved\n");
  write("backup/net_last.weights");
  write("saved/net_last.weights");

  EXPECT_EQ(Yolo4Folder{root.string()}.latest_weights(),
            (root / "saved" / "net_last.weights").string());
}

TEST_F(CTEST_Yolo4Folder, enter_points_the_context_to_the_folder)
{
  darknetxx011::LibraryContext actx;

  write("cfg/net.cfg", "[net]\n");
  write("data/obj.names", "a\n");
  write("chart_net.png");
  write("backup/net_last.weights");

  actx.detect_cfg = "detect.cfg";

  const Yolo4Folder folder{root.string()};

  ASSERT_TRUE(folder.enter(actx));

  EXPECT_EQ(fs::current_path(), root);
  EXPECT_TRUE(actx.detect_cfg.empty());
  EXPECT_EQ(actx.train_cfg, folder.cfg());
  EXPECT_EQ(actx.latest_weights, folder.latest_weights());
  EXPECT_EQ(actx.names, (root / "data" / "obj.names").string());
  EXPECT_EQ(actx.chart_path, folder.chart());
}

TEST_F(CTEST_Yolo4Folder, enter_creates_the_backup_folder)
{
  darknetxx011::LibraryContext actx;

  write("cfg/net.cfg", "[net]\n");

  ASSERT_TRUE(Yolo4Folder{root.string()}.enter(actx));

  EXPECT_TRUE(fs::is_directory(root / "backup"));
  EXPECT_TRUE(actx.names.empty());
  EXPECT_TRUE(actx.chart_path.empty());
  EXPECT_TRUE(actx.latest_weights.empty());
}
