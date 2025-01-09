# MannCalc
# Mathematical Expression Parser and Evaluator

A robust C program that parses, evaluates, and visualizes mathematical expressions using Abstract Syntax Trees (ASTs).

## Features

- **Expression Parsing**: Handles complex mathematical expressions with:
  - Basic arithmetic operations (+, -, *, /, ^)
  - Parentheses for precedence control
  - Unary operators (positive/negative)
  - Variables and functions
  - Equality comparisons
  - This is NOT a case sensitive program.

- **AST Visualization**: Generates a clear tree visualization of the parsed expression, showing the hierarchical relationship between operators, operands, and functions.

- **Variable Management**: Supports variable declaration and usage within expressions through an efficient hashmap implementation. The syntax for VAR = EXPRESSION is supported. this VAR can be used in next equation, with same syntax. 

- **Error Handling**: Robust error detection and reporting for:
  - Invalid syntax
  - Undefined variables
  - Malformed expressions
  - Memory allocation failures

