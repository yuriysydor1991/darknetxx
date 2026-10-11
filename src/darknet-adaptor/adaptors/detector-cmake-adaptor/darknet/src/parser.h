#ifndef PARSER_H
#define PARSER_H
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The network cfg files parser and the weights files of the original
 * darknet parser.c, implemented by the C++ network infrastructure now (the
 * src/darknet-adaptor/adaptors/NetworksCApi): the networks are made by it's
 * OrigDefaultLoader and their memory belongs to the C++ adaptors, the
 * weights files are read by it's OrigWeightsReader and written by it's
 * OrigWeightsWriter. The networks the parse_network_cfg*() make live until the
 * free_network() of their view.
 */

network parse_network_cfg(char *filename);
network parse_network_cfg_custom(char *filename, int batch, int time_steps);
void save_weights(network net, char *filename);
void save_weights_upto(network net, char *filename, int cutoff, int save_ema);
void load_weights(network *net, char *filename);
void load_weights_upto(network *net, char *filename, int cutoff);

#ifdef __cplusplus
}
#endif
#endif
