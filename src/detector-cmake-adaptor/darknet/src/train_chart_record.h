#ifndef DARKNET_TRAIN_CHART_RECORD_STRUCT_H
#define DARKNET_TRAIN_CHART_RECORD_STRUCT_H

/// @brief The training record a chart draws, of the borrowed arrays: the
/// average loss of every recorded iteration and the mAP of every mAP
/// calculation, in the order of the training.
typedef struct train_chart_record
{
  const int *iterations;
  const float *avg_losses;
  int points;
  const int *map_iterations;
  const float *maps;
  int map_points;
} train_chart_record;

#endif  // DARKNET_TRAIN_CHART_RECORD_STRUCT_H
