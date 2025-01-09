#ifndef SHUNT_YARD_ALGO_H
#define SHUNT_YARD_ALGO_H

#include "tokenizer.h"
#include "precidence.h"

/**
 * @brief Converts infix notation to postfix notation using Shunting Yard algorithm
 * 
 * @param tokens Input tokens in infix notation
 * @return TokenizerResult* Output tokens in postfix notation, or NULL if error
 */
TokenizerResult* shunt_yard_algo(TokenizerResult* tokens);

#endif /* SHUNT_YARD_ALGO_H */