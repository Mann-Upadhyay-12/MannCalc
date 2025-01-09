#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../../include/computation/tokenizer.h"
#include "../../include/computation/AST_tree.h"
#include "../../include/computation/precidence.h"
#include "../../include/datastructures/hashset.h"

#pragma GCC diagnostic ignored "-Wunused-function"

ASTResult ast_create_node(Token* token) {
    ASTResult result = {NULL, AST_OK, NULL};
    
    if (!token) {
        result.error = AST_NULL_INPUT;
        result.error_msg = "Token is NULL";
        return result;
    }

    ASTNode* node = (ASTNode*)calloc(1, sizeof(ASTNode));
    if (!node) {
        result.error = AST_MEMORY_ERROR;
        result.error_msg = "Memory allocation failed";
        return result;
    }
    
    node->token = token;
    node->left = NULL;
    node->right = NULL; 
    node->child = NULL;
    
    result.node = node;
    return result;
}

ASTStack* ast_create_stack(size_t initial_capacity) {
    ASTStack* stack = (ASTStack*)malloc(sizeof(ASTStack));
    if (!stack) {
        return NULL;
    }
    
    stack->nodes = (ASTNode**)malloc(initial_capacity * sizeof(ASTNode*));
    if (!stack->nodes) {
        free(stack);
        return NULL;
    }
    
    stack->capacity = initial_capacity;
    stack->size = 0;
    return stack;
}

ASTError ast_push(ASTStack* stack, ASTNode* node) {
    if (!stack || !node) {
        return AST_NULL_INPUT;
    }
    
    if (stack->size >= stack->capacity) {
        size_t new_capacity = stack->capacity * 2;
        ASTNode** new_nodes = (ASTNode**)realloc(stack->nodes, new_capacity * sizeof(ASTNode*));
        if (!new_nodes) {
            return AST_MEMORY_ERROR;
        }
        stack->nodes = new_nodes;
        stack->capacity = new_capacity;
    }
    
    stack->nodes[stack->size++] = node;
    return AST_OK;
}

ASTNode* ast_pop(ASTStack* stack) {
   if (!stack || stack->size == 0) {
       return NULL;
   }
   
   ASTNode* node = stack->nodes[--stack->size];
   return node;
}

ASTResult ast_peek(ASTStack* stack) {
    ASTResult result = {NULL, AST_OK, NULL};
    
    if (!stack || stack->size == 0) {
        result.error = AST_NULL_INPUT;
        result.error_msg = "Stack is empty or NULL";
        return result;
    }
    
    result.node = stack->nodes[stack->size - 1];
    return result;
}

ASTError ast_set_left(ASTNode* parent, ASTNode* left) {
    if (!parent) {
        return AST_NULL_INPUT;
    }
    
    parent->left = left;
    return AST_OK;
}

ASTError ast_set_right(ASTNode* parent, ASTNode* right) {
    if (!parent) {
        return AST_NULL_INPUT;
    }
    
    parent->right = right;
    return AST_OK;
}

ASTError ast_set_child(ASTNode* parent, ASTNode* child) {
    if (!parent) {
        return AST_NULL_INPUT;
    }
    
    parent->child = child;
    return AST_OK;
}

bool ast_has_children(const ASTNode* node) {
    if (!node) return false;
    return (node->left != NULL || node->right != NULL || node->child != NULL);
}

void print_token_just_val(ASTNode* node) {
    switch (node->token->type) {
        case TOKEN_NUMBER: 
            fprintf(stderr, "%f ", node->token->data.num_value); 
            break;
        case TOKEN_OPERATOR: 
            fprintf(stderr, "%c ", node->token->data.operator_value); 
            break;
        case TOKEN_FUNCTION: 
            fprintf(stderr, "%s ", node->token->data.function_name->value); 
            break;
        case TOKEN_VARIABLE: 
            fprintf(stderr, "%s(%f) ", node->token->data.var_name->name,hashmapconst_get_value(VARIABLES,node->token->data.var_name->name)); 
            break;
        case TOKEN_PARENTHESIS:
        case TOKEN_EQUALITY:
            fprintf(stderr, "%c ", node->token->data.parenthesis);
            break;
        case TOKEN_UNARY:
            if (node->token->data.unary_operator == TOKEN_UNARY_NEGATIVE) {
                fprintf(stderr, "NEG ");
            } else if (node->token->data.unary_operator == TOKEN_UNARY_POSITIVE) {
                fprintf(stderr, "POS ");
            }
            break;
        default:
            fprintf(stderr, "Unknown");
    }
}

