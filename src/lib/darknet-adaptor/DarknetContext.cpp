#include "src/lib/darknet-adaptor/DarknetContext.h"

#include <string.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <thread>

#include "src/gpu_index_decls.h"
#include "src/lib/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksCApi/NetworksRegistry.h"
#include "src/lib/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h"
#include "src/lib/facade/public/ITrainingListener.h"
#include "src/lib/facade/public/LibraryContext.h"
#include "src/lib/facade/public/TrainingProgress.h"
#include "src/lib/helpers/StringTools.h"
#include "src/log/log.h"
#include "src/network.h"

namespace darknet_adaptor
{

DarknetContext::~DarknetContext()
{
  stop_stop_watcher();

  if (internal != nullptr) {
    release_networks();
  }
}

DarknetContext::DarknetContext(LibCtxPtr nlctx) : lctx{nlctx}
{
  assert(lctx != nullptr);
}

bool DarknetContext::init()
{
  assert(lctx != nullptr);

  internal = create_subcontext();

  if (!propagate_context_params()) {
    LOGE("Fail to propagate app context params into internal");
    return false;
  }

  start_stop_watcher();

  return true;
}

DarknetContext::detector_context_ptr DarknetContext::get_darknet_ctx()
{
  return internal;
}

bool DarknetContext::propagate_context_params()
{
  assert(lctx != nullptr);
  assert(internal != nullptr);

  set_Avgframes(lctx->avgframes);
  set_Benchmark(lctx->benchmark);
  set_Benchmark_layers(lctx->benchmark_layers);
  set_Cam_index(lctx->cam_index);
  set_Chart_path(lctx->chart_path);
  set_Clear(lctx->clear);
  set_Detect_cfg(lctx->detect_cfg);
  set_Names(lctx->names);
  set_Dontdraw_bbox(lctx->dontdraw_bbox);
  set_Dont_save_charts_every_iter(lctx->dont_save_charts_every_iter);
  set_Dont_show(lctx->dont_show);
  set_Draw_precision(lctx->draw_precision);
  set_Ext_output(lctx->ext_output);
  set_Frame_skip(lctx->frame_skip);
  set_Gpus(lctx->gpus);
  set_Height(lctx->height);
  set_Hier_thresh(lctx->hier_thresh);
  set_Http_post_host(lctx->http_post_host);
  set_Iou_thresh(lctx->iou_thresh);
  set_Json_file_output(lctx->json_file_output);
  set_Json_port(lctx->json_port);
  set_Letter_box(lctx->letter_box);
  set_Map(lctx->map);
  set_MAP_epochs(lctx->mAP_epochs);
  set_Mjpeg_port(lctx->mjpeg_port);
  set_Num_of_clusters(lctx->num_of_clusters);
  set_Outfile(lctx->outfile);
  set_Out_filename(lctx->out_filename);
  set_Points(lctx->points);
  set_Prefix(lctx->prefix);
  set_Save_labels(lctx->save_labels);
  set_Show(lctx->show);
  set_Show_imgs(lctx->show_imgs);
  set_Thresh(lctx->thresh);
  set_Time_limit_sec(lctx->time_limit_sec);
  set_Train_cfg(lctx->train_cfg);
  set_Width(lctx->width);
  set_Latest_weights(lctx->latest_weights);
  set_Dont_resize_network(lctx->dont_resize_network);
  set_image(lctx->image);
  set_reload_data(lctx->reload_data);
  set_stop(lctx->stop_requested());
  set_net_nth_resize(lctx->net_nth_resize);
  nth_iteration_reload(lctx->nth_iteration_reload);
  stop_less_avg_loss(lctx->stop_less_avg_loss);
  listen_training();

  return true;
}

void DarknetContext::listen_training()
{
  assert(internal != nullptr);

  internal->iteration_listener = &DarknetContext::report_iteration;
  internal->iteration_listener_data = this;
}

void DarknetContext::report_iteration(const detector_context* ctx, void* data)
{
  assert(ctx != nullptr);
  assert(data != nullptr);

  const auto* dctx = static_cast<const DarknetContext*>(data);

  dctx->record(*ctx);

  if (dctx->lctx->training_listener == nullptr) {
    return;
  }

  dctx->lctx->training_listener->on_iteration(darknetxx011::TrainingProgress{
      ctx->current_iteration, ctx->net.max_batches, ctx->loss, ctx->avg_loss,
      get_current_rate(ctx->net), ctx->avg_time});
}

int DarknetContext::save_weights(const detector_context* ctx,
                                 const char* const stem, const int ema,
                                 void* data)
{
  assert(ctx != nullptr);
  assert(data != nullptr);

  const auto* dctx = static_cast<const DarknetContext*>(data);

  if (dctx->journal == nullptr) {
    LOGE("No training journal to save the weights by");
    return 0;
  }

  // the iteration saved before it's report keeps it's losses too
  dctx->record(*ctx);

  network view = ctx->net;

  return dctx->journal->save(view, stem, ema != 0) ? 1 : 0;
}

void DarknetContext::record(const detector_context& ctx) const
{
  if (journal == nullptr) {
    return;
  }

  journal->record(ctx.current_iteration, ctx.loss, ctx.avg_loss);

  if (ctx.map_calculated) {
    journal->record_map(ctx.iter_map, ctx.mean_average_precision);
  }
}

const std::string& DarknetContext::get_Save_format() const
{
  return lctx->save_format;
}

void DarknetContext::set_training_journal(TrainingJournalPtr njournal)
{
  assert(internal != nullptr);

  journal = std::move(njournal);

  internal->weights_saver =
      journal != nullptr ? &DarknetContext::save_weights : nullptr;
  internal->weights_saver_data = this;

  internal->resumed = train_chart_record{};

  if (journal == nullptr) {
    return;
  }

  const loaders::TrainingRecord& record = journal->rest().training;

  internal->resumed = train_chart_record{record.iterations().data(),
                                         record.avg_losses().data(),
                                         static_cast<int>(record.size()),
                                         record.map_iterations().data(),
                                         record.maps().data(),
                                         static_cast<int>(record.map_size())};
}

void DarknetContext::set_training_base(std::string nbase)
{
  assert(internal != nullptr);

  training_base = std::move(nbase);
  internal->base_name = training_base.empty() ? nullptr : training_base.data();
}

const DarknetContext::TrainingJournalPtr& DarknetContext::get_training_journal()
    const
{
  return journal;
}

void DarknetContext::set_class_names(std::vector<std::string> nnames)
{
  assert(internal != nullptr);

  class_names = std::move(nnames);
  class_names_view.clear();

  for (std::string& name : class_names) {
    class_names_view.push_back(name.data());
  }

  internal->names =
      class_names_view.empty() ? nullptr : class_names_view.data();
  internal->names_size = static_cast<int>(class_names_view.size());
}

void DarknetContext::start_stop_watcher()
{
  assert(lctx != nullptr);

  if (lctx == nullptr) {
    LOGE("No valid library context to watch the stop request of");
    return;
  }

  stop_stop_watcher();

  mwatching.store(true);

  mstop_watcher = std::thread{[this]() -> void { watch_the_app_stop_flag(); }};

  LOGD("Watching the stop request");
}

void DarknetContext::stop_stop_watcher()
{
  mwatching.store(false);

  if (mstop_watcher.joinable()) {
    mstop_watcher.join();
  }
}

void DarknetContext::watch_the_app_stop_flag()
{
  // Long enough to cost nothing at all and short enough to reach the original
  // Darknet loops long before their current iteration ends.
  static constexpr const std::chrono::milliseconds pollInterval{200};

  while (mwatching.load()) {
    if (lctx != nullptr && lctx->stop_requested()) {
      set_stop(true);

      // the only feedback before the current step of the work ends
      LOGW("The stop is requested, stopping once the current step ends");
      return;
    }

    std::this_thread::sleep_for(pollInterval);
  }
}

void DarknetContext::set_stop(const bool nstop)
{
  assert(internal != nullptr);

  detector_context_set_stop(internal.get(), nstop ? 1 : 0);
}

bool DarknetContext::get_stop()
{
  assert(internal != nullptr);

  return detector_context_is_stopped(internal.get()) != 0;
}

DarknetContext::detector_context_ptr DarknetContext::create_subcontext()
{
  auto ctx = std::make_shared<detector_context>();

  memset(ctx.get(), 0, sizeof(detector_context));

  return ctx;
}

bool DarknetContext::get_Dont_show() const
{
  assert(internal != nullptr);

  return internal->dont_show != 0;
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

bool DarknetContext::get_Benchmark() const
{
  assert(internal != nullptr);

  return internal->benchmark != 0;
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

bool DarknetContext::get_Benchmark_layers() const
{
  assert(internal != nullptr);

  return internal->benchmark_layers != 0;
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

bool DarknetContext::get_Show() const
{
  assert(internal != nullptr);

  return internal->show != 0;
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

bool DarknetContext::get_Letter_box() const
{
  assert(internal != nullptr);

  return internal->letter_box != 0;
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

bool DarknetContext::get_Map() const
{
  assert(internal != nullptr);

  return internal->calc_map != 0;
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

bool DarknetContext::get_Show_imgs() const
{
  assert(internal != nullptr);

  return internal->show_imgs != 0;
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

bool DarknetContext::get_Dontdraw_bbox() const
{
  assert(internal != nullptr);

  return internal->dontdraw_bbox != 0;
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

bool DarknetContext::get_Ext_output() const
{
  assert(internal != nullptr);

  return internal->ext_output != 0;
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

bool DarknetContext::get_Save_labels() const
{
  assert(internal != nullptr);

  return internal->save_labels != 0;
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

  if (chart_path.empty()) {
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

  if (chart_path.empty()) {
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

  // The command line parser validates the list already, so a malformed index
  // is reported and skipped here instead of terminating the application.
  for (const std::string& index : src) {
    try {
      intlist.push_back(std::stoi(index));
    }
    catch (const std::logic_error&) {
      LOGE("Skipping the invalid GPU index: '" << index << "'");
    }
  }

  return intlist;
}

void DarknetContext::set_Gpus(const std::string& ngpus)
{
  assert(internal != nullptr);

  gpus = ngpus;

  gpuslist = prepare_gpus_list(helpers::StringTools::split(gpus, ','));

  // without the --gpus list the computation takes the default GPU (the
  // original Darknet one), so the ctx->gpus is never a NULL pointer
  if (gpuslist.empty()) {
    gpuslist.push_back(gpu_index);
  }

  internal->ngpus = static_cast<int>(gpuslist.size());
  internal->gpu_list = gpus.data();
  internal->gpus = gpuslist.data();
}

void DarknetContext::set_Gpus(std::string&& ngpus)
{
  assert(internal != nullptr);

  gpus = std::move(ngpus);

  gpuslist = prepare_gpus_list(helpers::StringTools::split(gpus, ','));

  // without the --gpus list the computation takes the default GPU (the
  // original Darknet one), so the ctx->gpus is never a NULL pointer
  if (gpuslist.empty()) {
    gpuslist.push_back(gpu_index);
  }

  internal->ngpus = static_cast<int>(gpuslist.size());
  internal->gpu_list = gpus.data();
  internal->gpus = gpuslist.data();
}

bool DarknetContext::get_Clear() const
{
  assert(internal != nullptr);

  return internal->clear != 0;
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

bool DarknetContext::get_Draw_precision() const
{
  assert(internal != nullptr);

  return internal->draw_precision != 0;
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

  if (detect_cfg.empty()) {
    internal->cfg = NULL;
    return;
  }

  internal->cfg = detect_cfg.data();

  LOGD("Enabled detect cfg: " << detect_cfg);
}

const std::string& DarknetContext::get_Names() const { return names; }

void DarknetContext::set_Names(const std::string& nnames)
{
  assert(internal != nullptr);

  names = nnames;

  if (names.empty()) {
    internal->name_list = NULL;
    return;
  }

  internal->name_list = names.data();

  LOGD("Class names file: " << names);
}

void DarknetContext::set_Names(std::string&& nnames)
{
  assert(internal != nullptr);

  names = std::move(nnames);

  if (names.empty()) {
    internal->name_list = NULL;
    return;
  }

  internal->name_list = names.data();

  LOGD("Class names file: " << names);
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

void DarknetContext::set_train_networks(ANetworkAdaptorPtrs nnets)
{
  assert(internal != nullptr);

  for (const auto& net : train_networks) {
    adaptors::NetworksRegistry::remove(*net->get());
  }

  train_networks = std::move(nnets);
  train_views.clear();

  for (const auto& net : train_networks) {
    assert(net != nullptr);

    adaptors::NetworksRegistry::add(net, false);
    train_views.push_back(*net->get());
  }

  internal->nets = train_views.empty() ? nullptr : train_views.data();

  if (train_views.empty()) {
    std::memset(&internal->net, 0, sizeof(internal->net));
  } else {
    internal->net = train_views.front();
  }
}

void DarknetContext::set_map_network(ANetworkAdaptorPtr nnet)
{
  assert(internal != nullptr);

  if (map_network) {
    adaptors::NetworksRegistry::remove(*map_network->get());
  }

  map_network = std::move(nnet);

  if (!map_network) {
    std::memset(&internal->net_map, 0, sizeof(internal->net_map));
    return;
  }

  adaptors::NetworksRegistry::add(map_network, false);
  internal->net_map = *map_network->get();
}

void DarknetContext::set_detect_network(ANetworkAdaptorPtr nnet)
{
  assert(internal != nullptr);

  if (detect_network) {
    adaptors::NetworksRegistry::remove(*detect_network->get());
  }

  detect_network = std::move(nnet);

  if (!detect_network) {
    std::memset(&internal->net, 0, sizeof(internal->net));
    return;
  }

  adaptors::NetworksRegistry::add(detect_network, false);
  internal->net = *detect_network->get();
}

const DarknetContext::ANetworkAdaptorPtrs& DarknetContext::get_train_networks()
    const
{
  return train_networks;
}

const DarknetContext::ANetworkAdaptorPtr& DarknetContext::get_map_network()
    const
{
  return map_network;
}

const DarknetContext::ANetworkAdaptorPtr& DarknetContext::get_detect_network()
    const
{
  return detect_network;
}

void DarknetContext::release_networks()
{
  assert(internal != nullptr);

  // the computation context must not refer to the released networks
  internal->nets = nullptr;
  std::memset(&internal->net, 0, sizeof(internal->net));
  std::memset(&internal->net_map, 0, sizeof(internal->net_map));

  for (const auto& net : train_networks) {
    adaptors::NetworksRegistry::remove(*net->get());
  }

  if (map_network) {
    adaptors::NetworksRegistry::remove(*map_network->get());
  }

  if (detect_network) {
    adaptors::NetworksRegistry::remove(*detect_network->get());
  }

  train_views.clear();
  train_networks.clear();
  map_network.reset();
  detect_network.reset();
}

}  // namespace darknet_adaptor
