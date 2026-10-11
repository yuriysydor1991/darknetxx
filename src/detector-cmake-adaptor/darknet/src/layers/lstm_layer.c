#include "layers/lstm_layer.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blas.h"
#include "dark_cuda.h"
#include "gemm.h"
#include "layers/connected_layer.h"
#include "src/clog/clog.h"
#include "utils.h"

static void increment_layer(layer *l, int steps)
{
  int num = l->outputs * l->batch * steps;
  l->output += num;
  l->delta += num;
  l->x += num;
  l->x_norm += num;

#ifdef GPU
  l->output_gpu += num;
  l->delta_gpu += num;
  l->x_gpu += num;
  l->x_norm_gpu += num;
#endif
}

void update_lstm_layer(layer l, int batch, float learning_rate, float momentum,
                       float decay)
{
  update_connected_layer(*(l.wf), batch, learning_rate, momentum, decay);
  update_connected_layer(*(l.wi), batch, learning_rate, momentum, decay);
  update_connected_layer(*(l.wg), batch, learning_rate, momentum, decay);
  update_connected_layer(*(l.wo), batch, learning_rate, momentum, decay);
  update_connected_layer(*(l.uf), batch, learning_rate, momentum, decay);
  update_connected_layer(*(l.ui), batch, learning_rate, momentum, decay);
  update_connected_layer(*(l.ug), batch, learning_rate, momentum, decay);
  update_connected_layer(*(l.uo), batch, learning_rate, momentum, decay);
}

void forward_lstm_layer(layer l, network_state state)
{
  network_state s = {0};
  s.train = state.train;
  s.workspace = state.workspace;
  int i;
  layer wf = *(l.wf);
  layer wi = *(l.wi);
  layer wg = *(l.wg);
  layer wo = *(l.wo);

  layer uf = *(l.uf);
  layer ui = *(l.ui);
  layer ug = *(l.ug);
  layer uo = *(l.uo);

  fill_cpu(l.outputs * l.batch * l.steps, 0, wf.delta, 1);
  fill_cpu(l.outputs * l.batch * l.steps, 0, wi.delta, 1);
  fill_cpu(l.outputs * l.batch * l.steps, 0, wg.delta, 1);
  fill_cpu(l.outputs * l.batch * l.steps, 0, wo.delta, 1);

  fill_cpu(l.outputs * l.batch * l.steps, 0, uf.delta, 1);
  fill_cpu(l.outputs * l.batch * l.steps, 0, ui.delta, 1);
  fill_cpu(l.outputs * l.batch * l.steps, 0, ug.delta, 1);
  fill_cpu(l.outputs * l.batch * l.steps, 0, uo.delta, 1);
  if (state.train) {
    fill_cpu(l.outputs * l.batch * l.steps, 0, l.delta, 1);
  }

  for (i = 0; i < l.steps; ++i) {
    s.input = l.h_cpu;
    forward_connected_layer(wf, s);
    forward_connected_layer(wi, s);
    forward_connected_layer(wg, s);
    forward_connected_layer(wo, s);

    s.input = state.input;
    forward_connected_layer(uf, s);
    forward_connected_layer(ui, s);
    forward_connected_layer(ug, s);
    forward_connected_layer(uo, s);

    copy_cpu(l.outputs * l.batch, wf.output, 1, l.f_cpu, 1);
    axpy_cpu(l.outputs * l.batch, 1, uf.output, 1, l.f_cpu, 1);

    copy_cpu(l.outputs * l.batch, wi.output, 1, l.i_cpu, 1);
    axpy_cpu(l.outputs * l.batch, 1, ui.output, 1, l.i_cpu, 1);

    copy_cpu(l.outputs * l.batch, wg.output, 1, l.g_cpu, 1);
    axpy_cpu(l.outputs * l.batch, 1, ug.output, 1, l.g_cpu, 1);

    copy_cpu(l.outputs * l.batch, wo.output, 1, l.o_cpu, 1);
    axpy_cpu(l.outputs * l.batch, 1, uo.output, 1, l.o_cpu, 1);

    activate_array(l.f_cpu, l.outputs * l.batch, LOGISTIC);
    activate_array(l.i_cpu, l.outputs * l.batch, LOGISTIC);
    activate_array(l.g_cpu, l.outputs * l.batch, TANH);
    activate_array(l.o_cpu, l.outputs * l.batch, LOGISTIC);

    copy_cpu(l.outputs * l.batch, l.i_cpu, 1, l.temp_cpu, 1);
    mul_cpu(l.outputs * l.batch, l.g_cpu, 1, l.temp_cpu, 1);
    mul_cpu(l.outputs * l.batch, l.f_cpu, 1, l.c_cpu, 1);
    axpy_cpu(l.outputs * l.batch, 1, l.temp_cpu, 1, l.c_cpu, 1);

    copy_cpu(l.outputs * l.batch, l.c_cpu, 1, l.h_cpu, 1);
    activate_array(l.h_cpu, l.outputs * l.batch, TANH);
    mul_cpu(l.outputs * l.batch, l.o_cpu, 1, l.h_cpu, 1);

    copy_cpu(l.outputs * l.batch, l.c_cpu, 1, l.cell_cpu, 1);
    copy_cpu(l.outputs * l.batch, l.h_cpu, 1, l.output, 1);

    state.input += l.inputs * l.batch;
    l.output += l.outputs * l.batch;
    l.cell_cpu += l.outputs * l.batch;

    increment_layer(&wf, 1);
    increment_layer(&wi, 1);
    increment_layer(&wg, 1);
    increment_layer(&wo, 1);

    increment_layer(&uf, 1);
    increment_layer(&ui, 1);
    increment_layer(&ug, 1);
    increment_layer(&uo, 1);
  }
}