void print_tree_recursive(ASTNode* node, int level, char* prefix, bool is_last) {
    if (!node) return;

    fprintf(stderr, "%s", prefix);
    fprintf(stderr, "%s", is_last ? "└── " : "├── ");

    print_token_just_val(node);

    if (node->child) {
        fprintf(stderr, " --- ");
        print_tree_recursive(node->child, level + 1, "", true);
    } else {
        fprintf(stderr, "\n");
    }

    size_t new_prefix_len = strlen(prefix) + 5;
    char* new_prefix = malloc(new_prefix_len);
    if (!new_prefix) {
        fprintf(stderr, "Memory allocation failed in print_tree_recursive\n");
        return;
    }

    strncpy(new_prefix, prefix, new_prefix_len);
    new_prefix[new_prefix_len - 5] = '\0';
    strncat(new_prefix, is_last ? "    " : "│   ", 4);
    new_prefix[new_prefix_len - 1] = '\0';

    if (node->left) {
        print_tree_recursive(node->left, level + 1, new_prefix, node->right == NULL);
    }
    if (node->right) {
        print_tree_recursive(node->right, level + 1, new_prefix, true);
    }

    free(new_prefix);
}

void print_tree(ParseResult* result) {
    if (!result || !result->root) {
        fprintf(stderr, "Empty tree\n");
        return;
    }
    print_tree_recursive(result->root, 0, "", true);
}

