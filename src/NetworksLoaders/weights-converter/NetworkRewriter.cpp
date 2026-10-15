#include "src/NetworksLoaders/weights-converter/NetworkRewriter.h"

#include <algorithm>
#include <climits>
#include <exception>
#include <sstream>
#include <string>

#include "src/NetworksLoaders/OrigDefaultLoader/CfgSection.h"
#include "src/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/NetworksLoaders/weights-converter/WeightsConversion.h"
#include "src/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/NetworksLoaders/weights-model/WeightsLayout.h"
#include "src/log/log.h"

namespace darknet_adaptor::loaders
{

bool NetworkRewriter::rewrite(const std::string& src, const std::string& dst,
                              const NetworkDescription& network,
                              std::string& message)
{
  WeightsConversion conversion;

  conversion.to = WeightsFileFormats::of_file(dst);
  conversion.dst = dst;

  if (!conversion.to->keeps_network()) {
    message =
        "The " + conversion.to->name() + " file " + dst +
        " keeps no network, the dxxwjz3p project or the dxxwjz3 file does";
    return false;
  }

  WeightsDocument document;
  const IWeightsFileFormatPtr from =
      src.empty() ? nullptr : WeightsFileFormats::of_file(src);

  if (from != nullptr &&
      (!from->keeps_network() ||
       !from->create_reader()->read(src, nullptr, document))) {
    message = "Fail to read the network file " + src;
    return false;
  }

  adaptors::ANetworkAdaptorPtr net;

  try {
    // the single image network of the least memory: the arrays of the
    // weights don't depend on the batch
    net = OrigDefaultLoader{}.parse_network_text(network.cfg(), dst, 1, 1);
  }
  catch (const std::exception& failure) {
    message = "The darknet builds no network of the cfg: " +
              std::string{failure.what()};
    return false;
  }

  message = flaw(*net->get());

  if (!message.empty()) {
    return false;
  }

  const int dropped = keep(document, network.cfg(), *net->get());

  document.network = network;

  if (!WeightsConverter::write_destination(conversion, document)) {
    message = "Fail to write the network file " + dst;
    return false;
  }

  message = dropped < 0
                ? std::string{}
                : "The weights of the layer " + std::to_string(dropped) +
                      " and of the ones after it were dropped, the "
                      "network changed them";

  if (!message.empty()) {
    LOGW(message);
  }

  return true;
}

std::string NetworkRewriter::flaw(const network& net)
{
  for (int index = 0; index < net.n; ++index) {
    const layer& l = net.layers[index];
    const std::string name = "The layer " + std::to_string(index);

    if (l.outputs <= 0) {
      return name + " has no outputs";
    }

    for (int input = 1; l.type == ROUTE && input < l.n; ++input) {
      const layer& first = net.layers[l.input_layers[0]];
      const layer& next = net.layers[l.input_layers[input]];

      if (next.out_w != first.out_w || next.out_h != first.out_h) {
        return name + " routes the layers of the different sizes: " +
               std::to_string(first.out_w) + " x " +
               std::to_string(first.out_h) + " and " +
               std::to_string(next.out_w) + " x " + std::to_string(next.out_h);
      }
    }
  }

  return {};
}

int NetworkRewriter::keep(WeightsDocument& document, const std::string& cfg,
                          const network& net)
{
  const int kept = std::min(same_sections(document.network.cfg(), cfg),
                            fitting_layers(document, net));
  const auto first = std::find_if(
      document.layers.begin(), document.layers.end(),
      [kept](const WeightsLayer& layer) { return layer.index >= kept; });

  if (first == document.layers.end()) {
    return -1;
  }

  const int dropped = first->index;

  // the network to train of the weights kept
  document.layers.erase(first, document.layers.end());
  document.trailing.clear();
  document.training.clear();
  document.header.seen = 0U;

  return dropped;
}

int NetworkRewriter::same_sections(const std::string& cfg,
                                   const std::string& other)
{
  std::istringstream text{cfg};
  std::istringstream otherText{other};
  CfgSection::CfgSections sections;
  CfgSection::CfgSections others;

  try {
    sections = CfgSection::read_cfg(text, "the network");
    others = CfgSection::read_cfg(otherText, "the network");
  }
  catch (const std::exception& failure) {
    LOGD("No layer is the same: " << failure.what());
    return 0;
  }

  const auto same = [](const CfgSection& section, const CfgSection& another) {
    const auto options = section.options();
    const auto otherOptions = another.options();

    return section.type() == another.type() &&
           std::equal(options.begin(), options.end(), otherOptions.begin(),
                      otherOptions.end(), [](const auto& one, const auto& two) {
                        return one.first == two.first &&
                               (one.second == nullptr || two.second == nullptr
                                    ? one.second == two.second
                                    : std::string{one.second} == two.second);
                      });
  };

  std::size_t index = 1U;

  // the [net] section of the first one is no layer
  while (index < sections.size() && index < others.size() &&
         same(sections[index], others[index])) {
    ++index;
  }

  return static_cast<int>(index) - 1;
}

int NetworkRewriter::fitting_layers(const WeightsDocument& document,
                                    const network& net)
{
  const WeightsLayoutEntries layout = WeightsLayout::of(net);

  for (const WeightsLayer& layer : document.layers) {
    if (layer.index >= net.n || net.layers[layer.index].type != layer.type) {
      return layer.index;
    }

    for (const WeightsArray& array : layer.arrays) {
      const auto entry = std::find_if(
          layout.begin(), layout.end(), [&](const WeightsLayoutEntry& one) {
            return one.layer == layer.index && one.sub == array.sub &&
                   one.kind == array.kind;
          });

      if (entry == layout.end() || entry->count != array.values.size()) {
        return layer.index;
      }
    }
  }

  return INT_MAX;
}

}  // namespace darknet_adaptor::loaders
