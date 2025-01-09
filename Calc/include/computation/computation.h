#ifndef COMPUTATION_H
#define COMPUTATION_H

#include <math.h>
#include "AST_tree.h"

#define LOGS(fmt, ...) fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * @brief Error codes for computation operations
 */
typedef enum {
    COMPUTATION_OK = 0,           // No error occurred
    COMPUTATION_NULL_INPUT,       // Input was null
    COMPUTATION_DIVISION_BY_ZERO, // Division by zero attempted
    COMPUTATION_INVALID_OPERATION,// Invalid mathematical operation
    COMPUTATION_UNDEFINED_FUNCTION,// Function not found in supported set
    COMPUTATION_MEMORY_ERROR     // Memory allocation failed
} ComputationError;

/**
 * @brief Structure containing computation results and error information
 */
typedef struct {
    double value;           // The computed numeric result
    ComputationError error; // Error status of computation
    char* error_msg;        // Optional error message
    ASTNode* root;         // Root of AST used in computation
} ComputationResult;

/**
 * @brief Computes result from an AST
 * @param result Parse result containing AST to evaluate
 * @return ComputationResult with value or error information
 */
ComputationResult compute_ast(ParseResult* result);

/**
 * @brief Traverses AST for computation
 * @param node Current node being processed
 */
void traversal(ASTNode* node);

#endif /* COMPUTATION_H */