ParseResult* parse_expression(const TokenizerResult* tokens) {
    HashMap* precedence_map = innit_precidence(NULL);
    if (!precedence_map) {
        ParseResult* result = malloc(sizeof(ParseResult));
        result->error = AST_MEMORY_ERROR;
        result->error_msg = "Failed to create precedence map";
        return result;
    }
    
    ParseResult* result = malloc(sizeof(ParseResult));
    result->root = NULL;
    result->error = AST_OK;
    result->error_msg = NULL;
    result->tokens_processed = 0;

    if (!tokens || tokens->token_count == 0) {
        result->error = AST_NULL_INPUT;
        result->error_msg = "No tokens to parse";
        free_hashmap(precedence_map);
        return result;
    }

    ASTStack* stack = ast_create_stack(tokens->token_count);
    if (!stack) {
        result->error = AST_MEMORY_ERROR;
        result->error_msg = "Failed to create stack";
        free_hashmap(precedence_map);
        return result;
    }
    
    size_t token_idx = 0;
    while(token_idx < tokens->token_count) {
        ASTResult node_result = ast_create_node(&tokens->tokens[token_idx]);
        if (node_result.error != AST_OK) {
            result->error = node_result.error;
            result->error_msg = node_result.error_msg;
            free_hashmap(precedence_map);
            return result;
        }
        
        ast_push(stack, node_result.node);
        token_idx++;

        ASTResult peek_result = ast_peek(stack);
        if (peek_result.error == AST_OK && peek_result.node->token->type == TOKEN_OPERATOR) {
            if(stack->size < 2) {
                result->error = AST_SYNTAX_ERROR;
                result->error_msg = "Invalid syntax";
                free_hashmap(precedence_map);
                return result;
            }
            
            Operator* current_op = precidence_hashmap_get(precedence_map, (char[]){peek_result.node->token->data.operator_value, '\0'});
            if (!current_op) {
                result->error = AST_SYNTAX_ERROR;
                result->error_msg = "Unknown operator";
                free_hashmap(precedence_map);
                return result;
            }
            
            ASTNode* op = ast_pop(stack);
            ASTNode* right = ast_pop(stack);
            ASTNode* left = ast_pop(stack);
            op->left = left;
            op->right = right;
            ast_push(stack, op);
        }
        else if (peek_result.error == AST_OK && peek_result.node->token->type == TOKEN_UNARY) {
            if(stack->size < 1) {
                result->error = AST_SYNTAX_ERROR;
                result->error_msg = "Invalid syntax";
                free_hashmap(precedence_map);
                return result;
            }
            
            ASTNode* op = ast_pop(stack);
            ASTNode* child = ast_pop(stack);
            op->child= child;
            ast_push(stack, op);
        }
        else if(peek_result.error == AST_OK && peek_result.node->token->type == TOKEN_FUNCTION) {
            if(stack->size < 1) {
                result->error = AST_SYNTAX_ERROR;
                result->error_msg = "Invalid syntax";
                free_hashmap(precedence_map);
                return result;
            }
            int child_count = hashset_get_value(SUPPORTED_FUNCTIONS, peek_result.node->token->data.function_name->value);
            if(child_count == 0) {
                result->error = AST_SYNTAX_ERROR;
                result->error_msg = "Invalid syntax";
                free_hashmap(precedence_map);
                return result;
            }
            if(child_count == 1) {
                ASTNode* op = ast_pop(stack);
                ASTNode* child = ast_pop(stack);
                op->child= child;
                ast_push(stack, op);
            }
            if(child_count == 2) {
                ASTNode* op = ast_pop(stack);
                ASTNode* child1 = ast_pop(stack);
                ASTNode* child2 = ast_pop(stack);
                op->left = child1;
                op->right = child2;
                ast_push(stack, op);
            }
        }
        else if(peek_result.error ==AST_OK && peek_result.node->token->type == TOKEN_EQUALITY)
        {
            if(stack->size < 2) {
                result->error = AST_SYNTAX_ERROR;
                result->error_msg = "Invalid syntax";
                free_hashmap(precedence_map);
                return result;
            }
            ASTNode* op = ast_pop(stack);
            ASTNode* right = ast_pop(stack);
            ASTNode* left = ast_pop(stack);
            op->left = left;
            op->right = right;
            ast_push(stack, op);
        }
        else if(peek_result.error == AST_OK && peek_result.node->token->type == TOKEN_EQUALITY)
        {
            if(stack->size < 2) {
                result->error = AST_SYNTAX_ERROR;
                result->error_msg = "Invalid syntax";
                free_hashmap(precedence_map);
                return result;
            }
            ASTNode* op = ast_pop(stack);
            ASTNode* right = ast_pop(stack);
            ASTNode* left = ast_pop(stack);
            op->left = left;
            op->right = right;
            ast_push(stack, op);
        }
    }
    
    result->root = ast_pop(stack);
    ast_free_stack(stack);
    free_hashmap(precedence_map);
    return result;
}

static void ast_free_token(Token* token) {
    if (!token) return;
    
    if (token->type == TOKEN_FUNCTION && token->data.function_name) {
        free(token->data.function_name);
        token->data.function_name = NULL;
    }
    else if (token->type == TOKEN_VARIABLE && token->data.var_name->name) {
        free(token->data.var_name);
        token->data.var_name = NULL;
    }
}

void ast_free_node(ASTNode** node_ptr) {
    if (!node_ptr || !*node_ptr) return;
    
    if ((*node_ptr)->left) ast_free_node(&(*node_ptr)->left);
    if ((*node_ptr)->right) ast_free_node(&(*node_ptr)->right);
    if ((*node_ptr)->child) ast_free_node(&(*node_ptr)->child);
    
    free(*node_ptr);
    *node_ptr = NULL;
}

void cleanup_ast(ParseResult* result) {
    if (!result) return;
    
    if (result->root) {
        ast_free_node(&result->root);
        result->root = NULL;
    }
    
    result->error = AST_OK;
    result->error_msg = NULL;
    result->tokens_processed = 0;
    
    free(result);
}

void ast_free_stack(ASTStack* stack) {
    if (!stack) {
        return;
    }

    if (stack->nodes) {
        for (size_t i = 0; i < stack->size; i++) {
            if (stack->nodes[i]) {
                ast_free_node(&stack->nodes[i]);
                stack->nodes[i] = NULL;
            }
        }
        free(stack->nodes);
        stack->nodes = NULL;
    }

    stack->size = 0;
    stack->capacity = 0;
    free(stack);
}
