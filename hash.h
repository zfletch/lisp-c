#include "stdbool.h"

// hash table implementation
// keys must be strings
// values are void *pointers

// struct representing an entry in the hash table
// should probably only be used internally
struct HashEntry {
  char *key;
  void *val;
  struct HashEntry *next;
};

// struct representing the hash table
// the size_index is an index into the hash_sizes
// array defined in hash.c
struct HashTable {
  size_t size_index;
  size_t entry_count;
  struct HashEntry **entries;
};

// hash table functions
struct HashTable *create_hash_table(void);
void free_hash_table(struct HashTable *hash_table);

// hash operations
void hash_set(struct HashTable *hash_table, char *key, void *val);
void *hash_get(struct HashTable *hash_table, char *key);
void *hash_delete(struct HashTable *hash_table, char *key);
bool hash_exists(struct HashTable *hash_table, char *key);

// hash entry functions
struct HashEntry *create_entry(char *key, void *val);
void free_entry(struct HashEntry *entry, bool recursive);

// other functions
size_t generate_hash(struct HashTable *hash, char *key);
void hash_rehash(struct HashTable *hash_table, size_t size_index);
