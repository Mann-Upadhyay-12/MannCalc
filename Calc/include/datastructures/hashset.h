#ifndef HASHSET_H
#define HASHSET_H

#include <stddef.h>

// Define the initial size of the hash set
#define HASHSET_INITIAL_SIZE 16

// Structure for each entry in the hash set (bucket entry)
typedef struct hashset_entry {
    char* value;                ///< The value stored in the set
    int input_spaces;         ///< The input how much input does a function take
    struct hashset_entry* next; ///< Pointer to the next entry (for collision handling)
} hashset_entry_t;

// Structure for the hash set
typedef struct {
    hashset_entry_t** table;  ///< Array of linked list heads (buckets)
    size_t size;              ///< Current number of elements in the hash set
    size_t capacity;          ///< Current capacity of the hash set
} hashset_t;

/**
 * @brief Creates a new hash set.
 * 
 * Allocates and initializes a new hash set with an initial capacity.
 * 
 * @return A pointer to the newly created hash set, or NULL if allocation fails.
 */
hashset_t* hashset_create(void);

/**
 * @brief Destroys the hash set and frees all memory.
 * 
 * Frees the memory allocated for the hash set, including all stored values.
 * 
 * @param set The hash set to destroy.
 */
void hashset_destroy(hashset_t* set);

/**
 * @brief Computes the hash value of a string.
 * 
 * Uses a simple hash function to calculate an index for the given value.
 * 
 * @param value The value to hash.
 * 
 * @return The hash value, which is an integer index for the hash set's table.
 */
unsigned int hashset_hash(const char* value);

/**
 * @brief Adds a value to the hash set.
 * 
 * Inserts a new value into the set. If the value already exists, the set is not modified.
 * 
 * @param set The hash set in which to add the value.
 * @param value The value to add to the set.
 * 
 * @return 1 if the value was successfully added, 0 if it was already present.
 */
int hashset_add(hashset_t* set, const char* value,const int input_spaces);

/**
 * @brief Removes a value from the hash set.
 * 
 * Removes the specified value from the set if it exists.
 * 
 * @param set The hash set from which to remove the value.
 * @param value The value to remove from the set.
 */
void hashset_remove(hashset_t* set, const char* value);

/**
 * @brief Checks if a value exists in the hash set.
 * 
 * Searches the hash set to determine if the given value exists.
 * 
 * @param set The hash set to search.
 * @param value The value to search for.
 * 
 * @return 1 if the value exists, 0 if it does not exist.
 */
int hashset_contains(hashset_t* set, const char* value);

/**
 * @brief Resizes the hash set when the load factor exceeds a certain threshold.
 * 
 * If the number of elements in the hash set exceeds 70% of its current capacity,
 * the hash set is resized to double its current size, and all entries are rehashed.
 * 
 * @param set The hash set to resize.
 */
void hashset_resize(hashset_t* set);
int hashset_get_value(hashset_t* set, const char* key);
hashset_entry_t* hashset_get_entry(hashset_t* set, const char* key) ;
#endif 
// HASHSET_H
