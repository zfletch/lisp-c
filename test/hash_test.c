#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "../src/hash.h"

// generate strings of random  ASCII characters 64 to 126
// between 5 and 20 characters
static char *random_string()
{
  size_t length, ii;
  char *str;

  length = rand() % 15 + 5;
  str = malloc((length + 1) * sizeof(char));

  for (ii = 0; ii < length; ii++) {
    str[ii] = rand() % 62 + 64;
  }
  str[length] = '\0';

  return str;
}

static void hash_set_test()
{
  size_t size, ii;
  char **keys, **vals;
  struct HashTable *hash_table;

  size = 100;
  hash_table = create_hash_table();
  keys = malloc(size * sizeof(char *));
  vals = malloc(size * sizeof(char *));

  for (ii = 0; ii < size; ii++) {
    keys[ii] = random_string();
    vals[ii] = random_string();
    hash_set(hash_table, keys[ii], (void *) vals[ii]);
  }

  assert(hash_table->size_index == 2);
  assert(hash_table->entry_count == size);

  for (ii = 0; ii < size; ii++) {
    assert(strcmp(hash_get(hash_table, keys[ii]), vals[ii]) == 0);
  }

  for (ii = 0; ii < size; ii++) {
    free(keys[ii]);
    free(vals[ii]);
  }

  free(keys);
  free(vals);
  free_hash_table(hash_table);
}

static void hash_delete_test()
{
  size_t size, ii;
  char **keys, **vals;
  struct HashTable *hash_table;

  size = 100;
  hash_table = create_hash_table();
  keys = malloc(size * sizeof(char *));
  vals = malloc(size * sizeof(char *));

  for (ii = 0; ii < size; ii++) {
    keys[ii] = random_string();
    vals[ii] = random_string();
    hash_set(hash_table, keys[ii], (void *) vals[ii]);
  }

  assert(hash_table->size_index == 2);
  assert(hash_table->entry_count == size);

  for (ii = 0; ii < 7 * size / 8; ii++) {
    hash_delete(hash_table, keys[ii]);
  }

  for (ii = 0; ii < size; ii++) {
    if (ii < 7 * size / 8) {
      assert(hash_get(hash_table, keys[ii]) == NULL);
    } else {
      assert(strcmp(hash_get(hash_table, keys[ii]), vals[ii]) == 0);
    }
  }

  assert(hash_table->size_index == 1);
  assert(hash_table->entry_count == size - 7 * size / 8);

  for (ii = 0; ii < size; ii++) {
    free(keys[ii]);
    free(vals[ii]);
  }

  free(keys);
  free(vals);
  free_hash_table(hash_table);
}

static void hash_exists_test()
{
  struct HashTable *hash_table;

  hash_table = create_hash_table();

  hash_set(hash_table, "hello", (void *) "world");
  hash_set(hash_table, "nothing", NULL);

  assert(hash_exists(hash_table, "hello") == true);
  assert(hash_exists(hash_table, "nothing") == true);
  assert(hash_get(hash_table, "nothing") == NULL);
  assert(hash_exists(hash_table, "nope") == false);

  free_hash_table(hash_table);
}

int main(int argc, char **argv)
{
  hash_set_test();
  hash_delete_test();
  hash_exists_test();

  return 0;
}
