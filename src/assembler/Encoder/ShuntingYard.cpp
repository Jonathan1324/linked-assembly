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

ShuntingYard::PreparedTokens ShuntingYard::prepareTokens(
        const std::vector<Parser::ImmediateOperand>& operands,
        const std::unordered_map<std::string, Encoder::Label>& labels,
        const std::unordered_map<std::string, Encoder::Constant>& constants,
        uint64_t bytesWritten,
        uint64_t sectionOffset,
        const std::string* currentSection
    )
{
    PreparedTokens output;
    output.relocationPossible = true;

    std::vector<Token> outputQueue;
    std::stack<std::string> operatorStack;

    bool expectUnaryMinus = false;

    const std::string* usedSection;
    bool useSection = false;

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
                    throw Exception::SyntaxError("Mismatched parentheses", -1, -1); // FIXME: add line and column
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
        else if (std::holds_alternative<Parser::Integer>(op))
        {
            Int128 val = static_cast<Int128>(std::get<Parser::Integer>(op).value);
            if (expectUnaryMinus)
            {
                val = -val;
                expectUnaryMinus = false;
            }
            outputQueue.emplace_back(val);
        }
        else if (std::holds_alternative<Parser::String>(op))
        {
            const std::string& name = std::get<Parser::String>(op).value;
            if (auto it = labels.find(name); it != labels.end())
            {
                if (useSection && it->second.section.compare(*usedSection) != 0)
                {
                    output.relocationPossible = false;
                }
                Token token;
                token.type = Token::Type::Position;
                token.offset = it->second.offset;
                if (expectUnaryMinus)
                {
                    token.negative = true;
                    expectUnaryMinus = false;
                }
                outputQueue.push_back(std::move(token));
                usedSection = &it->second.section;
                useSection = true;
            }
            else if (auto it = constants.find(name); it != constants.end())
            {
                const Encoder::Constant& c = it->second;
                if (useSection && c.usedSection.compare(*usedSection) != 0 && c.hasPos == Encoder::HasPos::TRUE || !it->second.relocationPossible)
                {
                    output.relocationPossible = false;
                }
                if (!c.resolved)
                    throw Exception::InternalError("Unresolved constants '" + name + "' used in expression", -1, -1);

                if (c.useOffset)
                {
                    Token token;
                    token.type = Token::Type::Position;
                    token.offset = c.off;
                    if (expectUnaryMinus)
                    {
                        token.negative = true;
                        expectUnaryMinus = false;
                    }
                    outputQueue.push_back(std::move(token));
                    usedSection = &c.usedSection;
                    useSection = true;
                }
                else
                {
                    Int128 val = static_cast<Int128>(c.value);
                    if (expectUnaryMinus)
                    {
                        val = -val;
                        expectUnaryMinus = false;
                    }
                    outputQueue.emplace_back(val);
                }
            }
            else throw Exception::InternalError("Unknown string '" + name + "'", -1, -1);
        }
        else if (std::holds_alternative<Parser::CurrentPosition>(op))
        {
            const Parser::CurrentPosition& curPos = std::get<Parser::CurrentPosition>(op);
            Token token;
            token.type = Token::Type::Position;
            token.offset = curPos.sectionPos ? 0 : sectionOffset;
            if (expectUnaryMinus)
            {
                token.negative = true;
                expectUnaryMinus = false;
            }
            outputQueue.push_back(std::move(token));
            if (useSection && currentSection->compare(*usedSection) != 0)
            {
                output.relocationPossible = false;
            }
            usedSection = currentSection;
            useSection = true;
        }
        else
        {
            throw Exception::InternalError("Expected value operand", -1, -1);
        }
    }

    // put rest in operatorStack to outputQueue
    while (!operatorStack.empty())
    {
        if (operatorStack.top() == "(" || operatorStack.top() == ")")
            throw Exception::SyntaxError("Mismatched parentheses", -1, -1); // FIXME: add line and column
        outputQueue.emplace_back(operatorStack.top());
        operatorStack.pop();
    }

    output.tokens = outputQueue;
    if (useSection)
        output.usedSection = *usedSection;
    else
        output.usedSection = *currentSection;

    return output;
}

Int128 ShuntingYard::evaluate(const std::vector<ShuntingYard::Token>& tokens, uint64_t offset)
{
    std::stack<Int128> stack;

    for (const auto& token : tokens)
    {
        if (token.type == Token::Type::Number)
            stack.push(token.number);
        else if (token.type == Token::Type::Position)
            stack.push((token.negative ? -token.offset : token.offset) + offset);
        else if (token.type == Token::Type::Operator)
        {
            if (stack.size() < 2)
                throw Exception::InternalError("Invalid expression: not enough operands", -1, -1);

            Int128 rhs = stack.top(); stack.pop();
            Int128 lhs = stack.top(); stack.pop();

            if (token.op == "+") stack.push(lhs + rhs);
            else if (token.op == "-") stack.push(lhs - rhs);
            else if (token.op == "*") stack.push(lhs * rhs);
            else if (token.op == "/") {
                if (rhs == 0)
                    throw Exception::SemanticError("Division by zero", -1, -1);
                stack.push(lhs / rhs);
            }
            else if (token.op == "%") {
                if (rhs == 0)
                    throw Exception::SemanticError("Modulo by zero", -1, -1);
                stack.push(lhs % rhs);
            }
            else
                throw Exception::InternalError("Unknown operator: " + token.op, -1, -1);
        }
        else
            throw Exception::InternalError("Unknown token type", -1, -1);
    }

    if (stack.size() != 1)
        throw Exception::SyntaxError("Invalid expression", -1, -1);

    return stack.top();
}