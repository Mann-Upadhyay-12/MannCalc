#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include "../include/computation/tokenizer.h"
#include "../include/datastructures/hashset.h"
#include "../include/datastructures/hashmapforconst.h"
#include "../include/computation/shunt_yard_algo.h"
#include "../include/computation/AST_tree.h"
#include "../include/computation/computation.h"

#define MAX_INPUT_LENGTH 1024
#define INITIAL_TOKEN_CAPACITY 16
#define MAX_TOKEN_LENGTH 100
#define BUFFER_GROWTH_FACTOR 2
#define CONFIG_FILE "tokenizer_config.json"

TokenizerResult tokenize_input(const char* input) {
    TokenizerResult result = {0};
    
    if (!input || strlen(input) == 0) {
        result.error = TOKEN_NULL_INPUT;
        return result;
    }

    char processed_input[MAX_INPUT_LENGTH];
    strncpy(processed_input, input, MAX_INPUT_LENGTH - 1);
    processed_input[MAX_INPUT_LENGTH - 1] = '\0';

    result = tokenizeQuery(processed_input);

    if (result.error != TOKEN_SUCCESS) {
        return result;
    }

    return result;
}

ComputationResult calculate_from_tokens(TokenizerResult* tokens) {
    ComputationResult final_result = {0};
    TokenizerResult* shunt_yard_result = NULL;
    ParseResult* ast_root = NULL;

    shunt_yard_result = shunt_yard_algo(tokens);
    if (!shunt_yard_result || shunt_yard_result->error != TOKEN_SUCCESS) {
        final_result.error = COMPUTATION_MEMORY_ERROR;
        free(shunt_yard_result);
        return final_result;
    }

    ast_root = parse_expression(shunt_yard_result);
    if (!ast_root) {
        cleanup_tokens(shunt_yard_result->tokens, shunt_yard_result->token_count);
        free(shunt_yard_result);
        final_result.error = COMPUTATION_MEMORY_ERROR;
        return final_result;
    }

    final_result = compute_ast(ast_root);

    cleanup_tokens(shunt_yard_result->tokens, shunt_yard_result->token_count);
    cleanup_ast(ast_root);
    free(shunt_yard_result);

    return final_result;
}

void process_custom_input() {
    char input[MAX_INPUT_LENGTH];
    TokenizerResult token_result;
    ComputationResult calc_result;

    fprintf(stderr, "\nEnter an expression to tokenize (or '' to return to menu):\n> ");

    while (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
        }

        if (strncmp(input, "\\quit", 5) == 0) {
            break;
        }
        if (strncmp(input, "\\clear", 6) == 0) {
            system("clear");
            fprintf(stderr, "\nEnter another expression (or 'quit' to return to menu):\n> ");
            continue;
        }
        else if (strncmp(input, "\\version", 8) == 0) {
            fprintf(stderr, "Scientific Calculator Version 1.0\n");
            fprintf(stderr, "\nEnter another expression (or 'quit' to return to menu):\n> ");
            continue;
        }
        char *equals_pos = strchr(input, '='); 
        token_result = tokenize_input(input);
        if (token_result.error != TOKEN_SUCCESS) {
            fprintf(stderr, "Error tokenizing input: ");
            continue;
        }
        if(equals_pos != NULL && (token_result.tokens[0].type != TOKEN_VARIABLE || (token_result.tokens[0].type == TOKEN_VARIABLE && token_result.tokens[1].type != TOKEN_EQUALITY  ) )  ) {
            cleanup_tokens(token_result.tokens, token_result.token_count);
            token_result.error = TOKEN_INVALID_INPUT;
            fprintf(stderr, "Invalid input \"VAR = VALUE\" is the kind of supported input for computation or variables \n");
            fprintf(stderr, "\nEnter another expression (or 'quit' to return to menu):\n> ");
            continue;
        }
        else {
            fprintf(stderr, "\nTokenization result:\n");
            for (size_t i = 0; i < token_result.token_count; i++) {
                fprintf(stderr, "Token %zu: ", i + 1);
                print_token(&token_result.tokens[i]);
            }
            
            calc_result = calculate_from_tokens(&token_result);
            
            if (calc_result.error == COMPUTATION_OK) {
                fprintf(stderr, "\nResult: %f\n", calc_result.value);
            } else {
                fprintf(stderr, "\nComputation error occurred\n");
            }

            cleanup_tokens(token_result.tokens, token_result.token_count);
            
            fprintf(stderr, "\nEnter another expression (or 'quit' to return to menu):\n> ");
        }
    }
}

hashset_t* SUPPORTED_FUNCTIONS;
hashset_t* init_SUPPORTED_FUNCTIONS_() {
    hashset_t* set = hashset_create();
    if (!set) {
        fprintf(stderr, "Failed to create supported functions set\n");
        return NULL;
    }
    hashset_add(set, "sin",1);
    hashset_add(set, "cos",1);
    hashset_add(set, "tan",1);
    hashset_add(set, "logbase",2);
    hashset_add(set, "log",1);
    hashset_add(set, "sqrt",1);
    hashset_add(set, "exp",1);
    hashset_add(set, "abs",1);
    hashset_add(set, "log10",1);
    hashset_add(set, "log2",1);
    hashset_add(set, "loge",1);
    return set;
}

hashmapconst_t* VARIABLES;
hashmapconst_t* init_VARIABLES_() {
    hashmapconst_t* map = hashmapconst_create();
    if (!map) {
        fprintf(stderr, "Failed to create pre-stored variables map\n");
        return NULL;
    }
    hashmapconst_add(map, "e", 2.71828f);
    hashmapconst_add(map, "pi", 3.14159f);
    return map;
}

int main() {
    SUPPORTED_FUNCTIONS = init_SUPPORTED_FUNCTIONS_();
    if (!SUPPORTED_FUNCTIONS) {
        fprintf(stderr, "Initialization of supported functions failed\n");
        return 1;
    }

    VARIABLES = init_VARIABLES_();
    if(!VARIABLES) {
        fprintf(stderr, "Inititalisation of variables failed");
        return 1;
    }

    process_custom_input();

    hashset_destroy(SUPPORTED_FUNCTIONS);
    hashmapconst_destroy(VARIABLES);

    return 0;
}
