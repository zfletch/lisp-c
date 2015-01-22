#include <stdio.h>
#include <stdlib.h>
#include "./utils.h"
#include "./input.h"

struct Input *create_file_input(FILE *file)
{
  struct Input *input;
  union InputSource source;

  input = smalloc(sizeof(struct Input));

  source.file = file;
  input->type = FILE_TYPE;
  input->source = source;

  return input;
}

struct Input *create_string_input(const char *str)
{
  struct Input *input;
  union InputSource source;

  input = smalloc(sizeof(struct Input));

  source.str = str;
  input->type = STRING_TYPE;
  input->source = source;

  return input;
}

void free_input(struct Input *input)
{
  free(input);
}

int input_get_char(struct Input *input)
{
  union InputSource source;
  char ch;

  switch (input->type) {
    case FILE_TYPE:
      return getc(input->source.file);
    default:
      ch = *input->source.str;
      source.str = input->source.str + 1;
      input->source = source;

      return ch ? (int) ch : EOF;
  }
}
