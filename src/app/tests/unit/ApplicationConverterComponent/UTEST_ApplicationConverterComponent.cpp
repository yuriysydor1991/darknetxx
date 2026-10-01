#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <csignal>
#include <memory>

#include "src/app/converter/ApplicationConverterComponent.h"
#include "src/app/converter/IFormatsConverter.h"

using namespace app;
using namespace app::converter;
using namespace testing;

namespace
{

class FormatsConverterMock : public IFormatsConverter
{
 public:
  MOCK_METHOD(bool, convert, (std::shared_ptr<ApplicationContext> ctx),
              (override));
};

}  // namespace

class UTEST_ApplicationConverterComponent : public Test
{
 public:
  std::shared_ptr<ApplicationContext> ctx{
      std::make_shared<ApplicationContext>(argc, argv)};
  std::shared_ptr<FormatsConverterMock> formatsConverter{
      std::make_shared<FormatsConverterMock>()};

  int argc{0};
  char** argv{nullptr};
};

TEST_F(UTEST_ApplicationConverterComponent, no_context_error)
{
  ApplicationConverterComponent component{formatsConverter};

  EXPECT_CALL(*formatsConverter, convert(_)).Times(0);

  EXPECT_EQ(component.run({}), IApplication::INVALID);
}

TEST_F(UTEST_ApplicationConverterComponent, no_converter_error)
{
  ApplicationConverterComponent component{nullptr};

  EXPECT_EQ(component.run(ctx), IApplication::INVALID);
}

TEST_F(UTEST_ApplicationConverterComponent, successful_conversion_is_zero)
{
  ApplicationConverterComponent component{formatsConverter};

  EXPECT_CALL(*formatsConverter, convert(ctx)).Times(1).WillOnce(Return(true));

  EXPECT_EQ(component.run(ctx), 0);
}

TEST_F(UTEST_ApplicationConverterComponent, failed_conversion_is_invalid)
{
  ApplicationConverterComponent component{formatsConverter};

  EXPECT_CALL(*formatsConverter, convert(ctx)).Times(1).WillOnce(Return(false));

  EXPECT_EQ(component.run(ctx), IApplication::INVALID);
}

TEST_F(UTEST_ApplicationConverterComponent, stopped_conversion_is_the_signal)
{
  ApplicationConverterComponent component{formatsConverter};

  EXPECT_CALL(*formatsConverter, convert(ctx))
      .Times(1)
      .WillOnce(Invoke([](std::shared_ptr<ApplicationContext> stopped) {
        stopped->stop_signal = SIGTERM;
        stopped->stop = true;
        return false;
      }));

  // the exit status a shell reports for a process the signal killed
  EXPECT_EQ(component.run(ctx), 128 + SIGTERM);
}
