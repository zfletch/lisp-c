#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>
#include "./input.h"

enum TokenState {
  WHITESPACE, MIDWORD, MIDNUMBER
};

enum TokenType {
  OPEN_PAREN, CLOSE_PAREN, TOKEN_SYMBOL, TOKEN_INTEGER
};

struct Token {
  enum TokenType type;
  char *val;
  struct Token *next;
};

// token creation and destruction
struct Token *create_token(enum TokenType type, char *val);
void free_token(struct Token *token, bool recursive);

struct Token *scan(struct Input *input);

#endif
