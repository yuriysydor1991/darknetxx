#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "src/app/ApplicationFactory.h"
#include "src/app/RecognitionGuiApplication.h"

using namespace app;

// the rest of the factory is the training one (see the
// UTEST_TrainGuiApplicationFactory)
TEST(UTEST_RecognitionGuiApplicationFactory,
     default_application_is_the_recognition_window)
{
  EXPECT_NE(
      std::dynamic_pointer_cast<RecognitionGuiApplication>(
          std::make_shared<ApplicationFactory>()->create_default_application()),
      nullptr);
}
