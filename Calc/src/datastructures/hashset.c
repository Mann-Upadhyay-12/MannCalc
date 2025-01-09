#include "../../include/datastructures/hashset.h"
#include <stdlib.h>
#include <string.h>

unsigned int hashset_hash(const char* value) {
    unsigned int hash = 0;
    while (*value) {
        hash = (hash * 31) + *value;
        value++;
    }
    return hash;
}

hashset_t* hashset_create(void) {
    hashset_t* set = (hashset_t*)malloc(sizeof(hashset_t));
    if (!set) return NULL;
    
    set->capacity = HASHSET_INITIAL_SIZE;
    set->size = 0;
    set->table = (hashset_entry_t**)malloc(sizeof(hashset_entry_t*) * set->capacity);
    
    if (!set->table) {
        free(set);
        return NULL;
    }

    for (size_t i = 0; i < set->capacity; i++) {
        set->table[i] = NULL;
    }

    return set;
}

void hashset_destroy(hashset_t* set) {
    if (!set) return;

    for (size_t i = 0; i < set->capacity; i++) {
        hashset_entry_t* entry = set->table[i];
        while (entry) {
            hashset_entry_t* temp = entry;
            entry = entry->next;
            free(temp->value);
            free(temp);
        }
    }

    free(set->table);
    free(set);
}

int hashset_add(hashset_t* set, const char* value, const int input_spaces) {
    if (!set || !value) return 0;

    unsigned int index = hashset_hash(value) % set->capacity;
    hashset_entry_t* entry = set->table[index];

    while (entry) {
        if (strcmp(entry->value, value) == 0) {
            return 0;
        }
        entry = entry->next;
    }

    hashset_entry_t* new_entry = (hashset_entry_t*)malloc(sizeof(hashset_entry_t));
    if (!new_entry) return 0;

    new_entry->value = strdup(value);
    new_entry->input_spaces = input_spaces;
    if (!new_entry->value) {
        free(new_entry);
        return 0;
    }

    new_entry->next = set->table[index];
    set->table[index] = new_entry;
    set->size++;

    if (set->size > set->capacity * 0.7) {
        hashset_resize(set);
    }

    return 1;
}

void hashset_remove(hashset_t* set, const char* value) {
    if (!set || !value) return;

    unsigned int index = hashset_hash(value) % set->capacity;
    hashset_entry_t* entry = set->table[index];
    hashset_entry_t* prev = NULL;

    while (entry) {
        if (strcmp(entry->value, value) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                set->table[index] = entry->next;
            }

            free(entry->value);
            free(entry);
            set->size--;
            return;
        }

        prev = entry;
        entry = entry->next;
    }
}

int hashset_contains(hashset_t* set, const char* value) {
    if (!set || !value) return 0;

    unsigned int index = hashset_hash(value) % set->capacity;
    hashset_entry_t* entry = set->table[index];

    while (entry) {
        if (strcmp(entry->value, value) == 0) {
            return 1;
        }
        entry = entry->next;
    }

    return 0;
}

void hashset_resize(hashset_t* set) {
    if (!set) return;

    size_t new_capacity = set->capacity * 2;
    hashset_entry_t** new_table = (hashset_entry_t**)malloc(sizeof(hashset_entry_t*) * new_capacity);
    if (!new_table) return;

    for (size_t i = 0; i < new_capacity; i++) {
        new_table[i] = NULL;
    }

    for (size_t i = 0; i < set->capacity; i++) {
        hashset_entry_t* entry = set->table[i];
        while (entry) {
            unsigned int new_index = hashset_hash(entry->value) % new_capacity;

            hashset_entry_t* new_entry = (hashset_entry_t*)malloc(sizeof(hashset_entry_t));
            if (!new_entry) return;

            new_entry->value = entry->value;
            new_entry->next = new_table[new_index];
            new_table[new_index] = new_entry;

            entry = entry->next;
        }
    }

    free(set->table);
    set->table = new_table;
    set->capacity = new_capacity;
}

int hashset_get_value(hashset_t* set, const char* key) {
    if (!set || !key) return -1;

    unsigned int index = hashset_hash(key) % set->capacity;
    hashset_entry_t* entry = set->table[index];

    while (entry) {
        if (strcmp(entry->value, key) == 0) {
            return entry->input_spaces;
        }
        entry = entry->next;
    }

    return -1;
}

hashset_entry_t* hashset_get_entry(hashset_t* set, const char* key) {
    if (!set || !key) return NULL;

    unsigned int index = hashset_hash(key) % set->capacity;
    hashset_entry_t* entry = set->table[index];

    while (entry) {
        if (strcmp(entry->value, key) == 0) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}
