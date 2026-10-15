#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "src/app/ApplicationFactory.h"
#include "src/app/applications/NetEditorQtGuiApplication.h"

using namespace app;

// the rest of the factory is the training one (see the
// UTEST_TrainGuiApplicationFactory)
TEST(UTEST_NetEditorQtGuiApplicationFactory,
     default_application_is_the_network_editor_window)
{
  EXPECT_NE(
      std::dynamic_pointer_cast<NetEditorQtGuiApplication>(
          std::make_shared<ApplicationFactory>()->create_default_application()),
      nullptr);
}
