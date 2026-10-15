#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_REWRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_REWRITER_CLASS_H

#include <string>

#include "src/NetworksLoaders/weights-model/NetworkDescription.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief Writes the edited network description into the file of a format
 * keeping it (the dxxwjz2, the dxxwjz3 ones and their projects): the rest of
 * the source file goes on (the annotations, the training record, the
 * weights), while the weights of the layers the edit changed are dropped.
 *
 * The weights of a layer go on while every layer up to it keeps its cfg
 * section (the type and the options) and its arrays the sizes of the edited
 * network (the [net] channels, width and height size them too), so the
 * weights of the leading layers of the network the edit kept go on. The
 * document of the dropped weights is the network to train of the kept ones:
 * of no seen images, no training record and no trailing bytes, the one of no
 * weights at all is the network to train from scratch.
 */
class NetworkRewriter
{
 public:
  /**
   * @brief Writes the network into the destination of the format of its
   * file name extension, through the temporary file.
   *
   * @param src The file the rest comes of, empty for none (the network to
   * train of no annotations).
   * @param dst The file to write, the src one included.
   * @param network The cfg text and the class names, the cfg the darknet
   * builds the network of.
   * @param message Out: the reason of the failure, or the weights the success
   * dropped, empty for none.
   *
   * @return False on any failure, which leaves the destination untouched.
   */
  static bool rewrite(const std::string& src, const std::string& dst,
                      const NetworkDescription& network, std::string& message);

 private:
  /**
   * @brief The reason the training can't run the network the darknet built,
   * empty for none: the [route] layer of the inputs of the different sizes,
   * which the parse warns of and the first resize of the training refuses,
   * the layer of no outputs.
   */
  static std::string flaw(const network& net);

  /// @brief Drops the weights of the layers the network changed, tells the
  /// index of the first one dropped, -1 for none.
  static int keep(WeightsDocument& document, const std::string& cfg,
                  const network& net);

  /// @brief The count of the leading layer sections of the same type and
  /// options of both cfg texts.
  static int same_sections(const std::string& cfg, const std::string& other);

  /// @brief The index of the first document layer of the arrays the network
  /// has none of the size of, INT_MAX for none.
  static int fitting_layers(const WeightsDocument& document,
                            const network& net);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_REWRITER_CLASS_H
