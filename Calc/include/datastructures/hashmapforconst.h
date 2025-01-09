#ifndef HASHMAPFORCONST_H
#define HASHMAPFORCONST_H

#include <stdlib.h>
#include <string.h>

#define HASHMAPCONST_INITIAL_SIZE 16

// Structure for hash map entries
typedef struct hashmapconst_entry {
    char* name;
    double input_value;
    struct hashmapconst_entry* next;
} hashmapconst_entry_t;

// Structure for the hash map
typedef struct {
    hashmapconst_entry_t** table;
    size_t size;
    size_t capacity;
} hashmapconst_t;

// Function declarations
unsigned int hashmapconst_hash(const char* value);
hashmapconst_t* hashmapconst_create(void);
void hashmapconst_destroy(hashmapconst_t* map);
int hashmapconst_add(hashmapconst_t* map, const char* value, const float input_value);
void hashmapconst_remove(hashmapconst_t* map, const char* value);
int hashmapconst_contains(hashmapconst_t* map, const char* value);
void hashmapconst_resize(hashmapconst_t* map);
float hashmapconst_get_value(hashmapconst_t* map, const char* key);
hashmapconst_entry_t* hashmapconst_get_entry(hashmapconst_t* map, const char* key) ;
int hashmapconst_update(hashmapconst_t* map, const char* value, const float input_value);

#endif // HASHMAPFORCONST_H
