#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "rpn.h"

#define MAX_STACK 100

typedef struct {
    double data[MAX_STACK];
    int top;
} Stack;

void push(Stack* s, double value) {
    if (s->top >= MAX_STACK - 1) {
        printf("Stack overflow\n");
        exit(1);
    }
    s->data[++s->top] = value;
}

double pop(Stack* s) {
    if (s->top < 0) {
        printf("Stack underflow\n");
        exit(1);
    }
    return s->data[s->top--];
}

// Evaluate a space-separated RPN expression
double evaluateRPN(const char* expr) {
    Stack s;
    s.top = -1;
    
    char buffer[256];
    strcpy(buffer, expr);

    char* token = strtok(buffer, " ");
    while (token != NULL) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            push(&s, atof(token));
        } else if (strlen(token) == 1 && strchr("+-*/", token[0])) {
            double b = pop(&s);
            double a = pop(&s);
            switch (token[0]) {
                case '+': push(&s, a + b); break;
                case '-': push(&s, a - b); break;
                case '*': push(&s, a * b); break;
                case '/': 
                    if (b == 0) { printf("Division by zero\n"); exit(1); }
                    push(&s, a / b); 
                    break;
            }
        } else {
            printf("Unknown token: %s\n", token);
            exit(1);
        }
        token = strtok(NULL, " ");
    }

    if (s.top != 0) {
        printf("Invalid RPN expression\n");
        exit(1);
    }

    return pop(&s);
}
