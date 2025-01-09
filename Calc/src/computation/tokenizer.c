#include "../../include/computation/tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../../include/computation/tokenizer.h"
#include "../../include/datastructures/hashset.h"
#include "../../include/datastructures/hashmapforconst.h"
#include "../../include/computation/shunt_yard_algo.h"
#include "../../include/computation/AST_tree.h"
#include <stdbool.h>
#include "../../include/computation/computation.h"

#define MAX_INPUT_LENGTH 1024
#define INITIAL_TOKEN_CAPACITY 16
#define MAX_TOKEN_LENGTH 100
#define BUFFER_GROWTH_FACTOR 2
#define CONFIG_FILE "tokenizer_config.json"

typedef struct {
    const char** supported_functions;
    size_t function_count;
    const char** operators;
    size_t operator_count;
} TokenizerConfig;

static TokenizerConfig config = {0};

static int load_config(void);

static int load_config(void) {
    static const char* default_functions[] = {
        "sin", "cos", "tan", "log", "sqrt", "exp", "abs"
    };
    static const char* default_operators = "+-*/^%";
    
    config.supported_functions = default_functions;
    config.function_count = sizeof(default_functions) / sizeof(char*);
    config.operators = &default_operators;
    config.operator_count = strlen(default_operators);
    
    return 0;
}

TokenizerError replace_characters(const char *input, char *output, size_t output_size) {
    if (!input || !output || output_size == 0) {
        return TOKEN_NULL_INPUT;
    }

    static const char *special_chars = "()+-*/=^!<>{}[]&|,%";
    static char lookup[256] = {0};
    static int initialized = 0;

    if (!initialized) {
        for (const char *c = special_chars; *c; c++) {
            lookup[(unsigned char)*c] = 1;
        }
        initialized = 1;
    }

    size_t input_len = strlen(input);
    size_t j = 0;

    if (input_len * 3 + 1 > output_size) {
        return TOKEN_INPUT_TOO_LONG;
    }

    for (size_t i = 0; i < input_len && j < output_size - 1; i++) {
        unsigned char current = input[i];
        
        if (lookup[current]) {
            if (j + 3 > output_size - 1) return TOKEN_INPUT_TOO_LONG;
            output[j++] = ' ';
            output[j++] = current;
            output[j++] = ' ';
        } else {
            if (j + 1 > output_size - 1) return TOKEN_INPUT_TOO_LONG;
            output[j++] = current;
        }
    }
    output[j] = '\0';
    return TOKEN_SUCCESS;
}

TokenizerError to_lowercase(char *str, size_t max_len) {
    if (!str) return TOKEN_NULL_INPUT;
    
    size_t len = 0;
    for (char *p = str; *p && len < max_len; p++, len++) {
        *p = tolower((unsigned char)*p);
    }
    
    return (len == max_len && str[len] != '\0') ? TOKEN_INPUT_TOO_LONG : TOKEN_SUCCESS;
}

void free_token_array(TokenArrChar *token_array) {
    if (!token_array || !token_array->tokens) {
        return;
    }
    
    for (size_t i = 0; i < token_array->size; i++) {
        free(token_array->tokens[i]);
    }
    
    free(token_array->tokens);
    token_array->tokens = NULL;
    token_array->size = 0;
}

bool is_valid_function(hashset_t* set, const char *str) {
    if (!set || !str) return false;
    
    char *lower_str = strdup(str);
    if (!lower_str) return false;
    
    to_lowercase(lower_str, strlen(str));
    bool result = hashset_contains(set, lower_str);
    free(lower_str);
    
    return result;
}