void backward_lstm_layer(layer l, network_state state)
{
  network_state s = {0};
  s.train = state.train;
  s.workspace = state.workspace;
  int i;
  layer wf = *(l.wf);
  layer wi = *(l.wi);
  layer wg = *(l.wg);
  layer wo = *(l.wo);

  layer uf = *(l.uf);
  layer ui = *(l.ui);
  layer ug = *(l.ug);
  layer uo = *(l.uo);

  increment_layer(&wf, l.steps - 1);
  increment_layer(&wi, l.steps - 1);
  increment_layer(&wg, l.steps - 1);
  increment_layer(&wo, l.steps - 1);

  increment_layer(&uf, l.steps - 1);
  increment_layer(&ui, l.steps - 1);
  increment_layer(&ug, l.steps - 1);
  increment_layer(&uo, l.steps - 1);

  state.input += l.inputs * l.batch * (l.steps - 1);
  if (state.delta) state.delta += l.inputs * l.batch * (l.steps - 1);

  l.output += l.outputs * l.batch * (l.steps - 1);
  l.cell_cpu += l.outputs * l.batch * (l.steps - 1);
  l.delta += l.outputs * l.batch * (l.steps - 1);

  for (i = l.steps - 1; i >= 0; --i) {
    if (i != 0)
      copy_cpu(l.outputs * l.batch, l.cell_cpu - l.outputs * l.batch, 1,
               l.prev_cell_cpu, 1);
    copy_cpu(l.outputs * l.batch, l.cell_cpu, 1, l.c_cpu, 1);
    if (i != 0)
      copy_cpu(l.outputs * l.batch, l.output - l.outputs * l.batch, 1,
               l.prev_state_cpu, 1);
    copy_cpu(l.outputs * l.batch, l.output, 1, l.h_cpu, 1);

    l.dh_cpu = (i == 0) ? 0 : l.delta - l.outputs * l.batch;

    copy_cpu(l.outputs * l.batch, wf.output, 1, l.f_cpu, 1);
    axpy_cpu(l.outputs * l.batch, 1, uf.output, 1, l.f_cpu, 1);

    copy_cpu(l.outputs * l.batch, wi.output, 1, l.i_cpu, 1);
    axpy_cpu(l.outputs * l.batch, 1, ui.output, 1, l.i_cpu, 1);

    copy_cpu(l.outputs * l.batch, wg.output, 1, l.g_cpu, 1);
    axpy_cpu(l.outputs * l.batch, 1, ug.output, 1, l.g_cpu, 1);

    copy_cpu(l.outputs * l.batch, wo.output, 1, l.o_cpu, 1);
    axpy_cpu(l.outputs * l.batch, 1, uo.output, 1, l.o_cpu, 1);

    activate_array(l.f_cpu, l.outputs * l.batch, LOGISTIC);
    activate_array(l.i_cpu, l.outputs * l.batch, LOGISTIC);
    activate_array(l.g_cpu, l.outputs * l.batch, TANH);
    activate_array(l.o_cpu, l.outputs * l.batch, LOGISTIC);

    copy_cpu(l.outputs * l.batch, l.delta, 1, l.temp3_cpu, 1);

    copy_cpu(l.outputs * l.batch, l.c_cpu, 1, l.temp_cpu, 1);
    activate_array(l.temp_cpu, l.outputs * l.batch, TANH);

    copy_cpu(l.outputs * l.batch, l.temp3_cpu, 1, l.temp2_cpu, 1);
    mul_cpu(l.outputs * l.batch, l.o_cpu, 1, l.temp2_cpu, 1);

    gradient_array(l.temp_cpu, l.outputs * l.batch, TANH, l.temp2_cpu);
    axpy_cpu(l.outputs * l.batch, 1, l.dc_cpu, 1, l.temp2_cpu, 1);

    copy_cpu(l.outputs * l.batch, l.c_cpu, 1, l.temp_cpu, 1);
    activate_array(l.temp_cpu, l.outputs * l.batch, TANH);
    mul_cpu(l.outputs * l.batch, l.temp3_cpu, 1, l.temp_cpu, 1);
    gradient_array(l.o_cpu, l.outputs * l.batch, LOGISTIC, l.temp_cpu);
    copy_cpu(l.outputs * l.batch, l.temp_cpu, 1, wo.delta, 1);
    s.input = l.prev_state_cpu;
    s.delta = l.dh_cpu;
    backward_connected_layer(wo, s);

    copy_cpu(l.outputs * l.batch, l.temp_cpu, 1, uo.delta, 1);
    s.input = state.input;
    s.delta = state.delta;
    backward_connected_layer(uo, s);

    copy_cpu(l.outputs * l.batch, l.temp2_cpu, 1, l.temp_cpu, 1);
    mul_cpu(l.outputs * l.batch, l.i_cpu, 1, l.temp_cpu, 1);
    gradient_array(l.g_cpu, l.outputs * l.batch, TANH, l.temp_cpu);
    copy_cpu(l.outputs * l.batch, l.temp_cpu, 1, wg.delta, 1);
    s.input = l.prev_state_cpu;
    s.delta = l.dh_cpu;
    backward_connected_layer(wg, s);

    copy_cpu(l.outputs * l.batch, l.temp_cpu, 1, ug.delta, 1);
    s.input = state.input;
    s.delta = state.delta;
    backward_connected_layer(ug, s);

    copy_cpu(l.outputs * l.batch, l.temp2_cpu, 1, l.temp_cpu, 1);
    mul_cpu(l.outputs * l.batch, l.g_cpu, 1, l.temp_cpu, 1);
    gradient_array(l.i_cpu, l.outputs * l.batch, LOGISTIC, l.temp_cpu);
    copy_cpu(l.outputs * l.batch, l.temp_cpu, 1, wi.delta, 1);
    s.input = l.prev_state_cpu;
    s.delta = l.dh_cpu;
    backward_connected_layer(wi, s);

    copy_cpu(l.outputs * l.batch, l.temp_cpu, 1, ui.delta, 1);
    s.input = state.input;
    s.delta = state.delta;
    backward_connected_layer(ui, s);

    copy_cpu(l.outputs * l.batch, l.temp2_cpu, 1, l.temp_cpu, 1);
    mul_cpu(l.outputs * l.batch, l.prev_cell_cpu, 1, l.temp_cpu, 1);
    gradient_array(l.f_cpu, l.outputs * l.batch, LOGISTIC, l.temp_cpu);
    copy_cpu(l.outputs * l.batch, l.temp_cpu, 1, wf.delta, 1);
    s.input = l.prev_state_cpu;
    s.delta = l.dh_cpu;
    backward_connected_layer(wf, s);

    copy_cpu(l.outputs * l.batch, l.temp_cpu, 1, uf.delta, 1);
    s.input = state.input;
    s.delta = state.delta;
    backward_connected_layer(uf, s);

    copy_cpu(l.outputs * l.batch, l.temp2_cpu, 1, l.temp_cpu, 1);
    mul_cpu(l.outputs * l.batch, l.f_cpu, 1, l.temp_cpu, 1);
    copy_cpu(l.outputs * l.batch, l.temp_cpu, 1, l.dc_cpu, 1);

    state.input -= l.inputs * l.batch;
    if (state.delta) state.delta -= l.inputs * l.batch;
    l.output -= l.outputs * l.batch;
    l.cell_cpu -= l.outputs * l.batch;
    l.delta -= l.outputs * l.batch;

    increment_layer(&wf, -1);
    increment_layer(&wi, -1);
    increment_layer(&wg, -1);
    increment_layer(&wo, -1);

    increment_layer(&uf, -1);
    increment_layer(&ui, -1);
    increment_layer(&ug, -1);
    increment_layer(&uo, -1);
  }
}

