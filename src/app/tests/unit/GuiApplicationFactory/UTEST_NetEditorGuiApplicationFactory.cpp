#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "src/app/ApplicationFactory.h"
#include "src/app/applications/NetEditorGuiApplication.h"

using namespace app;

// the rest of the factory is the training one (see the
// UTEST_TrainGuiApplicationFactory)
TEST(UTEST_NetEditorGuiApplicationFactory,
     default_application_is_the_network_editor_window)
{
  EXPECT_NE(
      std::dynamic_pointer_cast<NetEditorGuiApplication>(
          std::make_shared<ApplicationFactory>()->create_default_application()),
      nullptr);
}
