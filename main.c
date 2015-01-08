#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// .h file
struct Token {
  char *str;
  size_t len;
  struct Token *next;
};
enum TokenState {
  WHITESPACE, MIDWORD
};
enum ASTNodeType {
  INT, EXPR, OP
};
enum OptType {
  PLUS, MINUS
};
struct ASTNode {
  enum ASTNodeType type;
  struct ASTNode *next;
  void *val;
};
struct ASTNodeAndToken {
  struct ASTNode *node;
  struct Token *token;
};
void free_ast(struct ASTNode* ast);
struct ASTNode *eval_op(enum OptType type, struct ASTNode *arg1, struct ASTNode *arg2);
struct ASTNode *eval_ast(struct ASTNode* ast);
struct ASTNode *eval_expr(struct ASTNode* ast);
struct ASTNodeAndToken build_ast(struct Token *token);
struct Token *new_token(char *str, size_t len);
void free_token(struct Token *token);
struct Token *tokenize (char *string);

// .c file
int main(int argc, char **argv)
{
  struct Token *token;
  struct Token *next;
  struct ASTNodeAndToken ast_and_t;

  next = token = tokenize(argv[1]);
  /* while ((next = next->next)) printf("%s\n", next->str); */
  ast_and_t = build_ast(token);

  printf("%d\n", (int) eval_ast(ast_and_t.node)->val);

  return 0;
}

void free_ast(struct ASTNode* ast)
{
  if (ast->next) free_ast(ast->next);
  switch (ast->type) {
    case INT: case OP:
      break;
    case EXPR:
      free_ast((struct ASTNode *) ast->val);
      break;
    default:
      break;
  }
  free(ast);
}

struct ASTNode *eval_op(enum OptType type, struct ASTNode *arg1, struct ASTNode *arg2)
{
  struct ASTNode *ast;

  ast = malloc(sizeof(struct ASTNode));
  ast->next = NULL;
  ast->type = INT;

  switch (type) {
    case PLUS:
      ast->val = (void *) ((int) arg1->val + (int) arg2->val);
      break;
    case MINUS:
      ast->val = (void *) ((int) arg1->val - (int) arg2->val);
      break;
  }

  return ast;
}

struct ASTNode *eval_ast(struct ASTNode* ast)
{
  switch (ast->type) {
    case INT:
      return ast;
    case EXPR:
      return eval_expr((struct ASTNode *) ast->val);
    default:
      return NULL;
  }
}

struct ASTNode *eval_expr(struct ASTNode* ast)
{
  switch (ast->type) {
    case OP:
      return eval_op((enum OptType) ast->val,
          eval_ast(ast->next),
          eval_ast(ast->next->next));
    default:
      return NULL;
  }
}

struct ASTNodeAndToken build_ast(struct Token *token)
{
  struct ASTNode *head, *tail, *node;
  struct ASTNodeAndToken ast_and_t;

  head = tail = node = NULL;
  while ((token = token->next)) {
    if (strcmp(token->str, "(") == 0) {
      node = malloc(sizeof(struct ASTNode));
      node->type = EXPR;
      ast_and_t = build_ast(token);
      node->val = (void *) ast_and_t.node;
      token = ast_and_t.token;
    } else if (strcmp(token->str, ")") == 0) {
      ast_and_t.node = head;
      ast_and_t.token = token;
      return ast_and_t;
    } else if (strcmp(token->str, "+") == 0) {
      node = malloc(sizeof(struct ASTNode));
      node->type = OP;
      node->next = NULL;
      node->val = (void *) PLUS;
    } else if (strcmp(token->str, "-") == 0) {
      node = malloc(sizeof(struct ASTNode));
      node->type = OP;
      node->next = NULL;
      node->val = (void *) MINUS;
    } else {
      node = malloc(sizeof(struct ASTNode));
      node->type = INT;
      node->next = NULL;
      node->val = (void *) atoi(token->str);
    }

    if (head == NULL) {
      head = tail = node;
    } else {
      tail = (tail->next = node);
    }
  }

  ast_and_t.node = head;
  ast_and_t.token = token;
  return ast_and_t;
}

struct Token *new_token(char *str, size_t len)
{
  struct Token *token;

  token = malloc(sizeof(struct Token));
  token->str = str;
  token->len = len;
  token->next = NULL;

  return token;
}

void free_token(struct Token *token)
{
  struct Token *next;

  while (token) {
    next = token->next;
    free(token->str);
    free(token);
    token = next;
  }
}

struct Token *tokenize (char *string)
{
  size_t start, end;
  char ch;
  char *str;
  struct Token *head, *tail;
  enum TokenState state;

  start = end = 0;
  head = tail = new_token("", 0);
  state = WHITESPACE;
  while ((ch = *string++)) {
    switch (state) {
      case WHITESPACE:
        switch (ch) {
          case '(':
            tail = (tail->next = new_token("(", 1));
            break;
          case ')':
            tail = (tail->next = new_token(")", 1));
            break;
          case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
            break;
          default:
            state = MIDWORD;
            start = end;
            break;
        }
        break;
      case MIDWORD:
        switch (ch) {
          case '(':
            str = malloc((end - start + 1) * sizeof(char));
            memcpy(str, string - (end - start + 1), end - start);
            str[end - start] = '\0';
            tail = (tail->next = new_token(str, end - start));
            tail = (tail->next = new_token("(", 1));
            state = WHITESPACE;
            break;
          case ')':
            str = malloc((end - start + 1) * sizeof(char));
            memcpy(str, string - (end - start + 1), end - start);
            str[end - start] = '\0';
            tail = (tail->next = new_token(str, end - start));
            tail = (tail->next = new_token(")", 1));
            state = WHITESPACE;
            break;
          case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
            str = malloc((end - start + 1) * sizeof(char));
            memcpy(str, string - (end - start + 1), end - start);
            str[end - start] = '\0';
            tail = (tail->next = new_token(str, end - start));
            state = WHITESPACE;
            break;
          default:
            break;
        }
        break;
    }

    end += 1;
  }

  if (state == MIDWORD) {
    str = malloc((end - start + 1) * sizeof(char));
    memcpy(str, string - (end - start + 1), end - start);
    str[end - start] = '\0';
    tail = (tail->next = new_token(str, end - start));
  }

  return head;
}