#ifdef GPU
void update_lstm_layer_gpu(layer l, int batch, float learning_rate,
                           float momentum, float decay, float loss_scale)
{
  update_connected_layer_gpu(*(l.wf), batch, learning_rate, momentum, decay,
                             loss_scale);
  update_connected_layer_gpu(*(l.wi), batch, learning_rate, momentum, decay,
                             loss_scale);
  update_connected_layer_gpu(*(l.wg), batch, learning_rate, momentum, decay,
                             loss_scale);
  update_connected_layer_gpu(*(l.wo), batch, learning_rate, momentum, decay,
                             loss_scale);
  update_connected_layer_gpu(*(l.uf), batch, learning_rate, momentum, decay,
                             loss_scale);
  update_connected_layer_gpu(*(l.ui), batch, learning_rate, momentum, decay,
                             loss_scale);
  update_connected_layer_gpu(*(l.ug), batch, learning_rate, momentum, decay,
                             loss_scale);
  update_connected_layer_gpu(*(l.uo), batch, learning_rate, momentum, decay,
                             loss_scale);
}

void forward_lstm_layer_gpu(layer l, network_state state)
{
  network_state s = {0};
  s.train = state.train;
  s.workspace = state.workspace;
  int i;
  layer wf = *(l.wf);
  layer wi = *(l.wi);
  layer wg = *(l.wg);
  layer wo = *(l.wo);

  layer uf = *(l.uf);
  layer ui = *(l.ui);
  layer ug = *(l.ug);
  layer uo = *(l.uo);

  fill_ongpu(l.outputs * l.batch * l.steps, 0, wf.delta_gpu, 1);
  fill_ongpu(l.outputs * l.batch * l.steps, 0, wi.delta_gpu, 1);
  fill_ongpu(l.outputs * l.batch * l.steps, 0, wg.delta_gpu, 1);
  fill_ongpu(l.outputs * l.batch * l.steps, 0, wo.delta_gpu, 1);

  fill_ongpu(l.outputs * l.batch * l.steps, 0, uf.delta_gpu, 1);
  fill_ongpu(l.outputs * l.batch * l.steps, 0, ui.delta_gpu, 1);
  fill_ongpu(l.outputs * l.batch * l.steps, 0, ug.delta_gpu, 1);
  fill_ongpu(l.outputs * l.batch * l.steps, 0, uo.delta_gpu, 1);
  if (state.train) {
    fill_ongpu(l.outputs * l.batch * l.steps, 0, l.delta_gpu, 1);
  }

  for (i = 0; i < l.steps; ++i) {
    s.input = l.h_gpu;
    forward_connected_layer_gpu(wf, s);
    forward_connected_layer_gpu(wi, s);
    forward_connected_layer_gpu(wg, s);
    forward_connected_layer_gpu(wo, s);

    s.input = state.input;
    forward_connected_layer_gpu(uf, s);
    forward_connected_layer_gpu(ui, s);
    forward_connected_layer_gpu(ug, s);
    forward_connected_layer_gpu(uo, s);

    copy_ongpu(l.outputs * l.batch, wf.output_gpu, 1, l.f_gpu, 1);
    axpy_ongpu(l.outputs * l.batch, 1, uf.output_gpu, 1, l.f_gpu, 1);

    copy_ongpu(l.outputs * l.batch, wi.output_gpu, 1, l.i_gpu, 1);
    axpy_ongpu(l.outputs * l.batch, 1, ui.output_gpu, 1, l.i_gpu, 1);

    copy_ongpu(l.outputs * l.batch, wg.output_gpu, 1, l.g_gpu, 1);
    axpy_ongpu(l.outputs * l.batch, 1, ug.output_gpu, 1, l.g_gpu, 1);

    copy_ongpu(l.outputs * l.batch, wo.output_gpu, 1, l.o_gpu, 1);
    axpy_ongpu(l.outputs * l.batch, 1, uo.output_gpu, 1, l.o_gpu, 1);

    activate_array_ongpu(l.f_gpu, l.outputs * l.batch, LOGISTIC);
    activate_array_ongpu(l.i_gpu, l.outputs * l.batch, LOGISTIC);
    activate_array_ongpu(l.g_gpu, l.outputs * l.batch, TANH);
    activate_array_ongpu(l.o_gpu, l.outputs * l.batch, LOGISTIC);

    copy_ongpu(l.outputs * l.batch, l.i_gpu, 1, l.temp_gpu, 1);
    mul_ongpu(l.outputs * l.batch, l.g_gpu, 1, l.temp_gpu, 1);
    mul_ongpu(l.outputs * l.batch, l.f_gpu, 1, l.c_gpu, 1);
    axpy_ongpu(l.outputs * l.batch, 1, l.temp_gpu, 1, l.c_gpu, 1);

    copy_ongpu(l.outputs * l.batch, l.c_gpu, 1, l.h_gpu, 1);
    activate_array_ongpu(l.h_gpu, l.outputs * l.batch, TANH);
    mul_ongpu(l.outputs * l.batch, l.o_gpu, 1, l.h_gpu, 1);

    copy_ongpu(l.outputs * l.batch, l.c_gpu, 1, l.cell_gpu, 1);
    copy_ongpu(l.outputs * l.batch, l.h_gpu, 1, l.output_gpu, 1);

    state.input += l.inputs * l.batch;
    l.output_gpu += l.outputs * l.batch;
    l.cell_gpu += l.outputs * l.batch;

    increment_layer(&wf, 1);
    increment_layer(&wi, 1);
    increment_layer(&wg, 1);
    increment_layer(&wo, 1);

    increment_layer(&uf, 1);
    increment_layer(&ui, 1);
    increment_layer(&ug, 1);
    increment_layer(&uo, 1);
  }
}

