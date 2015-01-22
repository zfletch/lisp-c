#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "../src/scanner.h"

static void scan_file_test()
{
  FILE *file = fopen("./scanner_test", "r");
  struct Token *head, *tail;

  head = scan_file(file);

  tail = head;
  assert(tail->type == OPEN_PAREN);

  tail = tail->next;
  assert(tail->type == SYMBOL);
  assert(strcmp(tail->val.symbol, "+") == 0);

  tail = tail->next;
  assert(tail->type == INTEGER);
  assert(tail->val.integer == 1);

  tail = tail->next;
  assert(tail->type == OPEN_PAREN);

  tail = tail->next;
  assert(tail->type == SYMBOL);
  assert(strcmp(tail->val.symbol, "somefunc") == 0);

  tail = tail->next;
  assert(tail->type == SYMBOL);
  assert(strcmp(tail->val.symbol, "_a1") == 0);

  tail = tail->next;
  assert(tail->type == INTEGER);
  assert(tail->val.integer == 2);

  tail = tail->next;
  assert(tail->type == CLOSE_PAREN);

  tail = tail->next;
  assert(tail->type == CLOSE_PAREN);

  free_token(head, true);
  fclose(file);
}

static void scan_string_test()
{
  struct Token *head, *tail;

  head = scan_string("( (- a 55))");

  tail = head;
  assert(tail->type == OPEN_PAREN);

  tail = tail->next;
  assert(tail->type == OPEN_PAREN);

  tail = tail->next;
  assert(tail->type == SYMBOL);
  assert(strcmp(tail->val.symbol, "-") == 0);

  tail = tail->next;
  assert(tail->type == SYMBOL);
  assert(strcmp(tail->val.symbol, "a") == 0);

  tail = tail->next;
  assert(tail->type == INTEGER);
  assert(tail->val.integer == 55);

  tail = tail->next;
  assert(tail->type == CLOSE_PAREN);

  tail = tail->next;
  assert(tail->type == CLOSE_PAREN);

  free_token(head, true);
}

int main(int argc, char **argv)
{
  scan_file_test();
  scan_string_test();

  return 0;
}
