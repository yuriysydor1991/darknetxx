#include <gtest/gtest.h>

#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"

using namespace testing;

using darknet_adaptor::adaptors::ALayerAdaptor;
using darknet_adaptor::adaptors::ALayerAdaptorPtr;
using darknet_adaptor::adaptors::layerptr;

namespace
{

/// @brief The layer array of a network, the way the ANetworkAdaptor keeps it.
using Slots = std::shared_ptr<std::vector<layer>>;

Slots make_slots(size_t count)
{
  // the value initialized C structs are the zeroed ones
  return std::make_shared<std::vector<layer>>(count);
}

layerptr slot_of(const Slots& slots, size_t index)
{
  return layerptr(slots, &(*slots)[index]);
}

/// @brief A float buffer field of the layer with it's setters and getter.
struct FloatField
{
  std::string name;
  std::function<void(ALayerAdaptor&, float*)> borrow;
  std::function<void(ALayerAdaptor&, std::vector<float>&&)> own;
  std::function<const std::vector<float>&(const ALayerAdaptor&)> get;
  float* layer::* field;
};

#define FLOAT_FIELD(name)                                          \
  FloatField                                                       \
  {                                                                \
    #name, [](ALayerAdaptor& a, float* p) { a.borrow_##name(p); }, \
        [](ALayerAdaptor& a, std::vector<float>&& v) {             \
          a.name(std::move(v));                                    \
        },                                                         \
        [](const ALayerAdaptor& a) -> const std::vector<float>& {  \
          return a.name();                                         \
        },                                                         \
        &layer::name                                               \
  }

/// @brief The float buffers the layers borrow of the other layers.
std::vector<FloatField> borrowed_float_fields()
{
  return {FLOAT_FIELD(output),       FLOAT_FIELD(delta),
          FLOAT_FIELD(weights),      FLOAT_FIELD(weight_updates),
          FLOAT_FIELD(biases),       FLOAT_FIELD(bias_updates),
          FLOAT_FIELD(scales),       FLOAT_FIELD(scale_updates),
          FLOAT_FIELD(mean),         FLOAT_FIELD(variance),
          FLOAT_FIELD(mean_delta),   FLOAT_FIELD(variance_delta),
          FLOAT_FIELD(rolling_mean), FLOAT_FIELD(rolling_variance)};
}

#undef FLOAT_FIELD

}  // namespace

class CTEST_ALayerAdaptor : public Test
{
 public:
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
};

TEST_F(CTEST_ALayerAdaptor, a_new_adaptor_keeps_the_zeroed_struct)
{
  const layerptr view = adaptor->get();

  ASSERT_NE(view, nullptr);

  layer zeroed;
  std::memset(&zeroed, 0, sizeof(zeroed));

  EXPECT_EQ(std::memcmp(view.get(), &zeroed, sizeof(layer)), 0);
}

TEST_F(CTEST_ALayerAdaptor, the_view_keeps_the_adaptor_alive)
{
  adaptor->output(std::vector<float>{1.f, 2.f, 3.f});

  const std::weak_ptr<ALayerAdaptor> watcher = adaptor;
  layerptr view = adaptor->get();

  adaptor.reset();

  ASSERT_FALSE(watcher.expired());
  EXPECT_FLOAT_EQ(view->output[2], 3.f);

  view.reset();

  EXPECT_TRUE(watcher.expired());
}

TEST_F(CTEST_ALayerAdaptor,
       get_of_the_adaptor_not_owned_by_a_shared_pointer_throws)
{
  ALayerAdaptor unowned;

  EXPECT_THROW(unowned.get(), std::bad_weak_ptr);
}

TEST_F(CTEST_ALayerAdaptor, the_setters_point_the_fields_at_the_own_buffers)
{
  const layerptr l = adaptor->get();

  adaptor->output(std::vector<float>(6, 0.5f));
  adaptor->indexes(std::vector<int>{1, 2});
  adaptor->align_bit_weights(std::string(5, '\x7f'));

  EXPECT_EQ(l->output, adaptor->output().data());
  EXPECT_EQ(adaptor->output().size(), 6U);
  EXPECT_EQ(l->indexes, adaptor->indexes().data());
  EXPECT_EQ(l->align_bit_weights, adaptor->align_bit_weights().data());

  // the empty containers are the absent buffers of darknet
  adaptor->output(std::vector<float>{});
  adaptor->indexes(std::vector<int>{});
  adaptor->align_bit_weights(std::string{});

  EXPECT_EQ(l->output, nullptr);
  EXPECT_EQ(l->indexes, nullptr);
  EXPECT_EQ(l->align_bit_weights, nullptr);
}

