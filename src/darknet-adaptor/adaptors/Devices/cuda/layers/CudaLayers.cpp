#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayers.h"

#include <string>

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaBatchnormLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaConnectedLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaContrastiveLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaConvLstmLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaConvolutionalLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaCropLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaDropoutLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaEmptyLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaGruLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaImplicitLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLocalLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLstmLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaMaxpoolLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaOutputLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaRnnLayer.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaShortcutLayer.h"
#include "src/darknet-adaptor/adaptors/NetworkError.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

const ICudaLayer& CudaLayers::of(LAYER_TYPE type)
{
  static const CudaConvolutionalLayer convolutional;
  static const CudaConnectedLayer connected;
  static const CudaMaxpoolLayer maxpool;
  static const CudaBatchnormLayer batchnorm;
  static const CudaShortcutLayer shortcut;
  static const CudaLocalLayer local;
  static const CudaCropLayer crop;
  static const CudaDropoutLayer dropout;
  static const CudaEmptyLayer empty;
  static const CudaImplicitLayer implicit;
  static const CudaContrastiveLayer contrastive;
  static const CudaGruLayer gru;
  static const CudaLstmLayer lstm;
  static const CudaConvLstmLayer conv_lstm;
  static const CudaRnnLayer rnn{forward_rnn_layer_gpu, backward_rnn_layer_gpu,
                                update_rnn_layer_gpu};
  static const CudaRnnLayer crnn{
      forward_crnn_layer_gpu, backward_crnn_layer_gpu, update_crnn_layer_gpu};
  static const CudaOutputLayer activation{forward_activation_layer_gpu,
                                          backward_activation_layer_gpu};
  static const CudaOutputLayer avgpool{forward_avgpool_layer_gpu,
                                       backward_avgpool_layer_gpu};
  static const CudaOutputLayer cost{forward_cost_layer_gpu,
                                    backward_cost_layer_gpu};
  static const CudaOutputLayer detection{forward_detection_layer_gpu,
                                         backward_detection_layer_gpu};
  static const CudaOutputLayer region{forward_region_layer_gpu,
                                      backward_region_layer_gpu};
  static const CudaOutputLayer reorg{forward_reorg_layer_gpu,
                                     backward_reorg_layer_gpu};
  static const CudaOutputLayer reorg_old{forward_reorg_old_layer_gpu,
                                         backward_reorg_old_layer_gpu};
  static const CudaOutputLayer route{forward_route_layer_gpu,
                                     backward_route_layer_gpu};
  static const CudaOutputLayer sam{forward_sam_layer_gpu,
                                   backward_sam_layer_gpu};
  static const CudaOutputLayer scale_channels{
      forward_scale_channels_layer_gpu, backward_scale_channels_layer_gpu};
  static const CudaOutputLayer upsample{forward_upsample_layer_gpu,
                                        backward_upsample_layer_gpu};
  static const CudaOutputLayer gaussian_yolo{forward_gaussian_yolo_layer_gpu,
                                             backward_gaussian_yolo_layer_gpu};
  static const CudaOutputLayer yolo{forward_yolo_layer_gpu,
                                    backward_yolo_layer_gpu,
                                    {{&layer::output_avg_gpu, &layer::output}}};
  static const CudaOutputLayer softmax{forward_softmax_layer_gpu,
                                       backward_softmax_layer_gpu,
                                       {{&layer::loss_gpu, &layer::loss}}};
  static const CudaOutputLayer normalization{
      forward_normalization_layer_gpu,
      backward_normalization_layer_gpu,
      {{&layer::squared_gpu, &layer::squared},
       {&layer::norms_gpu, &layer::norms}}};
  static const CudaOutputLayer history{forward_history_layer_gpu,
                                       backward_history_layer_gpu,
                                       {{&layer::prev_state_gpu, nullptr}}};

  switch (type) {
    case CONVOLUTIONAL:
      return convolutional;
    case CONNECTED:
      return connected;
    case MAXPOOL:
    case LOCAL_AVGPOOL:
      return maxpool;
    case BATCHNORM:
      return batchnorm;
    case SHORTCUT:
      return shortcut;
    case LOCAL:
      return local;
    case CROP:
      return crop;
    case DROPOUT:
      return dropout;
    case EMPTY:
      return empty;
    case IMPLICIT:
      return implicit;
    case CONTRASTIVE:
      return contrastive;
    case GRU:
      return gru;
    case LSTM:
      return lstm;
    case CONV_LSTM:
      return conv_lstm;
    case RNN:
      return rnn;
    case CRNN:
      return crnn;
    case ACTIVE:
      return activation;
    case AVGPOOL:
      return avgpool;
    case COST:
      return cost;
    case DETECTION:
      return detection;
    case REGION:
      return region;
    case REORG:
      return reorg;
    case REORG_OLD:
      return reorg_old;
    case ROUTE:
      return route;
    case SAM:
      return sam;
    case SCALE_CHANNELS:
      return scale_channels;
    case UPSAMPLE:
      return upsample;
    case GAUSSIAN_YOLO:
      return gaussian_yolo;
    case YOLO:
      return yolo;
    case SOFTMAX:
      return softmax;
    case NORMALIZATION:
      return normalization;
    case HISTORY:
      return history;
    default:
      break;
  }

  throw NetworkError(std::string{"No GPU computation of the layer "} +
                     layer_type_2_string(type));
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
