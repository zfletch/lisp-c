#include "string.h"
#include "stdlib.h"
#include "./hash.h"

static size_t next_size_index(size_t size_index);
static size_t previous_size_index(size_t size_index);

// the possible sizes for tha hash table
// they must all be prime numbers
static const size_t max_hash_size_index = 10;
static const size_t hash_sizes[max_hash_size_index] = {
  53,
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

void free_entry(struct HashEntry *entry)
{
  if (entry->next) free_entry(entry->next);
  free(entry->val);
  free(entry);
}

struct HashTable *create_hash_table(size_t size_index)
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

size_t generate_hash(struct HashTable *hash_table, char *key)
{
  size_t size;
  char ch;
  size_t hash;
  
  size = hash_sizes[hash_table->size_index];
  hash = 0;

  while ((ch = *key++)) hash = (hash + ch) % size;

  return hash;
}

void *hash_get(struct HashTable *hash_table, char *key)
{
  size_t hash;
  struct HashEntry *entry;

  hash = generate_hash(hash_table, key);
  entry = hash_table->entries[hash];

  while (entry && strcmp(key, entry->key) != 0) entry = entry->next;

  return entry;
}

void hash_set(struct HashTable *hash_table, char *key, void *val)
{
  size_t size, hash;
  struct HashEntry *entry;

  size = hash_sizes[hash_table->size_index];
  hash = generate_hash(hash_table, key);
  entry = create_entry(key, val);

  entry->next = hash_table->entries[hash];
  hash_table->entries[hash] = entry;
  hash_table->entry_count++;

  if (hash_table->entry_count > size / 2) {
    hash_rehash(hash_table, next_size_index(hash_table->size_index));
  }
  if (hash_table->entry_count < size / 4) {
    hash_rehash(hash_table, previous_size_index(hash_table->size_index));
  }
}

void hash_rehash(struct HashTable *hash_table, size_t size_index)
{
  size_t old_size, ii;
  struct HashEntry *entry;
  struct HashTable *new_hash_table;

  if (size_index == hash_table->size_index) return;

  old_size = hash_sizes[hash_table->size_index];
  new_hash_table = create_hash_table(size_index);

  for (ii = 0; ii < old_size; ii++) {
    if ((entry = hash_table->entries[ii])) {
      hash_set(new_hash_table, entry->key, entry->val);
      free(entry);
    }
  }
  free(hash_table->entries);

  hash_table->size_index = new_hash_table->size_index;
  hash_table->entry_count = new_hash_table->entry_count;
  hash_table->entries = new_hash_table->entries;

  free(new_hash_table);
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