TEST_F(CTEST_ALayerAdaptor, the_copying_setters_keep_their_own_copy)
{
  std::vector<float> values{1.f, 2.f};

  adaptor->weights(values);
  values[0] = 5.f;

  EXPECT_NE(adaptor->get()->weights, values.data());
  EXPECT_FLOAT_EQ(adaptor->get()->weights[0], 1.f);
  EXPECT_EQ(adaptor->weights().size(), 2U);
}

TEST_F(CTEST_ALayerAdaptor, the_borrowed_buffers_are_not_owned)
{
  for (const FloatField& field : borrowed_float_fields()) {
    SCOPED_TRACE(field.name);

    const ALayerAdaptorPtr owner = ALayerAdaptor::create();
    const ALayerAdaptorPtr borrower = ALayerAdaptor::create();

    field.own(*owner, std::vector<float>(4, 2.f));
    field.own(*borrower, std::vector<float>(8, 1.f));

    float* const buffer = (*owner->get()).*field.field;

    field.borrow(*borrower, buffer);

    // the own buffer is released, the field is the owner's one
    EXPECT_TRUE(field.get(*borrower).empty());
    EXPECT_EQ((*borrower->get()).*field.field, buffer);
    EXPECT_EQ(field.get(*owner).size(), 4U);

    // an own buffer replaces the borrowed one
    field.own(*borrower, std::vector<float>(3, 0.f));

    EXPECT_EQ((*borrower->get()).*field.field, field.get(*borrower).data());
    EXPECT_EQ((*owner->get()).*field.field, buffer);

    // borrowing none is the absent buffer
    field.borrow(*borrower, nullptr);

    EXPECT_EQ((*borrower->get()).*field.field, nullptr);
    EXPECT_TRUE(field.get(*borrower).empty());
  }
}

TEST_F(CTEST_ALayerAdaptor, the_borrowed_yolo_labels_are_not_owned)
{
  const ALayerAdaptorPtr yolo = ALayerAdaptor::create();

  yolo->labels(std::vector<int>(10, -1));
  yolo->class_ids(std::vector<int>(10, -1));

  adaptor->labels(std::vector<int>(3, 0));
  adaptor->class_ids(std::vector<int>(3, 0));
  adaptor->borrow_labels(yolo->get()->labels);
  adaptor->borrow_class_ids(yolo->get()->class_ids);

  EXPECT_EQ(adaptor->get()->labels, yolo->labels().data());
  EXPECT_EQ(adaptor->get()->class_ids, yolo->class_ids().data());
  EXPECT_TRUE(adaptor->labels().empty());
  EXPECT_TRUE(adaptor->class_ids().empty());
}

TEST_F(CTEST_ALayerAdaptor, bind_moves_the_struct_into_the_slot)
{
  adaptor->type(CONVOLUTIONAL);
  adaptor->batch(3);
  adaptor->output(std::vector<float>(4, 1.f));

  Slots slots = make_slots(2);
  layer* const target = &(*slots)[1];
  const std::weak_ptr<std::vector<layer>> slots_watcher = slots;

  adaptor->bind(slot_of(slots, 1));
  slots.reset();

  // the adaptor keeps the network array of it's slot alive
  ASSERT_FALSE(slots_watcher.expired());

  EXPECT_EQ(adaptor->get().get(), target);
  EXPECT_EQ(target->type, CONVOLUTIONAL);
  EXPECT_EQ(target->batch, 3);
  EXPECT_EQ(target->output, adaptor->output().data());

  // the darknet changes of the slot are the adaptor ones
  target->batch = 7;
  EXPECT_EQ(adaptor->batch(), 7);

  // the setters change the slot
  adaptor->delta(std::vector<float>(4, 0.f));
  EXPECT_EQ(target->delta, adaptor->delta().data());

  // binding the slot again changes nothing
  adaptor->bind(adaptor->get());
  EXPECT_EQ(adaptor->get().get(), target);
  EXPECT_EQ(target->batch, 7);

  adaptor.reset();

  EXPECT_TRUE(slots_watcher.expired());
}

