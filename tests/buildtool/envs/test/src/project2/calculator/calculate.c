#include "calculate.h"
#include "simple.h"

double calculate(char operator, double n1, double n2) {
    if(operator == '+' || operator == '-' || operator == '*' || operator == 'x' || operator == 'X' || operator == '/' || operator == ':'){
        return simplecalculate(operator, n1, n2);
    }

    return 0;
}