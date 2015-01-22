#include <string.h>
#include <stdlib.h>
#include "./scanner.h"
#include "./utils.h"
#include "./input.h"

// helper functions
static char *make_str(char *str, size_t char_count);
static void add_to_str(char **str, size_t *size, char ch, size_t char_count);

struct Token *create_token(enum TokenType type, char *val)
{
  struct Token *token;

  token = smalloc(sizeof(struct Token));

  token->type = type;
  token->val = val;
  token->next = NULL;

  return token;
}

void free_token(struct Token *token, bool recursive)
{
  if (recursive && token->next) free_token(token->next, recursive);

  switch (token->type) {
    case TOKEN_SYMBOL: case TOKEN_INTEGER:
      free(token->val);
      break;
    default: break;
  }

  free(token);
}

struct Token *scan(struct Input *input)
{
  int ch;
  char *str;
  size_t size, char_count;
  struct Token *head, *tail;
  enum TokenState state;

  // make head point to a bogus token and return head->next
  // to avoid if (head == NULL) { head = token; } in the loop
  head = create_token(CLOSE_PAREN, NULL);
  tail = head;

  state = WHITESPACE;
  char_count = 0;
  size = 200;
  str = smalloc(size);

  while ((ch = input_get_char(input)) != EOF) {
    switch (state) {
      case WHITESPACE:
        switch (ch) {
          case '(':
            tail->next = create_token(OPEN_PAREN, NULL);
            tail = tail->next;
            break;
          case ')':
            tail->next = create_token(CLOSE_PAREN, NULL);
            tail = tail->next;
            break;
          case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
            break;
          case '0': case '1': case '2': case '3': case '4': case '5':
          case '6': case '7': case '8': case '9':
            add_to_str(&str, &size, (char) ch, char_count = 1);
            state = MIDNUMBER;
            break;
          default:
            add_to_str(&str, &size, (char) ch, char_count = 1);
            state = MIDWORD;
            break;
        }
        break;
      case MIDNUMBER:
        switch (ch) {
          case '0': case '1': case '2': case '3': case '4': case '5':
          case '6': case '7': case '8': case '9':
            add_to_str(&str, &size, (char) ch, ++char_count);
            break;
          case '(':
            tail->next = create_token(TOKEN_INTEGER, make_str(str, char_count));
            tail->next->next = create_token(OPEN_PAREN, NULL);
            tail = tail->next->next;
            state = WHITESPACE;
            break;
          case ')':
            tail->next = create_token(TOKEN_INTEGER, make_str(str, char_count));
            tail->next->next = create_token(CLOSE_PAREN, NULL);
            tail = tail->next->next;
            state = WHITESPACE;
            break;
          case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
            tail->next = create_token(TOKEN_INTEGER, make_str(str, char_count));
            tail = tail->next;
            state = WHITESPACE;
            break;
          default:
            add_to_str(&str, &size, (char) ch, ++char_count);
            state = MIDWORD;
            break;
        }
        break;
      case MIDWORD:
        switch (ch) {
          case '(':
            tail->next = create_token(TOKEN_SYMBOL, make_str(str, char_count));
            tail->next->next = create_token(OPEN_PAREN, NULL);
            tail = tail->next->next;
            state = WHITESPACE;
            break;
          case ')':
            tail->next = create_token(TOKEN_SYMBOL, make_str(str, char_count));
            tail->next->next = create_token(CLOSE_PAREN, NULL);
            tail = tail->next->next;
            state = WHITESPACE;
            break;
          case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
            tail->next = create_token(TOKEN_SYMBOL, make_str(str, char_count));
            tail = tail->next;
            state = WHITESPACE;
            break;
          default:
            add_to_str(&str, &size, (char) ch, ++char_count);
            break;
        }
        break;
    }
  }

  if (state == MIDWORD) tail->next = create_token(TOKEN_SYMBOL, make_str(str, char_count));

  free(str);

  tail = head->next;
  free_token(head, false);

  return tail;
}

static char *make_str(char *str, size_t char_count)
{
  char *str_cpy;

  str_cpy = smalloc((char_count + 1) * sizeof(char));

  memcpy(str_cpy, str, char_count);
  str_cpy[char_count] = '\0';

  return str_cpy;
}

static void add_to_str(char **str, size_t *size, char ch, size_t char_count)
{
  (*str)[char_count - 1] = (char) ch;

  if (char_count >= *size) {
    *size *= 2;
    *str = srealloc(*str, *size);
  }
}
