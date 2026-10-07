#include "src/converters/ApplicationContext2LibraryContext.h"

#include <cassert>

namespace converters
{

bool ApplicationContext2LibraryContext::convert(app::AppCtxPtr appctx,
                                                LibraryContextPtr libctx)
{
  assert(appctx != nullptr);
  assert(libctx != nullptr);

  if (appctx == nullptr || libctx == nullptr) {
    return false;
  }

  convert_darknet_params(*appctx, *libctx);
  convert_conversion_params(*appctx, *libctx);

  libctx->stop_with(appctx->stop);

  return true;
}

void ApplicationContext2LibraryContext::convert_darknet_params(
    const app::ApplicationContext& appctx, darknetxx011::LibraryContext& libctx)
{
  libctx.train_cfg = appctx.train_cfg;
  libctx.detect_cfg = appctx.detect_cfg;
  libctx.names = appctx.names;
  libctx.latest_weights = appctx.latest_weights;
  libctx.image = appctx.image;
  libctx.gpus = appctx.gpus;
  libctx.threads = appctx.threads;
  libctx.dont_show = appctx.dont_show;
  libctx.benchmark = appctx.benchmark;
  libctx.benchmark_layers = appctx.benchmark_layers;
  libctx.show = appctx.show;
  libctx.letter_box = appctx.letter_box;
  libctx.map = appctx.map;
  libctx.points = appctx.points;
  libctx.show_imgs = appctx.show_imgs;
  libctx.mjpeg_port = appctx.mjpeg_port;
  libctx.avgframes = appctx.avgframes;
  libctx.dontdraw_bbox = appctx.dontdraw_bbox;
  libctx.json_port = appctx.json_port;
  libctx.http_post_host = appctx.http_post_host;
  libctx.time_limit_sec = appctx.time_limit_sec;
  libctx.out_filename = appctx.out_filename;
  libctx.json_file_output = appctx.json_file_output;
  libctx.outfile = appctx.outfile;
  libctx.prefix = appctx.prefix;
  libctx.thresh = appctx.thresh;
  libctx.iou_thresh = appctx.iou_thresh;
  libctx.hier_thresh = appctx.hier_thresh;
  libctx.cam_index = appctx.cam_index;
  libctx.frame_skip = appctx.frame_skip;
  libctx.num_of_clusters = appctx.num_of_clusters;
  libctx.width = appctx.width;
  libctx.height = appctx.height;
  libctx.ext_output = appctx.ext_output;
  libctx.save_labels = appctx.save_labels;
  libctx.chart_path = appctx.chart_path;
  libctx.mAP_epochs = appctx.mAP_epochs;
  libctx.clear = appctx.clear;
  libctx.dont_save_charts_every_iter = appctx.dont_save_charts_every_iter;
  libctx.draw_precision = appctx.draw_precision;
  libctx.dont_resize_network = appctx.dont_resize_network;
  libctx.reload_data = appctx.reload_data;
  libctx.net_nth_resize = appctx.net_nth_resize;
  libctx.nth_iteration_reload = appctx.nth_iteration_reload;
  libctx.stop_less_avg_loss = appctx.stop_less_avg_loss;
  libctx.save_format = appctx.save_format;
}

void ApplicationContext2LibraryContext::convert_conversion_params(
    const app::ApplicationContext& appctx, darknetxx011::LibraryContext& libctx)
{
  libctx.convert_from = appctx.convert_from;
  libctx.convert_src = appctx.convert_src;
  libctx.convert_to = appctx.convert_to;
  libctx.convert_dst = appctx.convert_dst;
  libctx.convert_cfg = appctx.convert_cfg;
  libctx.convert_names = appctx.convert_names;
}

}  // namespace converters
