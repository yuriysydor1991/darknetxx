#ifndef DARKNET_TRAIN_CHART_STATE_STRUCT_H
#define DARKNET_TRAIN_CHART_STATE_STRUCT_H

/// @brief The state of a training chart between the points drawn: the last
/// iteration the chart is saved at and the last points of the curves, which
/// the next points continue. The zeroed state starts a new chart.
typedef struct train_chart_state
{
  int saved_batch;
  float old_contr_acc;
  float old_precision;
  float max_precision;
  int iteration_old;
  int text_iteration_old;
} train_chart_state;

#endif  // DARKNET_TRAIN_CHART_STATE_STRUCT_H
