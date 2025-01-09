#ifndef AST_TREE_H
#define AST_TREE_H

#include <stdlib.h>
#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "datastructures/hashset.h"

// Debug logging macro
#define LOGS(fmt, ...) fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * @brief Node structure for Abstract Syntax Tree representation
 * @future Consider adding line/column tracking for better error reporting
 */
typedef struct ASTNode {
    Token* token;
    struct ASTNode* left;    // Left child (for binary operators)
    struct ASTNode* right;   // Right child (for binary operators)
    struct ASTNode* child;   // For functions (e.g., sin, log)
} ASTNode;

/**
 * @brief Error types for AST operations
 * @future Add more specific error codes for different parsing scenarios
 */
typedef enum {
    AST_OK = 0,        // No error occurred
    AST_NULL_INPUT,    // Input was null
    AST_MEMORY_ERROR,  // Memory allocation failed
    AST_SYNTAX_ERROR,  // Syntax error in parsing
    AST_INVALID_TOKEN,  // Invalid token encountered
    AST_IM_FREE         // This AST is free
} ASTError;

/**
 * @brief Result structure for AST operations
 * @future Add position information for error reporting
 */
typedef struct {
    ASTNode* node;     // The resulting node
    ASTError error;    // Error status
    char* error_msg;   // Optional error message
} ASTResult;

/**
 * @brief Structure containing parse results and error information
 * @future Add source code location for error reporting
 */
typedef struct {
    ASTNode* root;
    ASTError error;
    char* error_msg;
    int tokens_processed;
} ParseResult;

/**
 * @brief Stack structure for AST construction
 * @future Add overflow protection and dynamic resizing
 */
typedef struct ASTStack {
    ASTNode** nodes;   // Array of node pointers
    size_t capacity;   // Maximum capacity
    size_t size;       // Current size
} ASTStack;

// Function declarations

/**
 * @brief Creates a new AST node from a token
 * @param token The token to create the node from
 * @return ASTResult containing the created node or error information
 * @future Add memory pool allocation for better performance
 */
ASTResult ast_create_node(Token* token);

/**
 * @brief Creates a new stack for AST construction
 * @param initial_capacity Initial size of the stack
 * @return Pointer to created stack or NULL if allocation fails
 * @future Add error reporting mechanism
 */
ASTStack* ast_create_stack(size_t initial_capacity);

/**
 * @brief Pushes a node onto the stack
 * @param stack The stack to push to
 * @param node The node to push
 * @return Error status of the operation
 * @future Add stack overflow protection
 */
ASTError ast_push(ASTStack* stack, ASTNode* node);

/**
 * @brief Pops a node from the stack
 * @param stack The stack to pop from
 * @return The popped node or NULL if stack is empty
 * @future Add underflow checking
 */
ASTNode* ast_pop(ASTStack* stack);

/**
 * @brief Peeks at the top node of the stack
 * @param stack The stack to peek at
 * @return ASTResult containing top node or error information
 * @future Add bounds checking
 */
ASTResult ast_peek(ASTStack* stack);

/**
 * @brief Frees an AST node and all its children
 * @param node Pointer to the node pointer to free
 * @future Add memory leak detection
 */
void ast_free_node(ASTNode** node);

/**
 * @brief Frees a stack and all its contents
 * @param stack The stack to free
 * @future Add cleanup verification
 */
void ast_free_stack(ASTStack* stack);

/**
 * @brief Sets the left child of a node
 * @param parent Parent node
 * @param left Node to set as left child
 * @return Error status of the operation
 * @future Add cycle detection
 */
ASTError ast_set_left(ASTNode* parent, ASTNode* left);

/**
 * @brief Sets the right child of a node
 * @param parent Parent node
 * @param right Node to set as right child
 * @return Error status of the operation
 * @future Add validation checks
 */
ASTError ast_set_right(ASTNode* parent, ASTNode* right);

/**
 * @brief Sets the child of a node (for unary operations)
 * @param parent Parent node
 * @param child Node to set as child
 * @return Error status of the operation
 * @future Add type checking
 */
ASTError ast_set_child(ASTNode* parent, ASTNode* child);

/**
 * @brief Checks if a node has any children
 * @param node Node to check
 * @return true if node has children, false otherwise
 * @future Add validation for corrupted nodes
 */
bool ast_has_children(const ASTNode* node);

/**
 * @brief Recursively prints the tree structure
 * @param node Current node to print
 * @param level Current depth in tree
 * @param prefix String prefix for formatting
 * @param is_last Whether this is the last node at this level
 * @future Add options for different output formats
 */
void print_tree_recursive(ASTNode* node, int level, char* prefix, bool is_last);

/**
 * @brief Prints the entire parse tree
 * @param result Parse result containing the tree to print
 * @future Add output customization options
 */
void print_tree(ParseResult* result);

/**
 * @brief Parses a token stream into an AST
 * @param tokens Token stream to parse
 * @return Parse result containing AST or error information
 * @future Add recovery mechanisms for syntax errors
 */
ParseResult* parse_expression(const TokenizerResult* tokens);

/**
 * @brief Prints just the value of a node's token
 * @param node Node whose token to print
 * @future Add formatting options
 */
void print_token_just_val(ASTNode* node);

/**
 * @brief Initializes the set of supported functions
 * @return Initialized hashset of supported functions
 * @future Add configuration file support
 */
hashset_t* init_SUPPORTED_FUNCTIONS_();

/**
 * @brief Cleans up an AST and frees all associated memory
 * @param ast_root Root of the AST to clean up
 * @future Add memory leak detection
 */
void cleanup_ast(ParseResult* ast_root);

#endif // AST_TREE_H
