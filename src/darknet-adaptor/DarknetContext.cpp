#include "src/darknet-adaptor/DarknetContext.h"

#include <string.h>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/helpers/StringTools.h"
#include "src/log/log.h"

namespace darknet_adaptor
{

DarknetContext::~DarknetContext()
{
  if (mysub == nullptr) {
    return;
  }

  if (actx == nullptr) {
    return;
  }

  auto& subs = actx->stop_subscribers;

  auto siter = std::find(subs.begin(), subs.end(), mysub);

  if (siter != subs.end()) {
    subs.erase(siter);
  }
}

DarknetContext::DarknetContext(AppCtxPtr nactx) : actx{nactx}
{
  assert(actx != nullptr);
}

bool DarknetContext::init()
{
  assert(actx != nullptr);

  internal = create_subcontext();

  if (!propagate_context_params()) {
    LOGE("Fail to propagate app context params into internal");
    return false;
  }

  return true;
}

DarknetContext::detector_context_ptr DarknetContext::get_darknet_ctx()
{
  return internal;
}

bool DarknetContext::propagate_context_params()
{
  assert(actx != nullptr);
  assert(internal != nullptr);

  set_Avgframes(actx->avgframes);
  set_Benchmark(actx->benchmark);
  set_Benchmark_layers(actx->benchmark_layers);
  set_Cam_index(actx->cam_index);
  set_Chart_path(actx->chart_path);
  set_Clear(actx->clear);
  set_Detect_cfg(actx->detect_cfg);
  set_Dontdraw_bbox(actx->dontdraw_bbox);
  set_Dont_save_charts_every_iter(actx->dont_save_charts_every_iter);
  set_Dont_show(actx->dont_show);
  set_Draw_precision(actx->draw_precision);
  set_Ext_output(actx->ext_output);
  set_Frame_skip(actx->frame_skip);
  set_Gpus(actx->gpus);
  set_Height(actx->height);
  set_Hier_thresh(actx->hier_thresh);
  set_Http_post_host(actx->http_post_host);
  set_Iou_thresh(actx->iou_thresh);
  set_Json_file_output(actx->json_file_output);
  set_Json_port(actx->json_port);
  set_Letter_box(actx->letter_box);
  set_Map(actx->map);
  set_MAP_epochs(actx->mAP_epochs);
  set_Mjpeg_port(actx->mjpeg_port);
  set_Num_of_clusters(actx->num_of_clusters);
  set_Outfile(actx->outfile);
  set_Out_filename(actx->out_filename);
  set_Points(actx->points);
  set_Prefix(actx->prefix);
  set_Save_labels(actx->save_labels);
  set_Show(actx->show);
  set_Show_imgs(actx->show_imgs);
  set_Thresh(actx->thresh);
  set_Time_limit_sec(actx->time_limit_sec);
  set_Train_cfg(actx->train_cfg);
  set_Width(actx->width);
  set_Latest_weights(actx->latest_weights);
  set_Dont_resize_network(actx->dont_resize_network);
  set_image(actx->image);
  set_reload_data(actx->reload_data);
  set_stop(static_cast<int>(actx->stop.load()));
  set_net_nth_resize(actx->net_nth_resize);
  nth_iteration_reload(actx->nth_iteration_reload);
  stop_less_avg_loss(actx->stop_less_avg_loss);

  static std::function<void()> mysubfcn = [this]() -> void {
    if (actx == nullptr) {
      return;
    }

    set_stop(static_cast<int>(actx->stop.load()));
  };

  mysub = std::make_shared<std::function<void()>>(mysubfcn);

  actx->stop_subscribers.emplace_back(mysub);

  return true;
}

void DarknetContext::set_stop(const bool nstop)
{
  assert(internal != nullptr);

  // atomic_store(internal->stop, static_cast<bool>(nstop));
  internal->stop = static_cast<bool>(nstop);
}

bool DarknetContext::get_stop()
{
  assert(internal != nullptr);

  return internal->stop != 0;
}

DarknetContext::detector_context_ptr DarknetContext::create_subcontext()
{
  auto ctx = std::make_shared<detector_context>();

  memset(ctx.get(), 0, sizeof(detector_context));

  return ctx;
}

const bool& DarknetContext::get_Dont_show() const
{
  assert(internal != nullptr);

  return internal->dont_show;
}

void DarknetContext::set_Dont_show(const bool& ndont_show)
{
  assert(internal != nullptr);

  internal->dont_show = ndont_show;
}

void DarknetContext::set_Dont_show(bool&& ndont_show)
{
  assert(internal != nullptr);

  internal->dont_show = std::move(ndont_show);
}

const bool& DarknetContext::get_Benchmark() const
{
  assert(internal != nullptr);

  return internal->benchmark;
}

void DarknetContext::set_Benchmark(const bool& nbenchmark)
{
  assert(internal != nullptr);

  internal->benchmark = nbenchmark;
}

void DarknetContext::set_Benchmark(bool&& nbenchmark)
{
  assert(internal != nullptr);

  internal->benchmark = std::move(nbenchmark);
}

const bool& DarknetContext::get_Benchmark_layers() const
{
  assert(internal != nullptr);

  return internal->benchmark_layers;
}

void DarknetContext::set_Benchmark_layers(const bool& nbenchmark_layers)
{
  assert(internal != nullptr);

  internal->benchmark_layers = nbenchmark_layers;
}

void DarknetContext::set_Benchmark_layers(bool&& nbenchmark_layers)
{
  assert(internal != nullptr);

  internal->benchmark_layers = std::move(nbenchmark_layers);
}

const bool& DarknetContext::get_Show() const
{
  assert(internal != nullptr);

  return internal->show;
}

void DarknetContext::set_Show(const bool& nshow)
{
  assert(internal != nullptr);

  internal->show = nshow;
}

void DarknetContext::set_Show(bool&& nshow)
{
  assert(internal != nullptr);

  internal->show = std::move(nshow);
}

const bool& DarknetContext::get_Letter_box() const
{
  assert(internal != nullptr);

  return internal->letter_box;
}

void DarknetContext::set_Letter_box(const bool& nletter_box)
{
  assert(internal != nullptr);

  internal->letter_box = nletter_box;
}

void DarknetContext::set_Letter_box(bool&& nletter_box)
{
  assert(internal != nullptr);

  internal->letter_box = std::move(nletter_box);
}

const bool& DarknetContext::get_Map() const
{
  assert(internal != nullptr);

  return internal->calc_map;
}

void DarknetContext::set_Map(const bool& nmap)
{
  assert(internal != nullptr);

  internal->calc_map = nmap;
}

void DarknetContext::set_Map(bool&& nmap)
{
  assert(internal != nullptr);

  internal->calc_map = std::move(nmap);
}

const int& DarknetContext::get_Points() const
{
  assert(internal != nullptr);

  return internal->map_points;
}

void DarknetContext::set_Points(const int& npoints)
{
  assert(internal != nullptr);

  internal->map_points = npoints;
}

void DarknetContext::set_Points(int&& npoints)
{
  assert(internal != nullptr);

  internal->map_points = std::move(npoints);
}

const bool& DarknetContext::get_Show_imgs() const
{
  assert(internal != nullptr);

  return internal->show_imgs;
}

void DarknetContext::set_Show_imgs(const bool& nshow_imgs)
{
  assert(internal != nullptr);

  internal->show_imgs = nshow_imgs;
}

void DarknetContext::set_Show_imgs(bool&& nshow_imgs)
{
  assert(internal != nullptr);

  internal->show_imgs = std::move(nshow_imgs);
}

const int& DarknetContext::get_Mjpeg_port() const
{
  assert(internal != nullptr);

  return internal->mjpeg_port;
}

void DarknetContext::set_Mjpeg_port(const int& nmjpeg_port)
{
  assert(internal != nullptr);

  internal->mjpeg_port = nmjpeg_port;
}

void DarknetContext::set_Mjpeg_port(int&& nmjpeg_port)
{
  assert(internal != nullptr);

  internal->mjpeg_port = std::move(nmjpeg_port);
}

const int& DarknetContext::get_Avgframes() const
{
  assert(internal != nullptr);

  return internal->avgframes;
}

void DarknetContext::set_Avgframes(const int& navgframes)
{
  assert(internal != nullptr);

  internal->avgframes = navgframes;
}

void DarknetContext::set_Avgframes(int&& navgframes)
{
  assert(internal != nullptr);

  internal->avgframes = std::move(navgframes);
}

const bool& DarknetContext::get_Dontdraw_bbox() const
{
  assert(internal != nullptr);

  return internal->dontdraw_bbox;
}

void DarknetContext::set_Dontdraw_bbox(const bool& ndontdraw_bbox)
{
  assert(internal != nullptr);

  internal->dontdraw_bbox = ndontdraw_bbox;
}

void DarknetContext::set_Dontdraw_bbox(bool&& ndontdraw_bbox)
{
  assert(internal != nullptr);

  internal->dontdraw_bbox = std::move(ndontdraw_bbox);
}

const int& DarknetContext::get_Json_port() const
{
  assert(internal != nullptr);

  return internal->json_port;
}

void DarknetContext::set_Json_port(const int& njson_port)
{
  assert(internal != nullptr);

  internal->json_port = njson_port;
}

void DarknetContext::set_Json_port(int&& njson_port)
{
  assert(internal != nullptr);

  internal->json_port = std::move(njson_port);
}

const std::string& DarknetContext::get_Http_post_host() const
{
  assert(internal != nullptr);

  return http_post_host;
}

void DarknetContext::set_Http_post_host(const std::string& nhttp_post_host)
{
  assert(internal != nullptr);

  http_post_host = nhttp_post_host;

  internal->http_post_host = http_post_host.data();
}

void DarknetContext::set_Http_post_host(std::string&& nhttp_post_host)
{
  assert(internal != nullptr);

  http_post_host = std::move(nhttp_post_host);

  internal->http_post_host = http_post_host.data();
}

const int& DarknetContext::get_Time_limit_sec() const
{
  assert(internal != nullptr);

  return internal->time_limit_sec;
}

void DarknetContext::set_Time_limit_sec(const int& ntime_limit_sec)
{
  assert(internal != nullptr);

  internal->time_limit_sec = ntime_limit_sec;
}

void DarknetContext::set_Time_limit_sec(int&& ntime_limit_sec)
{
  assert(internal != nullptr);

  internal->time_limit_sec = std::move(ntime_limit_sec);
}

const std::string& DarknetContext::get_Out_filename() const
{
  assert(internal != nullptr);

  return out_filename;
}

void DarknetContext::set_Out_filename(const std::string& nout_filename)
{
  assert(internal != nullptr);

  out_filename = nout_filename;

  internal->out_filename = out_filename.data();
}

void DarknetContext::set_Out_filename(std::string&& nout_filename)
{
  assert(internal != nullptr);

  out_filename = std::move(nout_filename);

  internal->out_filename = out_filename.data();
}

const std::string& DarknetContext::get_Json_file_output() const
{
  assert(internal != nullptr);

  return json_file_output;
}

void DarknetContext::set_Json_file_output(const std::string& njson_file_output)
{
  assert(internal != nullptr);

  json_file_output = njson_file_output;

  internal->json_file_output = json_file_output.data();
}

void DarknetContext::set_Json_file_output(std::string&& njson_file_output)
{
  assert(internal != nullptr);

  json_file_output = std::move(njson_file_output);

  internal->json_file_output = json_file_output.data();
}

const std::string& DarknetContext::get_Outfile() const
{
  assert(internal != nullptr);
  return outfile;
}

void DarknetContext::set_Outfile(const std::string& noutfile)
{
  assert(internal != nullptr);

  outfile = noutfile;

  if (outfile.empty()) {
    internal->outfile = NULL;
    return;
  }

  internal->outfile = outfile.data();

  LOGD("Found out file: " << outfile);
}

void DarknetContext::set_Outfile(std::string&& noutfile)
{
  assert(internal != nullptr);

  outfile = std::move(noutfile);

  if (outfile.empty()) {
    internal->outfile = NULL;
    return;
  }

  internal->outfile = outfile.data();

  LOGD("Found out file: " << outfile);
}

const std::string& DarknetContext::get_Prefix() const
{
  assert(internal != nullptr);

  return prefix;
}

void DarknetContext::set_Prefix(const std::string& nprefix)
{
  assert(internal != nullptr);

  prefix = nprefix;

  if (prefix.empty()) {
    internal->prefix = NULL;
    return;
  }

  internal->prefix = prefix.data();

  LOGD("Found prefix: " << prefix);
}

void DarknetContext::set_Prefix(std::string&& nprefix)
{
  assert(internal != nullptr);

  prefix = std::move(nprefix);

  if (prefix.empty()) {
    internal->prefix = NULL;
    return;
  }

  internal->prefix = prefix.data();

  LOGD("Found prefix: " << prefix);
}

const float& DarknetContext::get_Thresh() const
{
  assert(internal != nullptr);

  return internal->thresh;
}

void DarknetContext::set_Thresh(const float& nthresh)
{
  assert(internal != nullptr);

  internal->thresh = nthresh;
}

void DarknetContext::set_Thresh(float&& nthresh)
{
  assert(internal != nullptr);

  internal->thresh = std::move(nthresh);
}

const float& DarknetContext::get_Iou_thresh() const
{
  assert(internal != nullptr);

  return internal->iou_thresh;
}

void DarknetContext::set_Iou_thresh(const float& niou_thresh)
{
  assert(internal != nullptr);

  internal->iou_thresh = niou_thresh;
}

void DarknetContext::set_Iou_thresh(float&& niou_thresh)
{
  assert(internal != nullptr);

  internal->iou_thresh = std::move(niou_thresh);
}

const float& DarknetContext::get_Hier_thresh() const
{
  assert(internal != nullptr);

  return internal->hier_thresh;
}

void DarknetContext::set_Hier_thresh(const float& nhier_thresh)
{
  assert(internal != nullptr);

  internal->hier_thresh = nhier_thresh;
}

void DarknetContext::set_Hier_thresh(float&& nhier_thresh)
{
  assert(internal != nullptr);

  internal->hier_thresh = std::move(nhier_thresh);
}

const int& DarknetContext::get_Cam_index() const
{
  assert(internal != nullptr);

  return internal->cam_index;
}

void DarknetContext::set_Cam_index(const int& ncam_index)
{
  assert(internal != nullptr);

  internal->cam_index = ncam_index;
}

void DarknetContext::set_Cam_index(int&& ncam_index)
{
  assert(internal != nullptr);

  internal->cam_index = std::move(ncam_index);
}

const int& DarknetContext::get_Frame_skip() const
{
  assert(internal != nullptr);

  return internal->frame_skip;
}

void DarknetContext::set_Frame_skip(const int& nframe_skip)
{
  assert(internal != nullptr);

  internal->frame_skip = nframe_skip;
}

void DarknetContext::set_Frame_skip(int&& nframe_skip)
{
  assert(internal != nullptr);

  internal->frame_skip = std::move(nframe_skip);
}

const int& DarknetContext::get_Num_of_clusters() const
{
  assert(internal != nullptr);

  return internal->num_of_clusters;
}

void DarknetContext::set_Num_of_clusters(const int& nnum_of_clusters)
{
  assert(internal != nullptr);

  internal->num_of_clusters = nnum_of_clusters;
}

void DarknetContext::set_Num_of_clusters(int&& nnum_of_clusters)
{
  assert(internal != nullptr);

  internal->num_of_clusters = std::move(nnum_of_clusters);
}

const int& DarknetContext::get_Width() const
{
  assert(internal != nullptr);

  return internal->width;
}

void DarknetContext::set_Width(const int& nwidth)
{
  assert(internal != nullptr);

  internal->width = nwidth;
}

void DarknetContext::set_Width(int&& nwidth)
{
  assert(internal != nullptr);

  internal->width = std::move(nwidth);
}

const int& DarknetContext::get_Height() const
{
  assert(internal != nullptr);

  return internal->height;
}

void DarknetContext::set_Height(const int& nheight)
{
  assert(internal != nullptr);

  internal->height = nheight;
}

void DarknetContext::set_Height(int&& nheight)
{
  assert(internal != nullptr);

  internal->height = std::move(nheight);
}

const bool& DarknetContext::get_Ext_output() const
{
  assert(internal != nullptr);

  return internal->ext_output;
}

void DarknetContext::set_Ext_output(const bool& next_output)
{
  assert(internal != nullptr);

  internal->ext_output = next_output;
}

void DarknetContext::set_Ext_output(bool&& next_output)
{
  assert(internal != nullptr);

  internal->ext_output = std::move(next_output);
}

const bool& DarknetContext::get_Save_labels() const
{
  assert(internal != nullptr);

  return internal->save_labels;
}

void DarknetContext::set_Save_labels(const bool& nsave_labels)
{
  assert(internal != nullptr);

  internal->save_labels = nsave_labels;
}

void DarknetContext::set_Save_labels(bool&& nsave_labels)
{
  assert(internal != nullptr);

  internal->save_labels = std::move(nsave_labels);
}

const std::string& DarknetContext::get_Chart_path() const
{
  assert(internal != nullptr);

  return chart_path;
}

void DarknetContext::set_Chart_path(const std::string& nchart_path)
{
  assert(internal != nullptr);

  chart_path = nchart_path;

  if (prefix.empty()) {
    internal->chart_path = NULL;
    return;
  }

  internal->chart_path = chart_path.data();

  LOGD("Found chart_path: " << chart_path);
}

void DarknetContext::set_Chart_path(std::string&& nchart_path)
{
  assert(internal != nullptr);

  chart_path = std::move(nchart_path);

  if (prefix.empty()) {
    internal->chart_path = NULL;
    return;
  }

  internal->chart_path = chart_path.data();

  LOGD("Found chart_path: " << chart_path);
}

const int& DarknetContext::get_MAP_epochs() const
{
  assert(internal != nullptr);

  return internal->mAP_epochs;
}

void DarknetContext::set_MAP_epochs(const int& nmAP_epochs)
{
  assert(internal != nullptr);

  internal->mAP_epochs = nmAP_epochs;
}

void DarknetContext::set_MAP_epochs(int&& nmAP_epochs)
{
  assert(internal != nullptr);

  internal->mAP_epochs = std::move(nmAP_epochs);
}

const std::string& DarknetContext::get_Gpus() const
{
  assert(internal != nullptr);

  return gpus;
}

inline std::vector<int> DarknetContext::prepare_gpus_list(
    const std::vector<std::string>& src)
{
  using ilist = std::vector<int>;

  ilist intlist;

  intlist.reserve(src.size());

  std::transform(src.begin(), src.end(), std::back_inserter(intlist),
                 [](const std::string& ii) { return std::stoi(ii); });

  return intlist;
}

void DarknetContext::set_Gpus(const std::string& ngpus)
{
  assert(internal != nullptr);

  gpus = ngpus;

  gpuslist = prepare_gpus_list(helpers::StringTools::split(gpus, ','));

  internal->ngpus = gpuslist.empty() ? 1 : gpuslist.size();
  internal->gpu_list = gpus.data();
  internal->gpus = gpuslist.data();
}

void DarknetContext::set_Gpus(std::string&& ngpus)
{
  assert(internal != nullptr);

  gpus = std::move(ngpus);

  gpuslist = prepare_gpus_list(helpers::StringTools::split(gpus, ','));

  internal->ngpus = gpuslist.empty() ? 1 : gpuslist.size();
  internal->gpu_list = gpus.data();
  internal->gpus = gpuslist.data();
}

const bool& DarknetContext::get_Clear() const
{
  assert(internal != nullptr);

  return internal->clear;
}

void DarknetContext::set_Clear(const bool& nclear)
{
  assert(internal != nullptr);

  internal->clear = nclear;
}

void DarknetContext::set_Clear(bool&& nclear)
{
  assert(internal != nullptr);

  internal->clear = std::move(nclear);
}

bool DarknetContext::get_Dont_save_charts_every_iter() const
{
  assert(internal != nullptr);

  return !internal->save_chart_every_iter;
}

void DarknetContext::set_Dont_save_charts_every_iter(
    const bool& ndont_save_charts_every_iter)
{
  assert(internal != nullptr);

  internal->save_chart_every_iter = !ndont_save_charts_every_iter;
}

const bool& DarknetContext::get_Draw_precision() const
{
  assert(internal != nullptr);

  return internal->draw_precision;
}

void DarknetContext::set_Draw_precision(const bool& ndraw_precision)
{
  assert(internal != nullptr);

  internal->draw_precision = ndraw_precision;
}

void DarknetContext::set_Draw_precision(bool&& ndraw_precision)
{
  assert(internal != nullptr);

  internal->draw_precision = std::move(ndraw_precision);
}

const std::string& DarknetContext::get_Train_cfg() const
{
  assert(internal != nullptr);

  return train_cfg;
}

void DarknetContext::set_Train_cfg(const std::string& ntrain_cfg)
{
  assert(internal != nullptr);

  if (ntrain_cfg.empty()) {
    return;
  }

  train_cfg = ntrain_cfg;

  internal->cfg = train_cfg.data();

  LOGD("Enabled train cfg: " << train_cfg);
}

void DarknetContext::set_Train_cfg(std::string&& ntrain_cfg)
{
  assert(internal != nullptr);

  if (ntrain_cfg.empty()) {
    return;
  }

  train_cfg = std::move(ntrain_cfg);

  internal->cfg = train_cfg.data();

  LOGD("Enabled train cfg: " << train_cfg);
}

const std::string& DarknetContext::get_Detect_cfg() const { return detect_cfg; }

void DarknetContext::set_Detect_cfg(const std::string& ndetect_cfg)
{
  assert(internal != nullptr);

  detect_cfg = ndetect_cfg;

  if (ndetect_cfg.empty()) {
    internal->cfg = NULL;
    return;
  }

  internal->cfg = detect_cfg.data();

  LOGD("Enabled detect cfg: " << detect_cfg);
}

void DarknetContext::set_Detect_cfg(std::string&& ndetect_cfg)
{
  assert(internal != nullptr);

  detect_cfg = std::move(ndetect_cfg);

  if (ndetect_cfg.empty()) {
    internal->cfg = NULL;
    return;
  }

  internal->cfg = detect_cfg.data();

  LOGD("Enabled detect cfg: " << detect_cfg);
}

const std::string& DarknetContext::get_Latest_weights() const
{
  return latest_weights;
}

void DarknetContext::set_Latest_weights(const std::string& nlatest_weights)
{
  latest_weights = nlatest_weights;

  if (latest_weights.empty()) {
    internal->weights = NULL;
    return;
  }

  internal->weights = latest_weights.data();

  LOGD("Latest weights file: " << latest_weights);
}

void DarknetContext::set_Latest_weights(std::string&& nlatest_weights)
{
  latest_weights = std::move(nlatest_weights);

  if (latest_weights.empty()) {
    internal->weights = NULL;
    return;
  }

  internal->weights = latest_weights.data();

  LOGD("Latest weights file: " << latest_weights);
}

bool DarknetContext::get_Dont_resize_network() const
{
  assert(internal != nullptr);

  return static_cast<bool>(internal->dont_resize_network);
}

void DarknetContext::set_Dont_resize_network(const bool& ndont_resize_network)
{
  assert(internal != nullptr);

  static_assert(static_cast<int>(true) == 1);
  static_assert(static_cast<int>(false) == 0);

  internal->dont_resize_network = static_cast<int>(ndont_resize_network);
}

const std::string& DarknetContext::get_image() const { return image; }

void DarknetContext::set_image(const std::string& nimage)
{
  assert(internal != nullptr);

  image = nimage;

  if (image.empty()) {
    internal->filename = NULL;
    return;
  }

  internal->filename = image.data();

  LOGD("Found image file provided: " << image);
}

void DarknetContext::set_image(std::string&& nimage)
{
  assert(internal != nullptr);

  image = std::move(nimage);

  if (image.empty()) {
    internal->filename = NULL;
    return;
  }

  internal->filename = image.data();

  LOGD("Found image file provided: " << image);
}

bool DarknetContext::get_reload_data() const
{
  assert(internal != nullptr);

  return static_cast<bool>(internal->reload_data);
}

void DarknetContext::set_reload_data(const bool nreload_data)
{
  assert(internal != nullptr);

  internal->reload_data = static_cast<int>(nreload_data);
}

void DarknetContext::set_net_nth_resize(const unsigned int& nth)
{
  assert(internal != nullptr);

  internal->resize_network_every_nth = nth;
}

const unsigned int& DarknetContext::get_net_nth_resize()
{
  assert(internal != nullptr);

  return internal->resize_network_every_nth;
}

void DarknetContext::nth_iteration_reload(const unsigned int& nth)
{
  assert(internal != nullptr);

  internal->nth_iteration_data_reload = nth;
}

const unsigned int& DarknetContext::nth_iteration_reload()
{
  assert(internal != nullptr);

  return internal->nth_iteration_data_reload;
}

void DarknetContext::stop_less_avg_loss(const double& avglosslim)
{
  assert(internal != nullptr);

  internal->stop_less_avg_loss = avglosslim;
}

const double& DarknetContext::stop_less_avg_loss()
{
  assert(internal != nullptr);

  return internal->stop_less_avg_loss;
}

}  // namespace darknet_adaptor
