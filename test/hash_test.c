#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

static void hash_set_test ()
{
  char *key, *val;
  struct HashTable *hash_table;

  hash_table = create_hash_table();
  key = random_string();
  val = random_string();

  hash_set(hash_table, key, (void *) val);

  assert(strcmp((char *) hash_get(hash_table, key), val) == 0);

  free(key); free(val);
  free_hash_table(hash_table);
}

int main (int argc, char **argv)
{
  hash_set_test();

  return 0;
}
