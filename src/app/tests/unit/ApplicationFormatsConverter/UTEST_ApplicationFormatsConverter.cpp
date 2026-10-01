#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "src/app/converter/ApplicationFormatsConverter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"

using namespace app;
using namespace app::converter;
using namespace testing;

using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;
using darknet_adaptor::loaders::WeightsFormat;

class UTEST_ApplicationFormatsConverter : public Test
{
 public:
  UTEST_ApplicationFormatsConverter()
  {
    ctx->convert_from = "weights";
    ctx->convert_src = "net.weights";
    ctx->convert_to = "dxxwjz1";
    ctx->convert_dst = "net.dxxwjz1";
    ctx->convert_cfg = "net.cfg";
  }

  ~UTEST_ApplicationFormatsConverter()
  {
    WeightsConverter::onMockCreate = nullptr;
  }

  int argc{0};
  char** argv{nullptr};

  std::shared_ptr<ApplicationContext> ctx{
      std::make_shared<ApplicationContext>(argc, argv)};

  ApplicationFormatsConverter formatsConverter;
};

TEST_F(UTEST_ApplicationFormatsConverter, no_context_error)
{
  MockFunction<void(WeightsConverter&)> onMockCreate;

  EXPECT_CALL(onMockCreate, Call(_)).Times(0);

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  EXPECT_FALSE(formatsConverter.convert({}));
}

TEST_F(UTEST_ApplicationFormatsConverter, conversion_of_the_command_line)
{
  for (const bool result : {true, false}) {
    MockFunction<void(WeightsConverter&)> onMockCreate;

    EXPECT_CALL(onMockCreate, Call(_))
        .Times(1)
        .WillOnce(Invoke([&](WeightsConverter& instance) {
          EXPECT_CALL(instance, convert(_))
              .Times(1)
              .WillOnce(Invoke([&](const WeightsConversion& conversion) {
                EXPECT_EQ(conversion.from, WeightsFormat::Original);
                EXPECT_EQ(conversion.src, "net.weights");
                EXPECT_EQ(conversion.to, WeightsFormat::Dxxwjz1);
                EXPECT_EQ(conversion.dst, "net.dxxwjz1");
                EXPECT_EQ(conversion.cfg, "net.cfg");
                return result;
              }));
        }));

    WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

    EXPECT_EQ(formatsConverter.convert(ctx), result);
  }
}

TEST_F(UTEST_ApplicationFormatsConverter, formats_of_both_directions)
{
  ctx->convert_from = "dxxwjz1";
  ctx->convert_to = "weights";

  MockFunction<void(WeightsConverter&)> onMockCreate;

  EXPECT_CALL(onMockCreate, Call(_))
      .Times(1)
      .WillOnce(Invoke([](WeightsConverter& instance) {
        EXPECT_CALL(instance, convert(_))
            .Times(1)
            .WillOnce(Invoke([](const WeightsConversion& conversion) {
              EXPECT_EQ(conversion.from, WeightsFormat::Dxxwjz1);
              EXPECT_EQ(conversion.to, WeightsFormat::Original);
              return true;
            }));
      }));

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  EXPECT_TRUE(formatsConverter.convert(ctx));
}

TEST_F(UTEST_ApplicationFormatsConverter, unknown_formats_convert_nothing)
{
  MockFunction<void(WeightsConverter&)> onMockCreate;

  EXPECT_CALL(onMockCreate, Call(_)).Times(0);

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  ctx->convert_from = "json";

  EXPECT_FALSE(formatsConverter.convert(ctx));

  ctx->convert_from = "weights";
  ctx->convert_to = "Dxxwjz1";

  EXPECT_FALSE(formatsConverter.convert(ctx));
}

TEST_F(UTEST_ApplicationFormatsConverter, stop_request_reaches_the_converter)
{
  MockFunction<void(WeightsConverter&)> onMockCreate;

  EXPECT_CALL(onMockCreate, Call(_))
      .Times(1)
      .WillOnce(Invoke([&](WeightsConverter& instance) {
        EXPECT_CALL(instance, convert(_))
            .Times(1)
            .WillOnce(Invoke([&](const WeightsConversion& conversion) {
              EXPECT_TRUE(conversion.stop_requested);
              EXPECT_FALSE(conversion.stop_requested());

              ctx->stop = true;

              EXPECT_TRUE(conversion.stop_requested());
              return false;
            }));
      }));

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  EXPECT_FALSE(formatsConverter.convert(ctx));
}
