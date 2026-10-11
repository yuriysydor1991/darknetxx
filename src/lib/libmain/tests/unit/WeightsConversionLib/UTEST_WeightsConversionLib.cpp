#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <utility>

#include "LibraryContext.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/lib/libmain/WeightsConversionLib.h"

using namespace darknetxx_impl;
using namespace testing;

using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;

class UTEST_WeightsConversionLib : public Test
{
 public:
  UTEST_WeightsConversionLib()
  {
    ctx->convert_from = "weights";
    ctx->convert_src = "net.weights";
    ctx->convert_to = "dxxwjz1";
    ctx->convert_dst = "net.dxxwjz1";
    ctx->convert_cfg = "net.cfg";
    ctx->convert_names = "net.names";
  }

  ~UTEST_WeightsConversionLib() { WeightsConverter::onMockCreate = nullptr; }

  darknetxx011::LibraryContextPtr ctx{
      std::make_shared<darknetxx011::LibraryContext>()};

  WeightsConversionLib conversionLib;
};

TEST_F(UTEST_WeightsConversionLib, no_context_error)
{
  MockFunction<void(WeightsConverter&)> onMockCreate;

  EXPECT_CALL(onMockCreate, Call(_)).Times(0);

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  EXPECT_FALSE(conversionLib.libcall({}));
}

TEST_F(UTEST_WeightsConversionLib, conversion_of_the_command_line)
{
  for (const bool result : {true, false}) {
    MockFunction<void(WeightsConverter&)> onMockCreate;

    EXPECT_CALL(onMockCreate, Call(_))
        .Times(1)
        .WillOnce(Invoke([&](WeightsConverter& instance) {
          EXPECT_CALL(instance, convert(_))
              .Times(1)
              .WillOnce(Invoke([&](const WeightsConversion& conversion) {
                EXPECT_EQ(conversion.from->name(), "weights");
                EXPECT_EQ(conversion.src, "net.weights");
                EXPECT_EQ(conversion.to->name(), "dxxwjz1");
                EXPECT_EQ(conversion.dst, "net.dxxwjz1");
                EXPECT_EQ(conversion.cfg, "net.cfg");
                EXPECT_EQ(conversion.names, "net.names");
                return result;
              }));
        }));

    WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

    EXPECT_EQ(conversionLib.libcall(ctx), result);
  }
}

TEST_F(UTEST_WeightsConversionLib, formats_of_both_directions)
{
  ctx->convert_from = "dxxwjz2";
  ctx->convert_to = "weights";

  MockFunction<void(WeightsConverter&)> onMockCreate;

  EXPECT_CALL(onMockCreate, Call(_))
      .Times(1)
      .WillOnce(Invoke([](WeightsConverter& instance) {
        EXPECT_CALL(instance, convert(_))
            .Times(1)
            .WillOnce(Invoke([](const WeightsConversion& conversion) {
              EXPECT_EQ(conversion.from->name(), "dxxwjz2");
              EXPECT_EQ(conversion.to->name(), "weights");
              return true;
            }));
      }));

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  EXPECT_TRUE(conversionLib.libcall(ctx));
}

TEST_F(UTEST_WeightsConversionLib,
       yolo4_folder_converts_into_the_project_and_back)
{
  MockFunction<void(WeightsConverter&)> onMockCreate;

  EXPECT_CALL(onMockCreate, Call(_))
      .Times(2)
      .WillRepeatedly(Invoke([this](WeightsConverter& instance) {
        EXPECT_CALL(instance, convert(_))
            .Times(1)
            .WillOnce(Invoke([this](const WeightsConversion& conversion) {
              EXPECT_EQ(conversion.from->name(), ctx->convert_from);
              EXPECT_EQ(conversion.to->name(), ctx->convert_to);
              return true;
            }));
      }));

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  for (const auto& [from, to] :
       {std::pair{"yolo4", "dxxwjz2p"}, std::pair{"dxxwjz2p", "yolo4"}}) {
    ctx->convert_from = from;
    ctx->convert_to = to;

    EXPECT_TRUE(conversionLib.libcall(ctx)) << from << " into " << to;
  }
}

TEST_F(UTEST_WeightsConversionLib, unknown_formats_convert_nothing)
{
  MockFunction<void(WeightsConverter&)> onMockCreate;

  EXPECT_CALL(onMockCreate, Call(_)).Times(0);

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  ctx->convert_from = "json";

  EXPECT_FALSE(conversionLib.libcall(ctx));

  ctx->convert_from = "weights";
  ctx->convert_to = "Dxxwjz1";

  EXPECT_FALSE(conversionLib.libcall(ctx));
}

TEST_F(UTEST_WeightsConversionLib, stop_request_reaches_the_converter)
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

              ctx->request_stop();

              EXPECT_TRUE(conversion.stop_requested());
              return false;
            }));
      }));

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  EXPECT_FALSE(conversionLib.libcall(ctx));
}

TEST_F(UTEST_WeightsConversionLib, no_source_makes_the_network_to_train)
{
  MockFunction<void(WeightsConverter&)> onMockCreate;

  ctx->convert_from.clear();
  ctx->convert_src.clear();
  ctx->convert_to = "dxxwjz2";

  EXPECT_CALL(onMockCreate, Call(_))
      .Times(1)
      .WillOnce(Invoke([&](WeightsConverter& instance) {
        EXPECT_CALL(instance, convert(_))
            .Times(1)
            .WillOnce(Invoke([&](const WeightsConversion& conversion) {
              EXPECT_EQ(conversion.from, nullptr);
              EXPECT_TRUE(conversion.src.empty());
              EXPECT_EQ(conversion.to->name(), "dxxwjz2");
              EXPECT_EQ(conversion.cfg, "net.cfg");
              return true;
            }));
      }));

  WeightsConverter::onMockCreate = onMockCreate.AsStdFunction();

  EXPECT_TRUE(conversionLib.libcall(ctx));
}
