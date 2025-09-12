#ifndef DARKNET_DETECTOR_C_DECLARATIONS_H
#define DARKNET_DETECTOR_C_DECLARATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

void validate_detector(char *datacfg, char *cfgfile, char *weightfile,
                       char *outfile);
void validate_detector_recall(char *datacfg, char *cfgfile, char *weightfile);
void calc_anchors(char *datacfg, int num_of_clusters, int width, int height,
                  int show);
void draw_object(char *datacfg, char *cfgfile, char *weightfile, char *filename,
                 float thresh, int dont_show, int it_num, int letter_box,
                 int benchmark_layers);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_DETECTOR_C_DECLARATIONS_H