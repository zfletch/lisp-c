#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "../src/input.h"
#include "../src/scanner.h"

static void scan_file_test()
{
  FILE *file;
  struct Input *input;
  struct Token *head, *tail;

  file = fopen("./scanner_test", "r");
  input = create_file_input(file);
  head = scan(input);

  tail = head;
  assert(tail->type == OPEN_PAREN);

  tail = tail->next;
  assert(tail->type == TOKEN_SYMBOL);
  assert(strcmp(tail->val, "+") == 0);

  tail = tail->next;
  assert(tail->type == TOKEN_INTEGER);
  assert(strcmp(tail->val, "1") == 0);

  tail = tail->next;
  assert(tail->type == OPEN_PAREN);

  tail = tail->next;
  assert(tail->type == TOKEN_SYMBOL);
  assert(strcmp(tail->val, "somefunc") == 0);

  tail = tail->next;
  assert(tail->type == TOKEN_SYMBOL);
  assert(strcmp(tail->val, "_a1") == 0);

  tail = tail->next;
  assert(tail->type == TOKEN_INTEGER);
  assert(strcmp(tail->val, "2") == 0);

  tail = tail->next;
  assert(tail->type == CLOSE_PAREN);

  tail = tail->next;
  assert(tail->type == CLOSE_PAREN);

  free_token(head, true);
  free_input(input);
  fclose(file);
}

static void scan_string_test()
{
  struct Input *input;
  struct Token *head, *tail;

  input = create_string_input("( (- a 55))");
  head = scan(input);

  tail = head;
  assert(tail->type == OPEN_PAREN);

  tail = tail->next;
  assert(tail->type == OPEN_PAREN);

  tail = tail->next;
  assert(tail->type == TOKEN_SYMBOL);
  assert(strcmp(tail->val, "-") == 0);

  tail = tail->next;
  assert(tail->type == TOKEN_SYMBOL);
  assert(strcmp(tail->val, "a") == 0);

  tail = tail->next;
  assert(tail->type == TOKEN_INTEGER);
  assert(strcmp(tail->val, "55") == 0);

  tail = tail->next;
  assert(tail->type == CLOSE_PAREN);

  tail = tail->next;
  assert(tail->type == CLOSE_PAREN);

  free_token(head, true);
  free_input(input);
}

int main(int argc, char **argv)
{
  scan_file_test();
  scan_string_test();

  return 0;
}
