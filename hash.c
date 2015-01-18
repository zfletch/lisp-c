#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "./hash.h"

// helper functions
static size_t next_size_index(size_t size_index);
static size_t previous_size_index(size_t size_index);
static struct HashTable *create_hash_table_with_size(size_t size_index);

// possible sizes for hash table; must be prime numbers
static const size_t max_hash_size_index = 13;
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
  104729,
  250007,
  500009
};

struct HashTable *create_hash_table(void)
{
  return create_hash_table_with_size(0);
}

static struct HashTable *create_hash_table_with_size(size_t size_index)
{
  struct HashTable *hash_table;

  hash_table = malloc(sizeof(struct HashTable));

  hash_table->size_index = size_index;
  hash_table->entry_count = 0;
  hash_table->entries = calloc(hash_sizes[size_index], sizeof(void *));

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
  size_t size, hash;
  char ch;
  
  size = hash_sizes[hash_table->size_index];
  hash = 0;

  while ((ch = *key++)) hash = (17 * hash + ch) % size;

  return hash;
}

void hash_rehash(struct HashTable *hash_table, size_t size_index)
{
  size_t hash, old_size, ii;
  struct HashEntry *entry, *new_entry, **entries;

  if (size_index == hash_table->size_index) return;

  old_size = hash_sizes[hash_table->size_index];
  entries = hash_table->entries;

  hash_table->size_index = size_index;
  hash_table->entries = calloc(hash_sizes[size_index], sizeof(void *));

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
