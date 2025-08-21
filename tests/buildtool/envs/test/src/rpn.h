#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Evaluate a space-separated RPN (Reverse Polish Notation) expression.
 * 
 * @param expr: A null-terminated string containing the RPN expression.
 *              Tokens must be separated by spaces, e.g. "3 4 + 2 *".
 * @return The computed result as a double.
 * @note Exits with error if expression is invalid or division by zero occurs.
 */
double evaluateRPN(const char* expr);

#ifdef __cplusplus
}
#endif