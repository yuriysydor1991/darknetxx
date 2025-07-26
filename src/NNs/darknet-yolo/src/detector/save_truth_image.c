#include "src/detector/save_truth_image.h"

#include <assert.h>

void save_truth_image_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);
  
  if (ctx != NULL) {
    return;
  }

  /*
  int k;
  for(k = 0; k < l.max_boxes; ++k){
  box b = float_to_box(train.y.vals[10] + 1 + k*5);
  if(!b.x) break;
  printf("loaded: %f %f %f %f\n", b.x, b.y, b.w, b.h);
  }
  image im = float_to_image(448, 448, 3, train.X.vals[10]);
  int k;
  for(k = 0; k < l.max_boxes; ++k){
  box b = float_to_box(train.y.vals[10] + 1 + k*5);
  printf("%d %d %d %d\n", truth.x, truth.y, truth.w, truth.h);
  draw_bbox(im, b, 8, 1,0,0);
  }
  save_image(im, "truth11");
  */
}