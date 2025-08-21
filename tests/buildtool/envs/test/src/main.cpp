#include <iostream>
#include <stack>
#include <queue>
#include <sstream>
#include <cctype>
#include <map>
#include <stdexcept>
#include "rpn.h"

using namespace std;

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// Convert infix to space-separated RPN string
string shuntingYard(const string& expr) {
    stack<char> ops;
    ostringstream output;
    istringstream iss(expr);
    char token;

    while (iss >> token) {
        if (isdigit(token) || token == '.') {
            string number(1, token);
            while (iss.peek() != EOF && (isdigit(iss.peek()) || iss.peek() == '.')) {
                number += iss.get();
            }
            output << number << ' ';
        } else if (isOperator(token)) {
            while (!ops.empty() && isOperator(ops.top()) &&
                   precedence(ops.top()) >= precedence(token)) {
                output << ops.top() << ' ';
                ops.pop();
            }
            ops.push(token);
        } else if (token == '(') {
            ops.push(token);
        } else if (token == ')') {
            while (!ops.empty() && ops.top() != '(') {
                output << ops.top() << ' ';
                ops.pop();
            }
            if (!ops.empty() && ops.top() == '(') ops.pop();
            else throw runtime_error("Mismatched parentheses");
        } else if (!isspace(token)) {
            throw runtime_error(string("Unknown token: ") + token);
        }
    }

    while (!ops.empty()) {
        if (ops.top() == '(' || ops.top() == ')') throw runtime_error("Mismatched parentheses");
        output << ops.top() << ' ';
        ops.pop();
    }

    return output.str();
}

// ------------------- Main -------------------
int main() {
    string expr;
    cout << "Enter a mathematical expression: ";
    getline(cin, expr);

    try {
        string rpn = shuntingYard(expr);
        double result = evaluateRPN(rpn.c_str());
        cout << "Result: " << result << endl;
    } catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}