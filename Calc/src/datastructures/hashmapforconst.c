#include "../../include/datastructures/hashmapforconst.h"
#include <stdlib.h>
#include <string.h>

unsigned int hashmapconst_hash(const char* name) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash * 31) + *name;
        name++;
    }
    return hash;
}

hashmapconst_t* hashmapconst_create(void) {
    hashmapconst_t* map = (hashmapconst_t*)malloc(sizeof(hashmapconst_t));
    if (!map) return NULL;
    
    map->capacity = HASHMAPCONST_INITIAL_SIZE;
    map->size = 0;
    map->table = (hashmapconst_entry_t**)malloc(sizeof(hashmapconst_entry_t*) * map->capacity);
    
    if (!map->table) {
        free(map);
        return NULL;
    }

    for (size_t i = 0; i < map->capacity; i++) {
        map->table[i] = NULL;
    }

    return map;
}

void hashmapconst_destroy(hashmapconst_t* map) {
    if (!map) return;

    for (size_t i = 0; i < map->capacity; i++) {
        hashmapconst_entry_t* entry = map->table[i];
        while (entry) {
            hashmapconst_entry_t* temp = entry;
            entry = entry->next;
            free(temp->name);
            free(temp);
        }
    }

    free(map->table);
    free(map);
}

int hashmapconst_add(hashmapconst_t* map, const char* value, const float input_value)   {
    if (!map || !value) return 0;

    unsigned int index = hashmapconst_hash(value) % map->capacity;
    hashmapconst_entry_t* entry = map->table[index];

    while (entry) {
        if (strcmp(entry->name, value) == 0) {
            return 0;
        }
        entry = entry->next;
    }

    hashmapconst_entry_t* new_entry = (hashmapconst_entry_t*)malloc(sizeof(hashmapconst_entry_t));
    if (!new_entry) return 0;

    new_entry->name = strdup(value);
    new_entry->input_value = input_value;
    if (!new_entry->name) {
        free(new_entry);
        return 0;
    }

    new_entry->next = map->table[index];
    map->table[index] = new_entry;
    map->size++;

    if (map->size > map->capacity * 0.7) {
        hashmapconst_resize(map);
    }

    return 1;
}

int hashmapconst_update(hashmapconst_t* map, const char* value, const float input_value)   {
    if (!map || !value) return 0;

    unsigned int index = hashmapconst_hash(value) % map->capacity;
    hashmapconst_entry_t* entry = map->table[index];

    while (entry) {
        if (strcmp(entry->name, value) == 0) {
            entry->input_value =input_value;
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

void hashmapconst_remove(hashmapconst_t* map, const char* name) {
    if (!map || !name) return;

    unsigned int index = hashmapconst_hash(name) % map->capacity;
    hashmapconst_entry_t* entry = map->table[index];
    hashmapconst_entry_t* prev = NULL;

    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                map->table[index] = entry->next;
            }

            free(entry->name);
            free(entry);
            map->size--;
            return;
        }

        prev = entry;
        entry = entry->next;
    }
}

int hashmapconst_contains(hashmapconst_t* map, const char* name) {
    if (!map || !name) return 0;

    unsigned int index = hashmapconst_hash(name) % map->capacity;
    hashmapconst_entry_t* entry = map->table[index];

    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return 1;
        }
        entry = entry->next;
    }

    return 0;
}

void hashmapconst_resize(hashmapconst_t* map) {
    if (!map) return;

    size_t new_capacity = map->capacity * 2;
    hashmapconst_entry_t** new_table = (hashmapconst_entry_t**)malloc(sizeof(hashmapconst_entry_t*) * new_capacity);
    if (!new_table) return;

    for (size_t i = 0; i < new_capacity; i++) {
        new_table[i] = NULL;
    }

    for (size_t i = 0; i < map->capacity; i++) {
        hashmapconst_entry_t* entry = map->table[i];
        while (entry) {
            unsigned int new_index = hashmapconst_hash(entry->name) % new_capacity;

            hashmapconst_entry_t* new_entry = (hashmapconst_entry_t*)malloc(sizeof(hashmapconst_entry_t));
            if (!new_entry) return;

            new_entry->name = entry->name;
            new_entry->input_value = entry->input_value;
            new_entry->next = new_table[new_index];
            new_table[new_index] = new_entry;

            entry = entry->next;
        }
    }

    free(map->table);
    map->table = new_table;
    map->capacity = new_capacity;
}

float hashmapconst_get_value(hashmapconst_t* map, const char* key) {
    if (!map || !key) return -1;

    unsigned int index = hashmapconst_hash(key) % map->capacity;
    hashmapconst_entry_t* entry = map->table[index];

    while (entry) {
        if (strcmp(entry->name, key) == 0) {
            return entry->input_value;
        }
        entry = entry->next;
    }

    return -1;
}

hashmapconst_entry_t* hashmapconst_get_entry(hashmapconst_t* map, const char* key){
    if (!map || !key) return NULL;

    unsigned int index = hashmapconst_hash(key) % map->capacity;
    hashmapconst_entry_t* entry = map->table[index];

    while (entry) {
        if (strcmp(entry->name, key) == 0) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}
