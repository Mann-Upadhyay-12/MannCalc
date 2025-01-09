#ifndef PRECEDENCE_H
#define PRECEDENCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Size of the hashmap used for operator precedence lookup
#define HASHMAP_SIZE 17

/**
 * @brief Defines operator associativity direction
 */
typedef enum {
    LEFT_TO_RIGHT,  // Operators that evaluate left to right (e.g. +, -, *, /)
    RIGHT_TO_LEFT   // Operators that evaluate right to left (e.g. ^)
} Associativity;

/**
 * @brief Structure representing a mathematical operator and its properties
 */
typedef struct Operator {
    char *symbol;          // Operator symbol (e.g. +, -, *, /, ^)
    int precedence;        // Operator precedence level
    Associativity assoc;   // Operator's associativity direction
    struct Operator *next; // Next operator in case of hash collision
} Operator;

/**
 * @brief Hashmap structure for storing operator information
 */
typedef struct {
    Operator *buckets[HASHMAP_SIZE]; // Array of operator linked lists
} HashMap;

/**
 * @brief Initializes an empty operator hashmap
 * @param map Pointer to hashmap to initialize
 */
void initialize_hashmap(HashMap *map);

/**
 * @brief Computes hash value for an operator symbol
 * @param symbol Operator symbol to hash
 * @return Hash value for the symbol
 */
int hash_function(char *symbol);

/**
 * @brief Retrieves operator information from the hashmap
 * @param map Hashmap to search in
 * @param symbol Operator symbol to look up
 * @return Pointer to operator info or NULL if not found
 */
Operator* precidence_hashmap_get(HashMap *map, char *symbol);

/**
 * @brief Frees memory allocated for an operator
 * @param op Pointer to operator to free
 */
void free_operator(Operator *op);

/**
 * @brief Frees all memory allocated for the hashmap
 * @param map Pointer to hashmap to free
 */
void free_hashmap(HashMap *map);

/**
 * @brief Prints operator information for debugging
 * @param op Pointer to operator to print
 */
void print_operator_info(Operator *op);

/**
 * @brief Initializes hashmap with standard mathematical operators
 * @param map Pointer to hashmap to initialize
 * @return Initialized hashmap or NULL on failure
 */
HashMap* innit_precidence(HashMap *map);

/**
 * @brief Gets precedence level for an operator
 * @param map Hashmap containing operator information
 * @param symbol Operator symbol to look up
 * @return Precedence level or -1 if operator not found
 */
int return_precidence(HashMap *map, char *symbol);

#endif /* PRECEDENCE_H */
