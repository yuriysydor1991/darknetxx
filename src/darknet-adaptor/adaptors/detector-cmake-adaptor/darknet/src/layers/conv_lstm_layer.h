#ifndef CONV_LSTM_LAYER_H
#define CONV_LSTM_LAYER_H

#include "activations.h"
#include "layer.h"
#include "network.h"
#define USET

#ifdef __cplusplus
extern "C" {
#endif
void free_state_conv_lstm(layer l);
void randomize_state_conv_lstm(layer l);
void remember_state_conv_lstm(layer l);
void restore_state_conv_lstm(layer l);

void forward_conv_lstm_layer(layer l, network_state state);
void backward_conv_lstm_layer(layer l, network_state state);
void update_conv_lstm_layer(layer l, int batch, float learning_rate,
                            float momentum, float decay);

void forward_history_layer(layer l, network_state state);
void backward_history_layer(layer l, network_state state);

#ifdef GPU
void forward_conv_lstm_layer_gpu(layer l, network_state state);
void backward_conv_lstm_layer_gpu(layer l, network_state state);
void update_conv_lstm_layer_gpu(layer l, int batch, float learning_rate,
                                float momentum, float decay, float loss_scale);

void forward_history_layer_gpu(const layer l, network_state state);
void backward_history_layer_gpu(const layer l, network_state state);
#endif

#ifdef __cplusplus
}
#endif

#endif  // CONV_LSTM_LAYER_H
