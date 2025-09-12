#include "option_list.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "list.h"
#include "memory.h"
#include "src/clog/clog.h"
#include "utils.h"

list *read_data_cfg(char *filename)
{
  assert(filename != NULL);

  FILE *file = fopen(filename, "r");

  if (file == NULL) file_error(__FILE__, __LINE__, filename);

  char *line = NULL;
  int nu = 0;

  list *options = make_list();

  while ((line = fgetl(file)) != 0) {
    ++nu;
    strip(line);
    // printf("Parsing the line: %s\n", line);
    switch (line[0]) {
      case '\0':
      case '#':
      case ';':
        free(line);
        break;
      default:
        // printf("Reading the option: %s\n", line);
        if (!read_option(line, options)) {
          CLOGE("Config file error line %d, could parse: %s", nu, line);
        }
        break;
    }
  }

  CLOGI("Read %d options from %s", options->size, filename);

  fclose(file);

  return options;
}

metadata get_metadata(char *file)
{
  metadata m = {0};
  list *options = read_data_cfg(file);

  char *name_list = option_find_str(options, "names", 0);
  if (!name_list) name_list = option_find_str(options, "labels", 0);
  if (!name_list) {
    CLOGEM("No names or labels found");
  } else {
    m.names = get_labels(name_list);
  }
  m.classes = option_find_int(options, "classes", 2);
  free_list(options);
  if (name_list) {
    CLOGI("Loaded - names_list: %s, classes = %d", name_list, m.classes);
  }
  return m;
}

int read_option(char *s, list *options)
{
  size_t i;
  size_t len = strlen(s);
  char *val = 0;
  for (i = 0; i < len; ++i) {
    if (s[i] == '=') {
      s[i] = '\0';
      val = s + i + 1;
      break;
    }
  }
  if (i == len - 1) {
    CLOGT("No option in line: %s", s);
    return 0;
  }
  char *key = s;
  CLOGT("Inserting option %s = %s", key, val);
  option_insert(options, key, val);
  return 1;
}

void option_insert(list *l, char *key, char *val)
{
  kvp *p = (kvp *)xmalloc(sizeof(kvp));
  p->key = key;
  p->val = val;
  p->used = 0;
  list_insert(l, p);
}

void option_unused(list *l)
{
  node *n = l->front;
  while (n) {
    kvp *p = (kvp *)n->val;
    if (!p->used) {
      CLOGW("Unused field: '%s = %s'", p->key, p->val);
    }
    n = n->next;
  }
}

char *option_find(list *l, char *key)
{
  assert(l != NULL);
  assert(l->front != NULL);

  if (l == NULL) {
    CLOGEM("No list provided");
    return 0;
  }

  if (l->front == NULL) {
    CLOGEM("Invalid list provided");
    return 0;
  }

  node *n = l->front;
  while (n) {
    kvp *p = (kvp *)n->val;
    if (strcmp(p->key, key) == 0) {
      p->used = 1;
      return p->val;
    }
    n = n->next;
  }
  return 0;
}
char *option_find_str(list *l, char *key, char *def)
{
  char *v = option_find(l, key);
  if (v) return v;
  if (def) CLOGI("%s: Using default '%s'", key, def);
  return def;
}

char *option_find_str_quiet(list *l, char *key, char *def)
{
  char *v = option_find(l, key);
  if (v) return v;
  return def;
}

int option_find_int(list *l, char *key, int def)
{
  char *v = option_find(l, key);
  if (v) return atoi(v);
  CLOGI("%s: Using default '%d'", key, def);
  return def;
}

int option_find_int_quiet(list *l, char *key, int def)
{
  char *v = option_find(l, key);
  if (v) return atoi(v);
  return def;
}

float option_find_float_quiet(list *l, char *key, float def)
{
  char *v = option_find(l, key);
  if (v) return atof(v);
  return def;
}

float option_find_float(list *l, char *key, float def)
{
  char *v = option_find(l, key);
  if (v) return atof(v);
  CLOGI("%s: Using default '%lf'", key, def);
  return def;
}
