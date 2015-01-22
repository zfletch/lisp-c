#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

enum InputType {
  FILE_TYPE, STRING_TYPE
};

union InputSource {
  FILE *file;
  const char *str;
};

struct Input {
  enum InputType type;
  union InputSource source;
};

// input creation and destruction
struct Input *create_file_input(FILE *file);
struct Input *create_string_input(const char *str);
void free_input(struct Input *input);

int input_get_char(struct Input *input);

#endif
