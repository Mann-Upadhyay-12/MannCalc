/**
 * @file tokenizer.h
 * @brief Header file for mathematical expression tokenizer
 *
 * This header defines the public interface and data structures for the tokenizer,
 * including token types, error codes, and main tokenization functions.
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>  // For size_t
#include <stdbool.h> // For boolean type
#include "datastructures/hashset.h" // Include hashset header for supported functions
#include "datastructures/hashmapforconst.h"// For constants like pi and e

// Debug logging macro
#define LOGS(fmt, ...) fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// Define supported operators
// IMPROVEMENT: Consider making this const char* instead of char[] for better type safety
static const char OPERATORS[] = "+-*/^";

// Error codes for tokenizer operations
// IMPROVEMENT: Add documentation for each error code
typedef enum {
    TOKEN_SUCCESS,        // Operation completed successfully
    TOKEN_NULL_INPUT,     // Null pointer was provided as input
    TOKEN_MEMORY_ERROR,   // Memory allocation failed
    TOKEN_INPUT_TOO_LONG, // Input exceeds maximum allowed length
    TOKEN_INVALID_INPUT   // Input contains invalid characters or format
} TokenizerError;

// Token types for different elements in mathematical expressions
typedef enum {
    TOKEN_NUMBER,      // Numeric values (integers or floating-point)
    TOKEN_OPERATOR,    // Mathematical operators (+, -, *, /, ^)
    TOKEN_FUNCTION,    // Mathematical functions (sin, cos, etc.)
    TOKEN_PARENTHESIS, // Opening and closing parentheses
    TOKEN_VARIABLE,    // Variable names
    TOKEN_EQUALITY,    // Equality operator (=)
    EOF_TOKEN,         // End of file marker
    TOKEN_UNARY,        // Unary operator
    TOKEN_COMMA        // Comma separator
} TokenType;


typedef enum {
    TOKEN_UNARY_NEGATIVE,
    TOKEN_UNARY_POSITIVE
} UnaryOperator;

// Global set of supported mathematical functions
// IMPROVEMENT: Consider making this const after initialization
extern hashset_t* SUPPORTED_FUNCTIONS;

extern hashmapconst_t* VARIABLES;


// Union to store different types of token data efficiently
typedef union {
    double num_value;      // For TOKEN_NUMBER: stores numeric values
    char operator_value;   // For TOKEN_OPERATOR: stores operator symbol
    hashset_entry_t* function_name;   // For TOKEN_FUNCTION: stores function name (dynamically allocated)
    hashmapconst_entry_t* var_name;       // For TOKEN_VARIABLE: stores variable name (dynamically allocated)
    char equality;        // For TOKEN_EQUALITY: stores equality symbol
    char parenthesis;     // For TOKEN_PARENTHESIS: stores parenthesis character
    UnaryOperator unary_operator; // For TOKEN_UNARY: stores unary operator
    char comma;           // For TOKEN_COMMA: stores comma character
} TokenData;

// Complete token structure combining type and data
typedef struct {
    TokenType type;    // Type identifier for the token
    TokenData data;    // Actual data stored in the token
} Token;

// Structure for managing arrays of string tokens
// IMPROVEMENT: Add capacity field for better memory management
typedef struct {
    char **tokens;     // Array of token strings (dynamically allocated)
    size_t size;       // Current number of tokens in the array
} TokenArrChar;

// Structure for returning tokenization results
typedef struct {
    Token* tokens;         // Array of processed tokens
    size_t token_count;    // Number of tokens in the array
    TokenizerError error;  // Error status of the tokenization
    int num_vars;
} TokenizerResult;





// Function declarations
// IMPROVEMENT: Add error parameter to functions that can fail
hashset_t* init_SUPPORTED_FUNCTIONS_();
// Frees memory allocated for tokens
void cleanup_tokens(Token* tokens,size_t size);

// Adds proper spacing around operators and special characters
TokenizerError replace_characters(const char *input, char *output, size_t output_size);

// Converts string to lowercase for case-insensitive processing
TokenizerError to_lowercase(char *str, size_t max_len);
// Frees memory allocated for token array
void free_token_array(TokenArrChar *token_array);

// Validates function names against supported functions set
 bool is_valid_function(hashset_t* SUPPORTED_FUNCTIONS,const char *str);

// Core tokenization function
Token* tokenize(const char *str, const char *delim, size_t *token_count, TokenizerError *error);

// High-level tokenization interface
TokenizerResult tokenizeQuery(const char *input_string);

// Debugging function to print token information
void print_token(const Token* token);

// Test suite runner
void run_tests(void);

// Interactive input processor
void process_custom_input(void);

// User interface menu
void display_menu(void);



#endif // TOKENIZER_H
