#include "string_tools.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef size_to_IEC_string_BUFF_SIZE
#define size_to_IEC_string_BUFF_SIZE 128U
#endif  // size_to_IEC_string_BUFF_SIZE

const char *size_to_IEC_string(const size_t size)
{
  const double bytes = (double)size;
  const double KiB = 1024;
  const double MiB = 1024 * KiB;
  const double GiB = 1024 * MiB;

  static char buffer[size_to_IEC_string_BUFF_SIZE];

  memset(&buffer, 0, size_to_IEC_string_BUFF_SIZE);

  if (size < KiB) sprintf(buffer, "%ld bytes", size);
  else if (size < MiB)
    sprintf(buffer, "%1.1f KiB", bytes / KiB);
  else if (size < GiB)
    sprintf(buffer, "%1.1f MiB", bytes / MiB);
  else
    sprintf(buffer, "%1.1f GiB", bytes / GiB);

  return buffer;
}
