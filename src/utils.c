#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "./utils.h"

void *smalloc(size_t size)
{
  void *result;

  result = malloc(size);

  if (!result) error(strerror(errno));

  return result;
}

void *scalloc(size_t num, size_t size)
{
  void *result;

  result = calloc(num, size);

  if (!result) error(strerror(errno));

  return result;
}

void error(const char *message)
{
  fprintf(stderr, "%s\n", message);
  exit(-1);
}
