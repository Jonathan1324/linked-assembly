#include "ShuntingYard.hpp"

#include <stack>

static int precedence(const std::string& op)
{
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/" || op == "%") return 2;
    if (op == "^") return 3;
    return 0;
}

static bool isLeftAssociative(const std::string& op)
{
    return op != "^";
}

std::vector<ShuntingYard::Token> ShuntingYard::prepareTokens(
        const std::vector<Parser::ImmediateOperand>& operands,
        const std::unordered_map<std::string, Encoder::Label>& labels,
        const std::unordered_map<std::string, Encoder::Constant>& constants,
        uint64_t bytesWritten,
        uint64_t sectionOffset
    )
{
    std::vector<Token> outputQueue;
    std::stack<std::string> operatorStack;

    auto getValue = [&](const Parser::ImmediateOperand& op) -> Int128
    {
        if (std::holds_alternative<Parser::Integer>(op))
        {
            return static_cast<Int128>(std::get<Parser::Integer>(op).value);
        }
        else if (std::holds_alternative<Parser::String>(op))
        {
            const std::string& name = std::get<Parser::String>(op).value;
            if (auto it = labels.find(name); it != labels.end())
                return static_cast<Int128>(it->second.offset);
            if (auto it = constants.find(name); it != constants.end())
            {
                if (!it->second.resolved)
                    throw Exception::InternalError("Unresolved constants '" + name + "' used in expression");
                return static_cast<Int128>(it->second.value);
            }
            throw Exception::InternalError("Unknown string '" + name + "'");
        }
        else if (std::holds_alternative<Parser::CurrentPosition>(op))
        {
            const Parser::CurrentPosition& curPos = std::get<Parser::CurrentPosition>(op);
            return curPos.sectionPos ? static_cast<Int128>(sectionOffset) : static_cast<Int128>(bytesWritten);
        }
        else
        {
            throw Exception::InternalError("Expected value operand");
        }
    };

    bool expectUnaryMinus = false;

    for (size_t i = 0; i < operands.size(); i++)
    {
        const auto& op = operands[i];

        if (std::holds_alternative<Parser::Operator>(op))
        {
            std::string opStr = std::get<Parser::Operator>(op).op;

            if (opStr == "-" && (
                i == 0 ||
                (std::holds_alternative<Parser::Operator>(operands[i - 1]) && std::get<Parser::Operator>(operands[i - 1]).op != ")")
            ))
            {
                expectUnaryMinus = true;
            }
            else if (opStr == "(")
            {
                operatorStack.push(opStr);
            }
            else if (opStr == ")")
            {
                while (!operatorStack.empty() && operatorStack.top() != "(")
                {
                    outputQueue.emplace_back(operatorStack.top());
                    operatorStack.pop();
                }
                if (operatorStack.empty())
                    throw Exception::SyntaxError("Mismatched parentheses"); // FIXME: add line and column
                operatorStack.pop();
            }
            else
            {
                // other operator
                while (!operatorStack.empty())
                {
                    const std::string& topOp = operatorStack.top();
                    if (topOp == "(")
                        break;

                    int topPrec = precedence(topOp);
                    int currPrec = precedence(opStr);

                    if ((isLeftAssociative(opStr) && currPrec <= topPrec) ||
                        (!isLeftAssociative(opStr) && currPrec < topPrec))
                    {
                        outputQueue.emplace_back(topOp);
                        operatorStack.pop();
                    }
                    else
                    {
                        break;
                    }
                }
                operatorStack.push(opStr);
            }
        }
        /*else if (std::holds_alternative<Parser::CurrentPosition>(op))
        {
            const Parser::CurrentPosition& curPos = std::get<Parser::CurrentPosition>(op);
        }*/
        else
        {
            Int128 val = getValue(op);
            if (expectUnaryMinus)
            {
                val = -val;
                expectUnaryMinus = false;
            }
            outputQueue.emplace_back(val);
        }
    }

    // put rest in operatorStack to outputQueue
    while (!operatorStack.empty())
    {
        if (operatorStack.top() == "(" || operatorStack.top() == ")")
            throw Exception::SyntaxError("Mismatched parentheses"); // FIXME: add line and column
        outputQueue.emplace_back(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}

Int128 ShuntingYard::evaluate(const std::vector<ShuntingYard::Token>& tokens)
{
    std::stack<Int128> stack;

    for (const auto& token : tokens)
    {
        if (token.type == Token::Type::Number)
            stack.push(token.number);
        else if (token.type == Token::Type::Operator)
        {
            if (stack.size() < 2)
                throw Exception::InternalError("Invalid expression: not enough operands");

            Int128 rhs = stack.top(); stack.pop();
            Int128 lhs = stack.top(); stack.pop();

            if (token.op == "+") stack.push(lhs + rhs);
            else if (token.op == "-") stack.push(lhs - rhs);
            else if (token.op == "*") stack.push(lhs * rhs);
            else if (token.op == "/") {
                if (rhs == 0)
                    throw Exception::SemanticError("Division by zero");
                stack.push(lhs / rhs);
            }
            else if (token.op == "%") {
                if (rhs == 0)
                    throw Exception::SemanticError("Modulo by zero");
                stack.push(lhs % rhs);
            }
            else
                throw Exception::InternalError("Unknown operator: " + token.op);
        }
        else
            throw Exception::InternalError("Unknown token type");
    }

    if (stack.size() != 1)
        throw Exception::SyntaxError("Invalid expression");

    return stack.top();
}