#include "layer.h"

#include <stdio.h>
#include <stdlib.h>

#include "dark_cuda.h"
#include "src/clog/clog.h"

char *layer_type_2_string(const LAYER_TYPE type)
{
  if (type == CONVOLUTIONAL) {
    return "CONVOLUTIONAL";
  }
  if (type == DECONVOLUTIONAL) {
    return "DECONVOLUTIONAL";
  }
  if (type == CONNECTED) {
    return "CONNECTED";
  }
  if (type == MAXPOOL) {
    return "MAXPOOL";
  }
  if (type == LOCAL_AVGPOOL) {
    return "LOCAL_AVGPOOL";
  }
  if (type == SOFTMAX) {
    return "SOFTMAX";
  }
  if (type == DETECTION) {
    return "DETECTION";
  }
  if (type == DROPOUT) {
    return "DROPOUT";
  }
  if (type == CROP) {
    return "CROP";
  }
  if (type == ROUTE) {
    return "ROUTE";
  }
  if (type == COST) {
    return "COST";
  }
  if (type == NORMALIZATION) {
    return "NORMALIZATION";
  }
  if (type == AVGPOOL) {
    return "AVGPOOL";
  }
  if (type == LOCAL) {
    return "LOCAL";
  }
  if (type == SHORTCUT) {
    return "SHORTCUT";
  }
  if (type == SCALE_CHANNELS) {
    return "SCALE_CHANNELS";
  }
  if (type == SAM) {
    return "SAM";
  }
  if (type == ACTIVE) {
    return "ACTIVE";
  }
  if (type == RNN) {
    return "RNN";
  }
  if (type == GRU) {
    return "GRU";
  }
  if (type == LSTM) {
    return "LSTM";
  }
  if (type == CONV_LSTM) {
    return "CONV_LSTM";
  }
  if (type == HISTORY) {
    return "HISTORY";
  }
  if (type == CRNN) {
    return "CRNN";
  }
  if (type == BATCHNORM) {
    return "BATCHNORM";
  }
  if (type == NETWORK) {
    return "NETWORK";
  }
  if (type == XNOR) {
    return "XNOR";
  }
  if (type == REGION) {
    return "REGION";
  }
  if (type == YOLO) {
    return "YOLO";
  }
  if (type == GAUSSIAN_YOLO) {
    return "GAUSSIAN_YOLO";
  }
  if (type == ISEG) {
    return "ISEG";
  }
  if (type == REORG) {
    return "REORG";
  }
  if (type == REORG_OLD) {
    return "REORG_OLD";
  }
  if (type == UPSAMPLE) {
    return "UPSAMPLE";
  }
  if (type == LOGXENT) {
    return "LOGXENT";
  }
  if (type == L2NORM) {
    return "L2NORM";
  }
  if (type == EMPTY) {
    return "EMPTY";
  }
  if (type == BLANK) {
    return "BLANK";
  }
  if (type == CONTRASTIVE) {
    return "CONTRASTIVE";
  }
  if (type == IMPLICIT) {
    return "IMPLICIT";
  }

  CLOGE("Unknown layer type given: %d", type);

  return "Unknown";
}