int has_var = 0;
Token* tokenize(const char *str, const char *delim, size_t *token_count, TokenizerError *error) {
    if (!str ||!delim || !token_count) {
        if (error) *error = TOKEN_NULL_INPUT;
        return NULL;
    }

    *token_count = 0;

    TokenArrChar temp_tokens = { NULL, 0 };
    char *str_copy = strdup(str);
    if (!str_copy) {
        if (error) *error = TOKEN_MEMORY_ERROR;
        return NULL;
    }

    temp_tokens.tokens = malloc(INITIAL_TOKEN_CAPACITY * sizeof(char*));
    if (!temp_tokens.tokens) {
        free(str_copy);
        if (error) *error = TOKEN_MEMORY_ERROR;
        return NULL;
    }

    size_t capacity = INITIAL_TOKEN_CAPACITY;
    
    char *token = strtok(str_copy, delim);
    while (token) {
        if (temp_tokens.size >= capacity) {
            capacity *= BUFFER_GROWTH_FACTOR;
            char **new_tokens = realloc(temp_tokens.tokens, capacity * sizeof(char*));
            if (!new_tokens) {
                free_token_array(&temp_tokens);
                free(str_copy);
                if (error) *error = TOKEN_MEMORY_ERROR;
                return NULL;
            }
            temp_tokens.tokens = new_tokens;
        }

        temp_tokens.tokens[temp_tokens.size] = strdup(token);
        if (!temp_tokens.tokens[temp_tokens.size]) {
            free_token_array(&temp_tokens);
            free(str_copy);
            if (error) *error = TOKEN_MEMORY_ERROR;
            return NULL;
        }

        temp_tokens.size++;
        token = strtok(NULL, delim);
    }

    Token* output_array = malloc(sizeof(Token) * temp_tokens.size);
    if (!output_array) {
        free_token_array(&temp_tokens);
        free(str_copy);
        if (error) *error = TOKEN_MEMORY_ERROR;
        return NULL;
    }
    int bracl = 0;
    int bracr = 0;

    for (size_t i = 0; i < temp_tokens.size; i++) {
        const char* current = temp_tokens.tokens[i];
        
        if (strlen(current) == 1) {
            char c = current[0];
            
            if (strchr(OPERATORS, c)) {
                if(((output_array[(*token_count)-1].type ==TOKEN_PARENTHESIS && output_array[(*token_count)-1].data.parenthesis=='(') || (output_array[(*token_count)-1].type !=TOKEN_PARENTHESIS)) && (output_array[(*token_count)-1].type !=TOKEN_NUMBER && output_array[(*token_count)-1].type !=TOKEN_PARENTHESIS) && output_array[(*token_count)-1].type !=TOKEN_VARIABLE)
                {
                    if(c=='-')
                        {
                            output_array[*token_count].type = TOKEN_UNARY;
                            output_array[*token_count].data.unary_operator = TOKEN_UNARY_NEGATIVE;
                        }
                    else if(c=='+')
                        {
                            output_array[*token_count].type = TOKEN_UNARY;
                            output_array[*token_count].data.unary_operator = TOKEN_UNARY_POSITIVE;
                        }
                }
                else
                {
                    output_array[*token_count].type = TOKEN_OPERATOR;
                    output_array[*token_count].data.operator_value = c;
                }
                (*token_count)++;
                continue;
            }
            
            if (c == '(' || c == ')') {
                output_array[*token_count].type = TOKEN_PARENTHESIS;
                output_array[*token_count].data.parenthesis = c;
                (*token_count)++;
                if(c == '(')
                {
                    bracl++;
                }
                else if(c == ')')
                {
                    bracr++;
                }
                continue;
            }
            
            if (c == '=') {
                output_array[*token_count].type = TOKEN_EQUALITY;
                output_array[*token_count].data.equality = c;
                (*token_count)++;
                continue;
            }
            if (c == ',') {
                output_array[*token_count].type = TOKEN_COMMA;
                output_array[*token_count].data.comma = c;
                (*token_count)++;
                continue;
            }
        }

        char* endptr;
        double num_val = strtod(current, &endptr);
        
        if (*endptr == '\0') {
            output_array[*token_count].type = TOKEN_NUMBER;
            output_array[*token_count].data.num_value = num_val;
            (*token_count)++;
            continue;
        }

        if (is_valid_function(SUPPORTED_FUNCTIONS,current)) {
            output_array[*token_count].type = TOKEN_FUNCTION;
            struct hashset_entry* func_entry = hashset_get_entry(SUPPORTED_FUNCTIONS, current);
            if (!func_entry) {
                hashset_add(SUPPORTED_FUNCTIONS, current, 1);
                func_entry = hashset_get_entry(SUPPORTED_FUNCTIONS, current);
            }
            output_array[*token_count].data.function_name = func_entry;
            (*token_count)++;
            continue;
        }

        if(hashmapconst_contains(VARIABLES,current))
        {
            output_array[*token_count].type = TOKEN_NUMBER;
            output_array[*token_count].data.num_value = hashmapconst_get_value(VARIABLES,current);
        }
        else{
            has_var += 1;
            output_array[*token_count].type = TOKEN_VARIABLE;
            struct hashmapconst_entry* var_entry = hashmapconst_get_entry(VARIABLES, current);
            if (!var_entry) {
                hashmapconst_add(VARIABLES, current, 0.0);
                var_entry = hashmapconst_get_entry(VARIABLES, current);
            }
            output_array[*token_count].data.var_name = var_entry;
        }

        (*token_count)++;
    }
    if(bracl != bracr)
    {
        free_token_array(&temp_tokens);
        free(str_copy);
        if (error) *error = TOKEN_INVALID_INPUT;
        return output_array;
    }

    free_token_array(&temp_tokens);
    free(str_copy);
    
    if (error) *error = TOKEN_SUCCESS;
    return output_array;
}

