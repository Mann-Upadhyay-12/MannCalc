
#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

// Define the initial size of the hash table
#define HASHMAP_INITIAL_SIZE 16

// Structure for each entry in the hash table (key-value pair)
typedef struct hashmap_entry {
    char* key;                    ///< The key of the entry
    char* value;                 ///< The value associated with the key
    struct hashmap_entry* next; ///< Pointer to the next entry (for collision handling)
} hashmap_entry_t;

// Structure for the hashmap
typedef struct {
    hashmap_entry_t** table;    ///< Array of linked list heads (buckets)
    size_t size;               ///< Current number of elements in the hashmap
    size_t capacity;          ///< Current capacity of the hashmap
} hashmap_t;

/**
 * @brief Creates a new hashmap.
 * 
 * Allocates and initializes a new hashmap with an initial capacity.
 * 
 * @return A pointer to the newly created hashmap, or NULL if allocation fails.
 */
hashmap_t* hashmap_create(void);

/**
 * @brief Destroys the hashmap and frees all memory.
 * 
 * Frees the memory allocated for the hashmap, including all key-value pairs.
 * 
 * @param hashmap The hashmap to destroy.
 */
void hashmap_destroy(hashmap_t* hashmap);

/**
 * @brief Computes the hash value of a string key.
 * 
 * Uses a simple hash function to calculate an index for the given key.
 * 
 * @param key The key to hash.
 * 
 * @return The hash value, which is an integer index for the hashmap's table.
 */
unsigned int hashmap_hash(const char* key);

/**
 * @brief Inserts a key-value pair into the hashmap.
 * 
 * Adds a new key-value pair to the hashmap. If the key already exists,
 * it updates the associated value.
 * 
 * @param hashmap The hashmap in which the key-value pair is to be inserted.
 * @param key The key for the new entry.
 * @param value The value associated with the key.
 */
void hashmap_put(hashmap_t* hashmap, const char* key, const char* value);

/**
 * @brief Retrieves the value for a given key.
 * 
 * Searches for the key in the hashmap and returns its associated value.
 * If the key does not exist, it returns NULL.
 * 
 * @param hashmap The hashmap from which to retrieve the value.
 * @param key The key for which to retrieve the value.
 * 
 * @return The value associated with the key, or NULL if the key does not exist.
 */
char* hashmap_get(hashmap_t* hashmap, const char* key);

/**
 * @brief Removes a key-value pair from the hashmap.
 * 
 * Deletes the key-value pair from the hashmap, freeing the memory associated
 * with the key and value.
 * 
 * @param hashmap The hashmap from which the key-value pair is to be removed.
 * @param key The key to remove.
 */
void hashmap_remove(hashmap_t* hashmap, const char* key);

/**
 * @brief Checks if a key exists in the hashmap.
 * 
 * Searches the hashmap to determine if the given key exists.
 * 
 * @param hashmap The hashmap to search.
 * @param key The key to search for.
 * 
 * @return 1 if the key exists, 0 if it does not exist.
 */
int hashmap_contains_key(hashmap_t* hashmap, const char* key);

/**
 * @brief Resizes the hashmap when the load factor exceeds a certain threshold.
 * 
 * If the number of elements in the hashmap exceeds 70% of its current capacity,
 * the hashmap is resized to double its current size, and all entries are rehashed.
 * 
 * @param hashmap The hashmap to resize.
 */
void hashmap_resize(hashmap_t* hashmap);

int hashmap_get(hashmap_t* map, const char* key);
#endif // HASHMAP_H
