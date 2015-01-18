#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "./hash.h"

// internal helper functions
static size_t next_size_index(size_t size_index);
static size_t previous_size_index(size_t size_index);
static struct HashTable *create_hash_table_with_size(size_t size_index);

// the possible sizes for tha hash table
// they must all be prime numbers
static const size_t max_hash_size_index = 11;
static const size_t hash_sizes[max_hash_size_index] = {
  53,
  101,
  211,
  503,
  1553,
  3407,
  6803,
  12503,
  25013,
  50261,
  104729
};

struct HashTable *create_hash_table(void)
{
  return create_hash_table_with_size(0);
}

static struct HashTable *create_hash_table_with_size(size_t size_index)
{
  size_t size;
  struct HashTable *hash_table;

  size = hash_sizes[size_index];
  hash_table = malloc(sizeof(struct HashTable));

  hash_table->size_index = size_index;
  hash_table->entry_count = 0;
  hash_table->entries = calloc(size, sizeof(void *));

  return hash_table;
}

void free_hash_table(struct HashTable *hash_table)
{
  size_t size, ii;
  struct HashEntry *entry;

  size = hash_sizes[hash_table->size_index];

  for (ii = 0; ii < size; ii++) {
    if ((entry = hash_table->entries[ii])) free_entry(entry, true);
  }

  free(hash_table->entries);
  free(hash_table);
}

void hash_set(struct HashTable *hash_table, char *key, void *val)
{
  size_t size, hash;
  struct HashEntry *entry;

  size = hash_sizes[hash_table->size_index];
  hash = generate_hash(hash_table, key);
  entry = hash_table->entries[hash];

  while (entry) {
    if (strcmp(key, entry->key) == 0) {
      entry->val = val;
      return;
    }
    entry = entry->next;
  }

  entry = create_entry(key, val);

  entry->next = hash_table->entries[hash];
  hash_table->entries[hash] = entry;
  hash_table->entry_count++;

  if (hash_table->entry_count > size / 2) {
    hash_rehash(hash_table, next_size_index(hash_table->size_index));
  }
}

void *hash_get(struct HashTable *hash_table, char *key)
{
  size_t hash;
  struct HashEntry *entry;

  hash = generate_hash(hash_table, key);
  entry = hash_table->entries[hash];

  while (entry && strcmp(key, entry->key) != 0) entry = entry->next;

  return entry ? entry->val : NULL;
}

void *hash_delete(struct HashTable *hash_table, char *key)
{
  size_t size, hash;
  struct HashEntry *entry;
  void *val;

  size = hash_sizes[hash_table->size_index];
  hash = generate_hash(hash_table, key);
  entry = hash_table->entries[hash];

  if (entry && strcmp(key, entry->key) == 0) {
    hash_table->entries[hash] = entry->next;
  } else {
    while (entry) {
      if (entry->next && strcmp(key, entry->next->key) == 0) {
        entry->next = entry->next->next;
        entry = entry->next;
        break;
      }
      entry = entry->next;
    }
  }

  if (!entry) return NULL;

  val = entry->val;
  free_entry(entry, false);
  hash_table->entry_count--;

  if (hash_table->entry_count < size / 4) {
    hash_rehash(hash_table, previous_size_index(hash_table->size_index));
  }

  return val;
}

bool hash_exists(struct HashTable *hash_table, char *key)
{
  size_t hash;
  struct HashEntry *entry;

  hash = generate_hash(hash_table, key);
  entry = hash_table->entries[hash];

  while (entry && strcmp(key, entry->key) != 0) entry = entry->next;

  return entry ? true : false;
}

struct HashEntry *create_entry(char *key, void *val)
{
  struct HashEntry *entry;
  char *key_cpy;

  key_cpy = malloc(sizeof(key));
  entry = malloc(sizeof(struct HashEntry));

  strcpy(key_cpy, key);
  entry->key = key_cpy;
  entry->val = val;

  return entry;
}

void free_entry(struct HashEntry *entry, bool recursive)
{
  if (recursive && entry->next) free_entry(entry->next, recursive);
  free(entry->key);
  free(entry);
}

size_t generate_hash(struct HashTable *hash_table, char *key)
{
  size_t size;
  char ch;
  size_t hash;
  
  size = hash_sizes[hash_table->size_index];
  hash = 0;

  while ((ch = *key++)) hash = (17 * hash + ch) % size;

  return hash;
}

void hash_rehash(struct HashTable *hash_table, size_t size_index)
{
  size_t hash, old_size, size, ii;
  struct HashEntry *entry, *new_entry, **entries;

  if (size_index == hash_table->size_index) return;

  old_size = hash_sizes[hash_table->size_index];
  size = hash_sizes[size_index];
  entries = hash_table->entries;

  hash_table->size_index = size_index;
  hash_table->entries = calloc(size, sizeof(void *));

  for (ii = 0; ii < old_size; ii++) {
    if ((entry = entries[ii])) {
      while (entry) {
        new_entry = create_entry(entry->key, entry->val);
        hash = generate_hash(hash_table, new_entry->key);
        new_entry->next = hash_table->entries[hash];
        hash_table->entries[hash] = new_entry;

        new_entry = entry;
        entry = entry->next;
        free_entry(new_entry, false);
      }
    }
  }
  free(entries);
}

