#ifndef DARKNET_CONTRASTIVE_PARAMS_H
#define DARKNET_CONTRASTIVE_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

// blas.h
typedef struct contrastive_params
{
  float sim;
  float exp_sim;
  float P;
  int i, j;
  int time_step_i, time_step_j;
} contrastive_params;

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_CONTRASTIVE_PARAMS_H
