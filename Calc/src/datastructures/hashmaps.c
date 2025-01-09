#include "../../include/datastructures/hashmaps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int hashmap_hash(const char* key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash;
}

hashmap_t* hashmap_create(void) {
    hashmap_t* hashmap = (hashmap_t*)malloc(sizeof(hashmap_t));
    if (!hashmap) {
        return NULL;
    }

    hashmap->capacity = HASHMAP_INITIAL_SIZE;
    hashmap->size = 0;
    hashmap->table = (hashmap_entry_t**)calloc(hashmap->capacity, sizeof(hashmap_entry_t*));

    if (!hashmap->table) {
        free(hashmap);
        return NULL;
    }

    return hashmap;
}

void hashmap_destroy(hashmap_t* hashmap) {
    if (!hashmap) return;

    for (size_t i = 0; i < hashmap->capacity; i++) {
        hashmap_entry_t* entry = hashmap->table[i];
        while (entry) {
            hashmap_entry_t* temp = entry;
            entry = entry->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }

    free(hashmap->table);
    free(hashmap);
}

void hashmap_put(hashmap_t* hashmap, const char* key, const char* value) {
    unsigned int hash = hashmap_hash(key) % hashmap->capacity;
    hashmap_entry_t* entry = hashmap->table[hash];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            free(entry->value);
            entry->value = strdup(value);
            return;
        }
        entry = entry->next;
    }

    hashmap_entry_t* new_entry = (hashmap_entry_t*)malloc(sizeof(hashmap_entry_t));
    new_entry->key = strdup(key);
    new_entry->value = strdup(value);
    new_entry->next = hashmap->table[hash];
    hashmap->table[hash] = new_entry;
    hashmap->size++;

    if (hashmap->size > hashmap->capacity * 0.7) {
        hashmap_resize(hashmap);
    }
}

char* hashmap_get(hashmap_t* hashmap, const char* key) {
    unsigned int hash = hashmap_hash(key) % hashmap->capacity;
    hashmap_entry_t* entry = hashmap->table[hash];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void hashmap_remove(hashmap_t* hashmap, const char* key) {
    unsigned int hash = hashmap_hash(key) % hashmap->capacity;
    hashmap_entry_t* entry = hashmap->table[hash];
    hashmap_entry_t* prev = NULL;

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                hashmap->table[hash] = entry->next;
            }
            free(entry->key);
            free(entry->value);
            free(entry);
            hashmap->size--;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

int hashmap_contains_key(hashmap_t* hashmap, const char* key) {
    unsigned int hash = hashmap_hash(key) % hashmap->capacity;
    hashmap_entry_t* entry = hashmap->table[hash];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

void hashmap_resize(hashmap_t* hashmap) {
    size_t old_capacity = hashmap->capacity;
    hashmap_entry_t** old_table = hashmap->table;

    hashmap->capacity *= 2;
    hashmap->table = (hashmap_entry_t**)calloc(hashmap->capacity, sizeof(hashmap_entry_t*));

    if (!hashmap->table) {
        return;
    }

    for (size_t i = 0; i < old_capacity; i++) {
        hashmap_entry_t* entry = old_table[i];
        while (entry) {
            hashmap_entry_t* next = entry->next;
            unsigned int hash = hashmap_hash(entry->key) % hashmap->capacity;
            entry->next = hashmap->table[hash];
            hashmap->table[hash] = entry;
            entry = next;
        }
    }

    free(old_table);
}
