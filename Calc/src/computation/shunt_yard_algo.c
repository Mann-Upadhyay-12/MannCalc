#include "../../include/computation/tokenizer.h"
#include "../../include/computation/precidence.h"
#include "../../include/computation/shunt_yard_algo.h"

TokenizerResult* shunt_yard_algo(TokenizerResult* input_tokens)
{
    TokenizerResult *result = {0};
    if (!input_tokens || !input_tokens->tokens) return NULL;

    HashMap* precedence_map = innit_precidence(NULL);
    if (!precedence_map) return NULL;

    Token* operator_stack = (Token*)malloc(sizeof(Token) * input_tokens->token_count);
    Token* output_queue = (Token*)malloc(sizeof(Token) * input_tokens->token_count);
    if (!operator_stack || !output_queue) {
        free(operator_stack);
        free(output_queue);
        free_hashmap(precedence_map);
        return NULL;
    }

    int stack_size = 0;
    int queue_size = 0;
    
    for(size_t i = 0; i < input_tokens->token_count; i++) {
        Token current_token = input_tokens->tokens[i];
        
        switch(current_token.type) {
            case TOKEN_NUMBER:
            case TOKEN_VARIABLE:
                output_queue[queue_size++] = current_token;
                break;
                
            case TOKEN_UNARY:
            case TOKEN_FUNCTION:
                operator_stack[stack_size++] = current_token;
                break;
                
            case TOKEN_OPERATOR: {
                char current_op_str[2] = {current_token.data.operator_value, '\0'};
                
                while(stack_size > 0) {
                    Token stack_top = operator_stack[stack_size - 1];
                    if (stack_top.type == TOKEN_PARENTHESIS && stack_top.data.parenthesis == '(') {
                        break;
                    }
                    
                    if (stack_top.type != TOKEN_OPERATOR) {
                        break;
                    }

                    char stack_op_str[2] = {stack_top.data.operator_value, '\0'};
                    int current_precedence = return_precidence(precedence_map, current_op_str);
                    int stack_precedence = return_precidence(precedence_map, stack_op_str);
                    
                    Operator* current_op = precidence_hashmap_get(precedence_map, current_op_str);
                    if (!current_op || (stack_precedence > current_precedence || 
                        (current_precedence == stack_precedence && current_op->assoc == LEFT_TO_RIGHT))) {
                        output_queue[queue_size++] = operator_stack[--stack_size];
                    } else {
                        break;
                    }
                }
                operator_stack[stack_size++] = current_token;
                break;
            }

            case TOKEN_PARENTHESIS:{
                if(current_token.data.parenthesis == '(') {
                    operator_stack[stack_size++] = current_token;
                }
                else if(current_token.data.parenthesis == ')') {
                    bool found_left_paren = false;
                    while(stack_size > 0) {
                        Token stack_top = operator_stack[--stack_size];
                        if(stack_top.type == TOKEN_PARENTHESIS && stack_top.data.parenthesis == '(') {
                            found_left_paren = true;
                            break;
                        }
                        output_queue[queue_size++] = stack_top;
                    }
                    
                    if (!found_left_paren) {
                        free(operator_stack);
                        free(output_queue);
                        free_hashmap(precedence_map);
                        return NULL;
                    }

                    if(stack_size > 0 && operator_stack[stack_size-1].type == TOKEN_FUNCTION) {
                        output_queue[queue_size++] = operator_stack[--stack_size];
                    }
                }
                break;
            }

            case TOKEN_EQUALITY:
                while (stack_size > 0) {
                    Token stack_top = operator_stack[stack_size - 1];
                    if (stack_top.type == TOKEN_PARENTHESIS) break;
                    output_queue[queue_size++] = operator_stack[--stack_size];
                }
                operator_stack[stack_size++] = current_token;
                break;

            default:
                break;
        }
    }

    while(stack_size > 0) {
        Token stack_top = operator_stack[--stack_size];
        if(stack_top.type == TOKEN_PARENTHESIS) {
            free(operator_stack);
            free(output_queue);
            free_hashmap(precedence_map);
            return NULL;
        }
        output_queue[queue_size++] = stack_top;
    }

    free(operator_stack);
    free_hashmap(precedence_map);

    Token* final_output = (Token*)malloc(sizeof(Token) * queue_size);
    if (!final_output) {
        free(output_queue);
        return NULL;
    }
    memcpy(final_output, output_queue, sizeof(Token) * queue_size);
    free(output_queue);
    
    result = (TokenizerResult*)malloc(sizeof(TokenizerResult)) ;
    result->tokens = final_output;
    result->token_count = queue_size;
    result->error = TOKEN_SUCCESS;
    return result;
}