TEST_F(CTEST_ALayerAdaptor, relink_follows_the_bound_layers)
{
  const ALayerAdaptorPtr child = ALayerAdaptor::create();
  const ALayerAdaptorPtr shared = ALayerAdaptor::create();

  adaptor->input_layer(child);
  adaptor->share_layer(shared);

  EXPECT_EQ(adaptor->get()->input_layer, child->get().get());
  EXPECT_EQ(adaptor->get()->share_layer, shared->get().get());

  const Slots slots = make_slots(2);

  shared->bind(slot_of(slots, 0));
  child->bind(slot_of(slots, 1));

  adaptor->relink();

  EXPECT_EQ(adaptor->get()->share_layer, &(*slots)[0]);
  EXPECT_EQ(adaptor->get()->input_layer, &(*slots)[1]);
  EXPECT_EQ(adaptor->input_layer(), child);
  EXPECT_EQ(adaptor->share_layer(), shared);
}

TEST_F(CTEST_ALayerAdaptor, the_sub_layers_live_with_their_parent)
{
  std::weak_ptr<ALayerAdaptor> child;

  {
    const ALayerAdaptorPtr sub = ALayerAdaptor::create();
    child = sub;
    adaptor->output_layer(sub);
  }

  ASSERT_FALSE(child.expired());
  EXPECT_EQ(adaptor->get()->output_layer, child.lock()->get().get());

  adaptor->output_layer(ALayerAdaptorPtr{});

  EXPECT_TRUE(child.expired());
  EXPECT_EQ(adaptor->get()->output_layer, nullptr);
}

TEST_F(CTEST_ALayerAdaptor, borrows_batchnorm_tells_the_shared_normalization)
{
  EXPECT_FALSE(adaptor->borrows_batchnorm());

  adaptor->rolling_mean(std::vector<float>(4, 0.f));
  EXPECT_FALSE(adaptor->borrows_batchnorm());

  const ALayerAdaptorPtr owner = ALayerAdaptor::create();
  owner->rolling_mean(std::vector<float>(4, 0.f));

  adaptor->borrow_rolling_mean(owner->get()->rolling_mean);
  EXPECT_TRUE(adaptor->borrows_batchnorm());

  adaptor->borrow_rolling_mean(nullptr);
  EXPECT_FALSE(adaptor->borrows_batchnorm());
}

TEST_F(CTEST_ALayerAdaptor, the_owned_softmax_tree_lives_until_replaced)
{
  int released = 0;
  tree* const owned = new tree{};

  adaptor->own_softmax_tree(std::shared_ptr<tree>(owned, [&released](tree* t) {
    ++released;
    delete t;
  }));

  EXPECT_EQ(adaptor->get()->softmax_tree, owned);
  EXPECT_EQ(adaptor->softmax_tree(), owned);

  // setting the owned tree again keeps it
  adaptor->softmax_tree(owned);
  EXPECT_EQ(released, 0);

  tree other{};
  adaptor->softmax_tree(&other);

  EXPECT_EQ(released, 1);
  EXPECT_EQ(adaptor->get()->softmax_tree, &other);

  adaptor->softmax_tree(nullptr);
  EXPECT_EQ(adaptor->get()->softmax_tree, nullptr);
}

TEST_F(CTEST_ALayerAdaptor, the_owned_softmax_tree_is_released_with_the_adaptor)
{
  int released = 0;

  adaptor->own_softmax_tree(
      std::shared_ptr<tree>(new tree{}, [&released](tree* t) {
        ++released;
        delete t;
      }));

  adaptor.reset();

  EXPECT_EQ(released, 1);
}

TEST_F(CTEST_ALayerAdaptor, the_functions_of_the_layer)
{
  adaptor->forward(forward_convolutional_layer);
  adaptor->backward(backward_convolutional_layer);
  adaptor->update(update_convolutional_layer);

  EXPECT_EQ(adaptor->forward(), &forward_convolutional_layer);
  EXPECT_EQ(adaptor->backward(), &backward_convolutional_layer);
  EXPECT_EQ(adaptor->update(), &update_convolutional_layer);

  EXPECT_EQ(adaptor->get()->forward, &forward_convolutional_layer);
  EXPECT_EQ(adaptor->get()->backward, &backward_convolutional_layer);
  EXPECT_EQ(adaptor->get()->update, &update_convolutional_layer);
}
