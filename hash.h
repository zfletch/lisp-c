#include "stdbool.h"

struct HashEntry {
  char *key;
  void *val;
  struct HashEntry *next;
};

struct HashTable {
  size_t size_index;
  size_t entry_count;
  struct HashEntry **entries;
};

struct HashEntry *create_entry(char *key, void *val);
void free_entry(struct HashEntry *entry, bool recursive);
struct HashTable *create_hash_table(void);
struct HashTable *create_hash_table_with_size(size_t size_index);
void *hash_delete(struct HashTable *hash_table, char *key);
void free_hash_table(struct HashTable *hash_table);
size_t generate_hash(struct HashTable *hash, char *key);
void *hash_get(struct HashTable *hash_table, char *key);
void hash_set(struct HashTable *hash_table, char *key, void *val);
void hash_rehash(struct HashTable *hash_table, size_t size_index);
