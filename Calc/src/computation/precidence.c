#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHMAP_SIZE 17

typedef enum {
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT
} Associativity;

typedef struct Operator {
    char *symbol;
    int precedence;
    Associativity assoc;
    struct Operator *next;
} Operator;

typedef struct {
    Operator *buckets[HASHMAP_SIZE];
} HashMap;

HashMap* initialize_hashmap(HashMap *map) {
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        map->buckets[i] = NULL;
    }
    return map;
}

int hash_function(char *symbol) {
    int hash = 0;
    for (int i = 0; symbol[i] != '\0'; i++) {
        hash = (hash * 31 + symbol[i]) % HASHMAP_SIZE;
    }
    return hash;
}   

Operator* precidence_hashmap_get(HashMap *map, char *symbol) {
    int index = hash_function(symbol);
    Operator *op = map->buckets[index];
    while (op) {
        if (strcmp(op->symbol, symbol) == 0) {
            return op;
        }
        op = op->next;
    }
    return NULL;
}

void free_operator(Operator *op) {
    if (op) {
        free(op);
    }
}

void free_hashmap(HashMap *map) {
    if (!map) return;
    
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        Operator *current = map->buckets[i];
        while (current) {
            Operator *next = current->next;
            free_operator(current);
            current = next;
        }
        map->buckets[i] = NULL;
    }
    free(map);
}

void print_operator_info(Operator *op) {
    if (op) {
        fprintf(stderr, "Operator: %s\n", op->symbol);
        fprintf(stderr, "Precedence: %d\n", op->precedence);
        fprintf(stderr, "Associativity: %s\n", (op->assoc == LEFT_TO_RIGHT) ? "Left-to-Right" : "Right-to-Left");
    } else {
        fprintf(stderr, "Operator not found.\n");
    }
}

void hashmap_insert(HashMap* map, char* symbol, int precedence, Associativity assoc) {
    int index = hash_function(symbol);
    
    Operator* new_op = (Operator*)malloc(sizeof(Operator));
    new_op->symbol = symbol;
    new_op->precedence = precedence;
    new_op->assoc = assoc;
    new_op->next = map->buckets[index];
    
    map->buckets[index] = new_op;
}

HashMap* innit_precidence(HashMap* map) {
    map = (HashMap*)malloc(sizeof(HashMap));
    initialize_hashmap(map);
    
    hashmap_insert(map, "sin", 5, LEFT_TO_RIGHT);
    hashmap_insert(map, "cos", 5, LEFT_TO_RIGHT);
    hashmap_insert(map, "tan", 5, LEFT_TO_RIGHT);
    hashmap_insert(map, "log", 5, LEFT_TO_RIGHT);
    hashmap_insert(map, "sqrt", 5, LEFT_TO_RIGHT);
    
    hashmap_insert(map, "(", 4, LEFT_TO_RIGHT);
    hashmap_insert(map, ")", 4, LEFT_TO_RIGHT);
    
    hashmap_insert(map, "^", 3, RIGHT_TO_LEFT);
    
    hashmap_insert(map, "*", 2, LEFT_TO_RIGHT);
    hashmap_insert(map, "/", 2, LEFT_TO_RIGHT);
    hashmap_insert(map, "%", 2, LEFT_TO_RIGHT);
    
    hashmap_insert(map, "+", 1, LEFT_TO_RIGHT);
    hashmap_insert(map, "-", 1, LEFT_TO_RIGHT);
    
    hashmap_insert(map, "=", 0, RIGHT_TO_LEFT);
    hashmap_insert(map, "+=", 0, RIGHT_TO_LEFT);
    hashmap_insert(map, "-=", 0, RIGHT_TO_LEFT);
    hashmap_insert(map, "*=", 0, RIGHT_TO_LEFT);
    hashmap_insert(map, "/=", 0, RIGHT_TO_LEFT);
    hashmap_insert(map, "%=", 0, RIGHT_TO_LEFT);
    
    return map;
}

int return_precidence(HashMap *map, char *symbol) {
    Operator *op = precidence_hashmap_get(map, symbol);
    return op->precedence;
}
