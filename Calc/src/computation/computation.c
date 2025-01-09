#include "../../include/computation/computation.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include "../../include/computation/AST_tree.h"
#include "../../include/computation/tokenizer.h"
#define M_PI 3.14159265358979323846
#include "../../include/datastructures/hashmapforconst.h"

void print_token_just_val_test(ASTNode* node) {
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
            fprintf(stderr, "%s(%f) ", node->token->data.var_name->name,node->token->data.var_name->input_value); 
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

void print_tree_recursive_test(ASTNode* node, int level, char* prefix, bool is_last) {
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

    char* new_prefix = malloc(strlen(prefix) + 5);
    strncpy(new_prefix, prefix, strlen(prefix) + 1);
    strncat(new_prefix, is_last ? "    " : "│   ", 4);

    if (node->left) {
        print_tree_recursive(node->left, level + 1, new_prefix, false);
    }
    if (node->right) {
        print_tree_recursive(node->right, level + 1, new_prefix, true);
    }

    free(new_prefix);
}

void print_tree_test(ASTNode* root) {
    print_tree_recursive_test(root, 0, "", true);
}

void traversal(ASTNode* node) {
    if(node == NULL) {
        return;
    }

    traversal(node->left);
    if(node->right!=NULL) {
        traversal(node->right);
    }
    
    if(node->token->type == TOKEN_OPERATOR) 
    {
        if(node->token->data.operator_value == '+') {
            double data_computed = 0;
            data_computed = node->left->token->data.num_value + node->right->token->data.num_value;
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if (node->token->data.operator_value == '-')
        {
            double data_computed = 0;
            data_computed = node->left->token->data.num_value - node->right->token->data.num_value;
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if (node->token->data.operator_value == '*')
        {
            double data_computed = 0;
            data_computed = node->left->token->data.num_value * node->right->token->data.num_value;
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }  
        else if (node->token->data.operator_value == '/')
        {
            double data_computed = 0;
            data_computed = node->left->token->data.num_value / node->right->token->data.num_value;
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }   
        else if (node->token->data.operator_value == '^')
        {
            double data_computed = 0;
            data_computed = pow(node->left->token->data.num_value, node->right->token->data.num_value);
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }   
    }
    else if(node->token->type == TOKEN_NUMBER) {
        return;
    }
    else if(node->token->type == TOKEN_VARIABLE) {
        return;
    }
    else if(node->token->type == TOKEN_FUNCTION) {  
        if (strcmp(node->token->data.function_name->value, "logbase") == 0)
        {
            if (!node->left || !node->right || !node->left->token || !node->right->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = log(node->right->token->data.num_value)/log(node->left->token->data.num_value);
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if(strcmp(node->token->data.function_name->value, "sin") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            double radians = (node->child->token->data.num_value * M_PI) / 180.0;
            data_computed = sin(radians);
            
            node->token->type = TOKEN_NUMBER;
            node->token->data.num_value = data_computed;
            print_tree_test(node);
        }
        else if(strcmp(node->token->data.function_name->value, "cos") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = cos((node->child->token->data.num_value));
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if(strcmp(node->token->data.function_name->value, "tan") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = tan((node->child->token->data.num_value));
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if(strcmp(node->token->data.function_name->value, "log") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = log((node->child->token->data.num_value));
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if(strcmp(node->token->data.function_name->value, "sqrt") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = sqrt((node->child->token->data.num_value));
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if(strcmp(node->token->data.function_name->value, "exp") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = exp((node->child->token->data.num_value));
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if(strcmp(node->token->data.function_name->value, "abs") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = fabs((node->child->token->data.num_value));
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
    }
        else if(strcmp(node->token->data.function_name->value, "log10") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = log10((node->child->token->data.num_value));
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if(strcmp(node->token->data.function_name->value, "log2") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = log2((node->child->token->data.num_value));
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if(strcmp(node->token->data.function_name->value, "loge") == 0) 
        {
            if (!node->child || !node->child->token) {
                return;
            }
            
            double data_computed = 0;
            data_computed = log((node->child->token->data.num_value));
            
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        return;
    }
    else if(node->token->type == TOKEN_UNARY) {
        if(node->token->data.unary_operator == TOKEN_UNARY_NEGATIVE) 
        {
            double data_computed = 0;
            data_computed = (node->child->token->data.num_value)*-1;
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        else if(node->token->data.unary_operator == TOKEN_UNARY_POSITIVE) 
        {
            double data_computed = 0;
            data_computed = (node->child->token->data.num_value);
            node->token->type=TOKEN_NUMBER;
            node->token->data.num_value=data_computed;
        }
        return;
    }
}

ComputationResult compute_ast(ParseResult* result) {
    ComputationResult ans;
    ans.error = COMPUTATION_OK;
    ans.error_msg = NULL;
    ans.value = 0;
    ans.root = result->root;

    if(result == NULL) {
        ans.error = COMPUTATION_NULL_INPUT;
        ans.error_msg = "No root node provided";
        return ans;
    }

    if(result->error != AST_OK) {  
        ans.error = COMPUTATION_INVALID_OPERATION;
        ans.error_msg = result->error_msg;
        return ans;
    }

    if(result->root->token->type == TOKEN_EQUALITY)
    {
      traversal(result->root->right);
      ans.value = result->root->right->token->data.num_value;
      hashmapconst_update(VARIABLES, result->root->left->token->data.var_name->name,ans.value);
      ans.value = result->root->right->token->data.num_value;
      result->root->token->type=TOKEN_NUMBER;
      result->root->token->data.num_value=ans.value;
    }
    else{
    traversal(result->root);
    print_tree(result);
    ans.value = result->root->token->data.num_value;
  }
    FILE * file = fopen("computation.txt", "w");
    fprintf(file, "%f\n", ans.value);
    fclose(file);
    return ans;
}