TokenizerResult tokenizeQuery(const char *input_string) {
    TokenizerResult result = {NULL, 0, TOKEN_SUCCESS};
    
    if (!input_string) {
        result.error = TOKEN_NULL_INPUT;
        return result;
    }

    if (strlen(input_string) > MAX_INPUT_LENGTH) {
        result.error = TOKEN_INPUT_TOO_LONG;
        return result;
    }

    char processed_string[MAX_INPUT_LENGTH * 3];
    result.error = replace_characters(input_string, processed_string, sizeof(processed_string));
    if (result.error != TOKEN_SUCCESS) {
        return result;
    }

    to_lowercase(processed_string, strlen(processed_string));
    
    size_t len = strlen(processed_string);
    if (len > 0 && processed_string[len - 1] == '\n') {
        processed_string[len - 1] = '\0';
    }

    result.tokens = tokenize(processed_string, " ", &result.token_count, &result.error);
    result.num_vars = has_var;
    return result;
}

void print_token(const Token* token) {
    if (!token) return;
    
    switch (token->type) {
        case TOKEN_NUMBER:
            fprintf(stderr, "NUMBER: %f\n", token->data.num_value);
            break;
        case TOKEN_OPERATOR:
            fprintf(stderr, "OPERATOR: %c\n", token->data.operator_value);
            break;
        case TOKEN_UNARY:
            switch (token->data.unary_operator) {
                case 0:
                    fprintf(stderr, "UNARY: %s\n","NEG");
                    break;
                case 1:
                    fprintf(stderr, "UNARY: %s\n","POS");                 
                    break;
                default:
                    break;
            }
            break;
        case TOKEN_FUNCTION:
            fprintf(stderr, "FUNCTION: %s\n", token->data.function_name->value);
            break;
        case TOKEN_VARIABLE:
            fprintf(stderr, "VARIABLE: %s\n", token->data.var_name->name);
            break;
        case TOKEN_PARENTHESIS:
            fprintf(stderr, "PARENTHESIS: %c\n", token->data.parenthesis);
            break;
        case TOKEN_EQUALITY:
            fprintf(stderr, "EQUALITY: %c\n", token->data.equality);
            break;
        case TOKEN_COMMA:
            fprintf(stderr, "COMMA: %c\n",token->data.comma);
            break;
        default:
            fprintf(stderr, "UNKNOWN TOKEN TYPE\n");
    }
}

void cleanup_tokens(Token* tokens) {
    if (!tokens) {
        return;
    }

    free(tokens);
}
