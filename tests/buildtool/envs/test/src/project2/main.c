#include <stdio.h>
#include "calculator/calculate.h"

int main() {
    double number1;
    double number2;
    char operator;
    printf("Enter your first number: ");
    scanf("%lf", &number1);

    printf("\nEnter a operator: ");
    scanf(" %c", &operator);

    printf("\nEnter your second number: ");
    scanf("%lf", &number2);

    printf("\n\n");

    double result = calculate(operator, number1, number2);

    printf("%lf %c %lf = %lf\n", number1, operator, number2, result);

    return 0;
}