void backward_lstm_layer_gpu(layer l, network_state state)
{
  network_state s = {0};
  s.train = state.train;
  s.workspace = state.workspace;
  int i;
  layer wf = *(l.wf);
  layer wi = *(l.wi);
  layer wg = *(l.wg);
  layer wo = *(l.wo);

  layer uf = *(l.uf);
  layer ui = *(l.ui);
  layer ug = *(l.ug);
  layer uo = *(l.uo);

  increment_layer(&wf, l.steps - 1);
  increment_layer(&wi, l.steps - 1);
  increment_layer(&wg, l.steps - 1);
  increment_layer(&wo, l.steps - 1);

  increment_layer(&uf, l.steps - 1);
  increment_layer(&ui, l.steps - 1);
  increment_layer(&ug, l.steps - 1);
  increment_layer(&uo, l.steps - 1);

  state.input += l.inputs * l.batch * (l.steps - 1);
  if (state.delta) state.delta += l.inputs * l.batch * (l.steps - 1);

  l.output_gpu += l.outputs * l.batch * (l.steps - 1);
  l.cell_gpu += l.outputs * l.batch * (l.steps - 1);
  l.delta_gpu += l.outputs * l.batch * (l.steps - 1);

  for (i = l.steps - 1; i >= 0; --i) {
    if (i != 0)
      copy_ongpu(l.outputs * l.batch, l.cell_gpu - l.outputs * l.batch, 1,
                 l.prev_cell_gpu, 1);
    copy_ongpu(l.outputs * l.batch, l.cell_gpu, 1, l.c_gpu, 1);
    if (i != 0)
      copy_ongpu(l.outputs * l.batch, l.output_gpu - l.outputs * l.batch, 1,
                 l.prev_state_gpu, 1);
    copy_ongpu(l.outputs * l.batch, l.output_gpu, 1, l.h_gpu, 1);

    l.dh_gpu = (i == 0) ? 0 : l.delta_gpu - l.outputs * l.batch;

    copy_ongpu(l.outputs * l.batch, wf.output_gpu, 1, l.f_gpu, 1);
    axpy_ongpu(l.outputs * l.batch, 1, uf.output_gpu, 1, l.f_gpu, 1);

    copy_ongpu(l.outputs * l.batch, wi.output_gpu, 1, l.i_gpu, 1);
    axpy_ongpu(l.outputs * l.batch, 1, ui.output_gpu, 1, l.i_gpu, 1);

    copy_ongpu(l.outputs * l.batch, wg.output_gpu, 1, l.g_gpu, 1);
    axpy_ongpu(l.outputs * l.batch, 1, ug.output_gpu, 1, l.g_gpu, 1);

    copy_ongpu(l.outputs * l.batch, wo.output_gpu, 1, l.o_gpu, 1);
    axpy_ongpu(l.outputs * l.batch, 1, uo.output_gpu, 1, l.o_gpu, 1);

    activate_array_ongpu(l.f_gpu, l.outputs * l.batch, LOGISTIC);
    activate_array_ongpu(l.i_gpu, l.outputs * l.batch, LOGISTIC);
    activate_array_ongpu(l.g_gpu, l.outputs * l.batch, TANH);
    activate_array_ongpu(l.o_gpu, l.outputs * l.batch, LOGISTIC);

    copy_ongpu(l.outputs * l.batch, l.delta_gpu, 1, l.temp3_gpu, 1);

    copy_ongpu(l.outputs * l.batch, l.c_gpu, 1, l.temp_gpu, 1);
    activate_array_ongpu(l.temp_gpu, l.outputs * l.batch, TANH);

    copy_ongpu(l.outputs * l.batch, l.temp3_gpu, 1, l.temp2_gpu, 1);
    mul_ongpu(l.outputs * l.batch, l.o_gpu, 1, l.temp2_gpu, 1);

    gradient_array_ongpu(l.temp_gpu, l.outputs * l.batch, TANH, l.temp2_gpu);
    axpy_ongpu(l.outputs * l.batch, 1, l.dc_gpu, 1, l.temp2_gpu, 1);

    copy_ongpu(l.outputs * l.batch, l.c_gpu, 1, l.temp_gpu, 1);
    activate_array_ongpu(l.temp_gpu, l.outputs * l.batch, TANH);
    mul_ongpu(l.outputs * l.batch, l.temp3_gpu, 1, l.temp_gpu, 1);
    gradient_array_ongpu(l.o_gpu, l.outputs * l.batch, LOGISTIC, l.temp_gpu);
    copy_ongpu(l.outputs * l.batch, l.temp_gpu, 1, wo.delta_gpu, 1);
    s.input = l.prev_state_gpu;
    s.delta = l.dh_gpu;
    backward_connected_layer_gpu(wo, s);

    copy_ongpu(l.outputs * l.batch, l.temp_gpu, 1, uo.delta_gpu, 1);
    s.input = state.input;
    s.delta = state.delta;
    backward_connected_layer_gpu(uo, s);

    copy_ongpu(l.outputs * l.batch, l.temp2_gpu, 1, l.temp_gpu, 1);
    mul_ongpu(l.outputs * l.batch, l.i_gpu, 1, l.temp_gpu, 1);
    gradient_array_ongpu(l.g_gpu, l.outputs * l.batch, TANH, l.temp_gpu);
    copy_ongpu(l.outputs * l.batch, l.temp_gpu, 1, wg.delta_gpu, 1);
    s.input = l.prev_state_gpu;
    s.delta = l.dh_gpu;
    backward_connected_layer_gpu(wg, s);

    copy_ongpu(l.outputs * l.batch, l.temp_gpu, 1, ug.delta_gpu, 1);
    s.input = state.input;
    s.delta = state.delta;
    backward_connected_layer_gpu(ug, s);

    copy_ongpu(l.outputs * l.batch, l.temp2_gpu, 1, l.temp_gpu, 1);
    mul_ongpu(l.outputs * l.batch, l.g_gpu, 1, l.temp_gpu, 1);
    gradient_array_ongpu(l.i_gpu, l.outputs * l.batch, LOGISTIC, l.temp_gpu);
    copy_ongpu(l.outputs * l.batch, l.temp_gpu, 1, wi.delta_gpu, 1);
    s.input = l.prev_state_gpu;
    s.delta = l.dh_gpu;
    backward_connected_layer_gpu(wi, s);

    copy_ongpu(l.outputs * l.batch, l.temp_gpu, 1, ui.delta_gpu, 1);
    s.input = state.input;
    s.delta = state.delta;
    backward_connected_layer_gpu(ui, s);

    copy_ongpu(l.outputs * l.batch, l.temp2_gpu, 1, l.temp_gpu, 1);
    mul_ongpu(l.outputs * l.batch, l.prev_cell_gpu, 1, l.temp_gpu, 1);
    gradient_array_ongpu(l.f_gpu, l.outputs * l.batch, LOGISTIC, l.temp_gpu);
    copy_ongpu(l.outputs * l.batch, l.temp_gpu, 1, wf.delta_gpu, 1);
    s.input = l.prev_state_gpu;
    s.delta = l.dh_gpu;
    backward_connected_layer_gpu(wf, s);

    copy_ongpu(l.outputs * l.batch, l.temp_gpu, 1, uf.delta_gpu, 1);
    s.input = state.input;
    s.delta = state.delta;
    backward_connected_layer_gpu(uf, s);

    copy_ongpu(l.outputs * l.batch, l.temp2_gpu, 1, l.temp_gpu, 1);
    mul_ongpu(l.outputs * l.batch, l.f_gpu, 1, l.temp_gpu, 1);
    copy_ongpu(l.outputs * l.batch, l.temp_gpu, 1, l.dc_gpu, 1);

    state.input -= l.inputs * l.batch;
    if (state.delta) state.delta -= l.inputs * l.batch;
    l.output_gpu -= l.outputs * l.batch;
    l.cell_gpu -= l.outputs * l.batch;
    l.delta_gpu -= l.outputs * l.batch;

    increment_layer(&wf, -1);
    increment_layer(&wi, -1);
    increment_layer(&wg, -1);
    increment_layer(&wo, -1);

    increment_layer(&uf, -1);
    increment_layer(&ui, -1);
    increment_layer(&ug, -1);
    increment_layer(&uo, -1);
  }
}
#endif
