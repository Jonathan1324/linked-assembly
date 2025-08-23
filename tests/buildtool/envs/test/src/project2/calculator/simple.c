#include "simple.h"

double simplecalculate(char op, double n1, double n2) {
    double result;
    if(op == '+'){
        result = n1 + n2;
    } else if(op == '-'){
        result = n1 - n2;
    } else if(op == '*' || op == 'x' || op == 'X'){
        result = n1 * n2;
    } else if(op == '/' || op == ':'){
        result = n1 / n2;
    }
    
    return result;
}