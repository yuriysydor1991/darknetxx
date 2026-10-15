#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>

#include "LibraryFacade.h"
#include "src/gui/net-editor/NetworkProject.h"
#include "src/gui/net-editor/SectionSchemas.h"

using net_editor::NetworkProject;
using net_editor::SectionSchemas;

namespace fs = std::filesystem;

class CTEST_NetworkProject : public testing::Test
{
 public:
  void SetUp() override
  {
    fs::remove_all(dir);
    fs::create_directories(dir);
  }

  void TearDown() override { fs::remove_all(dir); }

  std::string file(const std::string& name) const
  {
    return (dir / name).string();
  }

  /// @brief The new network of a convolutional and a maxpool layers.
  void compose()
  {
    for (const std::string type : {"convolutional", "maxpool"}) {
      project.model.add(type, SectionSchemas::of(type).initial());
    }

    project.model.classes = {"cat", "dog"};
  }

  fs::path dir{fs::path{CTEST_NetworkProject_DATA_DIR} /
               testing::UnitTest::GetInstance()->current_test_info()->name()};
  NetworkProject project;
  std::string message;
};

TEST_F(CTEST_NetworkProject, the_new_network_saves_and_opens_back)
{
  EXPECT_EQ(project.title(), "New network");
  EXPECT_FALSE(project.modified());

  compose();

  EXPECT_TRUE(project.modified());
  EXPECT_EQ(project.title(), "New network *");
  ASSERT_TRUE(project.save(file("net.dxxwjz3p"), message)) << message;
  EXPECT_EQ(message, "");
  EXPECT_FALSE(project.modified());
  EXPECT_EQ(project.path(), file("net.dxxwjz3p"));
  EXPECT_EQ(project.title(), "net.dxxwjz3p");

  NetworkProject opened;
  std::string cfg;
  std::vector<std::string> classes;

  ASSERT_TRUE(opened.open(file("net.dxxwjz3p"), message));
  EXPECT_EQ(opened.model.cfg(), project.model.cfg());
  EXPECT_EQ(opened.model.classes, project.model.classes);
  EXPECT_FALSE(opened.modified());

  // the saved project is the network to train
  ASSERT_TRUE(darknetxx011::LibraryFacade::network_description(
      file("net.dxxwjz3p"), cfg, classes));
  EXPECT_EQ(cfg, project.model.cfg());
}

TEST_F(CTEST_NetworkProject, the_wrong_options_are_not_saved)
{
  compose();
  project.model.net()->set("subdivisions", "7");

  EXPECT_FALSE(project.save(file("net.dxxwjz3p"), message));
  EXPECT_EQ(message,
            "The network has 1 wrong options:\n[net] subdivisions: The 7 "
            "doesn't divide the batch=64");
  EXPECT_FALSE(fs::exists(file("net.dxxwjz3p")));
  EXPECT_TRUE(project.modified());
}

TEST_F(CTEST_NetworkProject, the_darknet_refusal_is_told)
{
  // the route of the layers of the different sizes passes the editor
  // checks, the training of the darknet can't run it
  compose();
  project.model.add("route", {{"layers", "-1,0"}});

  EXPECT_FALSE(project.save(file("net.dxxwjz3p"), message));
  EXPECT_EQ(message,
            "The layer 2 routes the layers of the different sizes: 208 x 208 "
            "and 416 x 416");
  EXPECT_FALSE(fs::exists(file("net.dxxwjz3p")));
}

TEST_F(CTEST_NetworkProject, the_file_of_no_network_is_refused)
{
  compose();

  EXPECT_FALSE(project.open(file("none.dxxwjz3p"), message));
  EXPECT_EQ(message, "The " + file("none.dxxwjz3p") + " file keeps no network");
  EXPECT_EQ(project.model.layers().size(), 2U);
  EXPECT_TRUE(project.modified());
}

TEST_F(CTEST_NetworkProject, the_edited_project_saves_into_itself)
{
  compose();
  ASSERT_TRUE(project.save(file("net.dxxwjz3p"), message));

  project.model.net()->set("max_batches", "8000");
  project.model.net()->set("steps", "6400,7200");

  EXPECT_TRUE(project.modified());
  ASSERT_TRUE(project.save(project.path(), message)) << message;

  NetworkProject opened;

  ASSERT_TRUE(opened.open(file("net.dxxwjz3p"), message));
  EXPECT_EQ(*opened.model.net()->find("max_batches"), "8000");

  // the saved copy goes on as the project
  ASSERT_TRUE(opened.save(file("copy.dxxwjz3p"), message));
  EXPECT_EQ(opened.path(), file("copy.dxxwjz3p"));
}

TEST_F(CTEST_NetworkProject, the_chosen_name_of_no_extension_is_the_project)
{
  EXPECT_EQ(NetworkProject::project_of("/a/net"), "/a/net.dxxwjz3p");
  EXPECT_EQ(NetworkProject::project_of("/a.b/net"), "/a.b/net.dxxwjz3p");
  EXPECT_EQ(NetworkProject::project_of("/a/net.dxxwjz3"), "/a/net.dxxwjz3");
}
