#include <stdbool.h>

enum TokenState {
  WHITESPACE, MIDWORD, NUMBER
};

enum TokenType {
  OPEN_PAREN, CLOSE_PAREN, SYMBOL, INTEGER
};

union TokenVal {
  char *symbol;
  long int integer;
};

struct Token {
  enum TokenType type;
  union TokenVal val;
  struct Token *next;
};

// token creation and destruction
struct Token *create_token(enum TokenType type, union TokenVal val);
void free_token(struct Token *token, bool recursive);
struct Token *scan(FILE *file);