static size_t next_size_index(size_t size_index)
{
  if (size_index == max_hash_size_index) return size_index;

  return size_index + 1;
}

static size_t previous_size_index(size_t size_index)
{
  if (size_index == 0) return size_index;

  return size_index - 1;
}

static void printTest(struct HashTable *hash_table)
{
  size_t ii;
  struct HashEntry *entry;

  printf("Size: %zu\n", hash_sizes[hash_table->size_index]);
  printf("Entry Count: %zu\n", hash_table->entry_count);

  for (ii = 0; ii < hash_sizes[hash_table->size_index]; ii++) {
    if ((entry = hash_table->entries[ii])) {
      printf("%zu: ", ii);
      while(entry) {
        printf("%s -> %s ", entry->key, (char *) entry->val);
        entry = entry->next;
      }
      printf("\n");
    }
  }
}

// example usage
/* int main (int argc, char **argv) */
/* { */
/*   struct HashTable *hash_table; */
/*  */
/*   hash_table = create_hash_table(); */
/*  */
/*   hash_set(hash_table, "foo", "bar"); */
/*   hash_set(hash_table, "fizz", "buzz"); */
/*   hash_set(hash_table, "nope", "pope"); */
/*  */
/*   hash_set(hash_table, "foo1", "bar"); */
/*   hash_set(hash_table, "foo1", "bar"); */
/*   hash_set(hash_table, "foo1", "bar"); */
/*   hash_set(hash_table, "foo1", "bar"); */
/*   hash_set(hash_table, "foo1", "barp"); */
/*  */
/*   hash_set(hash_table, "foo15", "cat"); */
/*   hash_set(hash_table, "foo2", "bar"); */
/*   hash_set(hash_table, "foo3", "bar"); */
/*   hash_set(hash_table, "foo4", "bar"); */
/*   hash_set(hash_table, "foo5", "zar"); */
/*   hash_set(hash_table, "foo6", "bar"); */
/*   hash_set(hash_table, "foo7", "bar"); */
/*   hash_set(hash_table, "foo8", "bar"); */
/*   hash_set(hash_table, "foo9", "bar"); */
/*   hash_set(hash_table, "foo10", "bar"); */
/*   hash_set(hash_table, "foo11", "bar"); */
/*   hash_set(hash_table, "foo12", "bar"); */
/*   hash_set(hash_table, "foo13", "bar"); */
/*   hash_set(hash_table, "foo14", "bar"); */
/*   hash_set(hash_table, "foo15", "bar"); */
/*   hash_set(hash_table, "foo16", "bar"); */
/*   hash_set(hash_table, "foo17", "bar"); */
/*   hash_set(hash_table, "foo18", "bar"); */
/*   hash_set(hash_table, "foo19", "bar"); */
/*   hash_set(hash_table, "foo20", "bar"); */
/*   hash_set(hash_table, "foo21", "bar"); */
/*   hash_set(hash_table, "foo22", "bar"); */
/*   hash_set(hash_table, "foo23", "bar"); */
/*   hash_set(hash_table, "foo24", "bar"); */
/*   hash_set(hash_table, "foo25", "bar"); */
/*   hash_set(hash_table, "foo26", "bar"); */
/*   hash_set(hash_table, "foo27", "bar"); */
/*   hash_set(hash_table, "foo28", "bar"); */
/*   hash_set(hash_table, "foo29", NULL); */
/*   hash_set(hash_table, "foo30", "bar"); */
/*   hash_set(hash_table, "foo31", "bar"); */
/*   hash_set(hash_table, "foo32", "bar"); */
/*   hash_set(hash_table, "foo33", "bar"); */
/*   hash_set(hash_table, "foo34", "bar"); */
/*   hash_set(hash_table, "foo35", "bar"); */
/*   hash_set(hash_table, "foo36", "bar"); */
/*   hash_set(hash_table, "foo37", "bar"); */
/*   hash_set(hash_table, "foo38", "bar"); */
/*   hash_set(hash_table, "foo38", "bar"); */
/*  */
/*   printTest(hash_table); */
/*   printf("\n"); */
/*  */
/*   printf("%s\n", (char *) hash_get(hash_table, "foo38")); */
/*   printf("%s\n", (char *) hash_get(hash_table, "bubblebobble")); */
/*   printf("%s\n", (char *) hash_get(hash_table, "foo29")); */
/*  */
/*   printf("\n"); */
/*  */
/*   printf("%d\n", hash_exists(hash_table, "foo38")); */
/*   printf("%d\n", hash_exists(hash_table, "bubblebobble")); */
/*   printf("%d\n", hash_exists(hash_table, "foo29")); */
/*  */
/*   printf("\n"); */
/*  */
/*   printf("%s\n", (char *) hash_delete(hash_table, "foo1")); */
/*   printf("%s\n", (char *) hash_delete(hash_table, "bubblebobble")); */
/*  */
/*   printf("\n"); */
/*   printTest(hash_table); */
/*  */
/*   free_hash_table(hash_table); */
/* } */
