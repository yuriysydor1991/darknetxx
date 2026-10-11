#ifndef DARKNET_TREE_STRUCTURE_H
#define DARKNET_TREE_STRUCTURE_H

#ifdef __cplusplus
extern "C" {
#endif

// tree.h
typedef struct tree
{
  int *leaf;
  int n;
  int *parent;
  int *child;
  int *group;
  char **name;

  int groups;
  int *group_size;
  int *group_offset;
} tree;

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_CONTRASTIVE_PARAMS_H
