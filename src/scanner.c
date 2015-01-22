#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./scanner.h"
#include "./utils.h"

enum InputType {
  FILE_TYPE, STRING_TYPE
};

union InputSource {
  FILE *file;
  char *str;
};

struct Input {
  enum InputType type;
  union InputSource source;
};

// helper functions
static int next_char(struct Input *input);
static struct Token *scan(struct Input input);
static struct Token *open_paren(void);
static struct Token *close_paren(void);
static struct Token *symbol(char *str, size_t char_count);
static struct Token *integer(long int integer);
static void add_to_str(char **str, size_t *size, char ch, size_t char_count);

static int next_char(struct Input *input)
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

struct Token *scan_file(FILE *file)
{
  struct Input input;
  union InputSource source;

  input.type = FILE_TYPE;
  source.file = file;
  input.source = source;

  return scan(input);
}

struct Token *scan_string(char *str)
{
  struct Input input;
  union InputSource source;

  input.type = STRING_TYPE;
  source.str = str;
  input.source = source;

  return scan(input);
}

static struct Token *scan(struct Input input)
{
  int ch;
  char *str;
  size_t size, char_count;
  struct Token *head, *tail;
  enum TokenState state;

  // make head point to a bogus token and return head->next
  // to avoid if (head == NULL) { head = token; } in the loop
  head = close_paren();
  tail = head;

  state = WHITESPACE;
  char_count = 0;
  size = 200;
  str = smalloc(size);

  while ((ch = next_char(&input)) != EOF) {
    switch (state) {
      case WHITESPACE:
        switch (ch) {
          case '(':
            tail->next = open_paren();
            tail = tail->next;
            break;
          case ')':
            tail->next = close_paren();
            tail = tail->next;
            break;
          case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
            break;
          case '0': case '1': case '2': case '3': case '4': case '5':
          case '6': case '7': case '8': case '9':
            add_to_str(&str, &size, (char) ch, char_count = 1);
            state = NUMBER;
            break;
          default:
            add_to_str(&str, &size, (char) ch, char_count = 1);
            state = MIDWORD;
            break;
        }
        break;
      case NUMBER:
        switch (ch) {
          case '0': case '1': case '2': case '3': case '4': case '5':
          case '6': case '7': case '8': case '9':
            add_to_str(&str, &size, (char) ch, ++char_count);
            break;
          case '(':
            str[char_count] = '\0';
            tail->next = integer(atol(str));
            tail->next->next = open_paren();
            tail = tail->next->next;
            state = WHITESPACE;
            break;
          case ')':
            str[char_count] = '\0';
            tail->next = integer(atol(str));
            tail->next->next = close_paren();
            tail = tail->next->next;
            state = WHITESPACE;
            break;
          case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
            str[char_count] = '\0';
            tail->next = integer(atol(str));
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
            tail->next = symbol(str, char_count);
            tail->next->next = open_paren();
            tail = tail->next->next;
            state = WHITESPACE;
            break;
          case ')':
            tail->next = symbol(str, char_count);
            tail->next->next = close_paren();
            tail = tail->next->next;
            state = WHITESPACE;
            break;
          case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
            tail->next = symbol(str, char_count);
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

  if (state == MIDWORD) tail = (tail->next = symbol(str, char_count));

  free(str);

  tail = head->next;
  free_token(head, false);

  return tail;
}

struct Token *create_token(enum TokenType type, union TokenVal val)
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
    case SYMBOL:
      free(token->val.symbol);
      break;
    default: break;
  }

  free(token);
}

static struct Token *open_paren()
{
  union TokenVal val;

  return create_token(OPEN_PAREN, val);
}

static struct Token *close_paren()
{
  union TokenVal val;

  return create_token(CLOSE_PAREN, val);
}

struct Token *symbol(char *str, size_t char_count)
{
  union TokenVal val;
  char *symbol;

  symbol = smalloc((char_count + 1) * sizeof(char));

  memcpy(symbol, str, char_count);
  symbol[char_count] = '\0';
  val.symbol = symbol;

  return create_token(SYMBOL, val);
}

static struct Token *integer(long int integer)
{
  union TokenVal val;

  val.integer = integer;

  return create_token(INTEGER, val);
}

static void add_to_str(char **str, size_t *size, char ch, size_t char_count)
{
  (*str)[char_count - 1] = (char) ch;

  if (char_count >= *size) {
    *size *= 2;
    *str = srealloc(*str, *size);
  }
}
