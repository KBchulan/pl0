#include "../include/Parser.h"

#include <sstream>
#include <iostream>

namespace pl0
{

    std::unique_ptr<Program> Parser::parse()
    {
        try
        {
            return parseProgram();
        }
        catch (const std::exception &e)
        {
            addError(e.what());
            return nullptr;
        }
    }

    std::unique_ptr<Program> Parser::parseProgram()
    {
        auto block = parseBlock();
        [[maybe_unused]] auto period = consume(TokenType::PERIOD, "程序必须以'.'结束");
        return std::make_unique<Program>(std::move(block));
    }

    std::unique_ptr<Block> Parser::parseBlock()
    {
        auto constDecls = parseConstDeclarations();
        auto varDecls = parseVarDeclarations();
        auto procedures = parseProcedures();
        auto statement = parseStatement();

        return std::make_unique<Block>(
            std::move(constDecls),
            std::move(varDecls),
            std::move(procedures),
            std::move(statement));
    }

    std::vector<std::unique_ptr<ConstDeclaration>> Parser::parseConstDeclarations()
    {
        std::vector<std::unique_ptr<ConstDeclaration>> decls;

        if (match(TokenType::CONST))
        {
            do
            {
                auto name = consume(TokenType::IDENTIFIER, "常量声明需要标识符");
                [[maybe_unused]] auto eq = consume(TokenType::EQ, "常量声明需要'='");
                auto value = consume(TokenType::NUMBER, "常量声明需要数字");

                decls.push_back(std::make_unique<ConstDeclaration>(
                    name.value<std::string_view>(),
                    value.value<int64_t>()));
            } while (match(TokenType::COMMA));

            [[maybe_unused]] auto semi = consume(TokenType::SEMICOLON, "常量声明需要以';'结束");
        }

        return decls;
    }

    std::vector<std::unique_ptr<VarDeclaration>> Parser::parseVarDeclarations()
    {
        std::vector<std::unique_ptr<VarDeclaration>> decls;

        if (match(TokenType::VAR))
        {
            do
            {
                auto name = consume(TokenType::IDENTIFIER, "变量声明需要标识符");
                decls.push_back(std::make_unique<VarDeclaration>(
                    name.value<std::string_view>()));
            } while (match(TokenType::COMMA));

            [[maybe_unused]] auto semi = consume(TokenType::SEMICOLON, "变量声明需要以';'结束");
        }

        return decls;
    }

    std::vector<std::unique_ptr<ProcedureDeclaration>> Parser::parseProcedures()
    {
        std::vector<std::unique_ptr<ProcedureDeclaration>> procs;

        while (match(TokenType::PROCEDURE))
        {
            auto name = consume(TokenType::IDENTIFIER, "过程声明需要标识符");
            [[maybe_unused]] auto semi1 = consume(TokenType::SEMICOLON, "过程声明头部需要以';'结束");

            auto block = parseBlock();
            [[maybe_unused]] auto semi2 = consume(TokenType::SEMICOLON, "过程声明需要以';'结束");

            procs.push_back(std::make_unique<ProcedureDeclaration>(
                name.value<std::string_view>(),
                std::move(block)));
        }

        return procs;
    }

    std::unique_ptr<Statement> Parser::parseStatement()
    {
        auto token = peek();
        std::cerr << "解析语句,当前token: " << static_cast<int>(token.type());
        if (token.hasValue())
        {
            if (token.type() == TokenType::IDENTIFIER)
            {
                std::cerr << " (标识符: " << token.value<std::string_view>() << ")";
            }
        }
        std::cerr << '\n';

        switch (token.type())
        {
        case TokenType::IDENTIFIER:
            return parseAssignStatement();
        case TokenType::CALL:
            return parseCallStatement();
        case TokenType::BEGIN:
            return parseBeginStatement();
        case TokenType::IF:
            return parseIfStatement();
        case TokenType::WHILE:
            return parseWhileStatement();
        case TokenType::END:
        case TokenType::SEMICOLON:
            return nullptr; // 这些token标记语句的结束，不是错误
        default:
            if (token.type() == TokenType::ERROR)
            {
                error("词法错误");
            }
            else
            {
                error("预期语句，但得到: " + std::to_string(static_cast<int>(token.type())));
            }
            return nullptr;
        }
    }

    std::unique_ptr<Statement> Parser::parseAssignStatement()
    {
        auto name = consume(TokenType::IDENTIFIER, "赋值语句需要标识符").value<std::string_view>();
        [[maybe_unused]] auto assign = consume(TokenType::ASSIGN, "赋值语句需要':='");
        auto expr = parseExpression();
        return std::make_unique<AssignStatement>(name, std::move(expr));
    }

    std::unique_ptr<Statement> Parser::parseCallStatement()
    {
        [[maybe_unused]] auto call_token = advance(); // 消费CALL
        auto name = consume(TokenType::IDENTIFIER, "CALL语句需要过程名");
        return std::make_unique<CallStatement>(name.value<std::string_view>());
    }

    std::unique_ptr<Statement> Parser::parseBeginStatement()
    {
        [[maybe_unused]] auto begin_token = advance(); // 消费BEGIN
        std::vector<std::unique_ptr<Statement>> statements;

        while (!check(TokenType::END))
        {
            auto stmt = parseStatement();
            if (stmt)
            {
                statements.push_back(std::move(stmt));
            }

            if (!match(TokenType::SEMICOLON) && !check(TokenType::END))
            {
                error("语句之间需要分号");
            }
        }

        [[maybe_unused]] auto end_token = consume(TokenType::END, "BEGIN语句需要以END结束");
        return std::make_unique<BeginStatement>(std::move(statements));
    }

    std::unique_ptr<Statement> Parser::parseIfStatement()
    {
        [[maybe_unused]] auto if_token = advance(); // 消费IF
        auto condition = parseCondition();
        [[maybe_unused]] auto then = consume(TokenType::THEN, "IF语句需要THEN");
        auto then_stmt = parseStatement();

        // 检查是否有BEGIN-END块
        if (then_stmt && !dynamic_cast<BeginStatement *>(then_stmt.get()) && !check(TokenType::END) && !check(TokenType::SEMICOLON))
        {
            // 如果then_stmt不是BEGIN块，且后面不是END或分号，则需要将其包装在BEGIN-END块中
            std::vector<std::unique_ptr<Statement>> statements;
            statements.push_back(std::move(then_stmt));

            while (!check(TokenType::END) && !check(TokenType::SEMICOLON))
            {
                if (auto stmt = parseStatement())
                {
                    statements.push_back(std::move(stmt));
                }
                [[maybe_unused]] bool has_semi = match(TokenType::SEMICOLON); // 修复match()的[[nodiscard]]警告
            }

            then_stmt = std::make_unique<BeginStatement>(std::move(statements));
        }

        return std::make_unique<IfStatement>(std::move(condition), std::move(then_stmt));
    }

    std::unique_ptr<Statement> Parser::parseWhileStatement()
    {
        [[maybe_unused]] auto while_token = advance(); // 消费WHILE
        auto condition = parseCondition();
        [[maybe_unused]] auto do_token = consume(TokenType::DO, "WHILE语句需要DO");
        auto body = parseStatement();
        return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
    }

    std::unique_ptr<Expression> Parser::parseCondition()
    {
        if (check(TokenType::ODD))
        {
            [[maybe_unused]] auto token = advance(); // 消费ODD
            return parseExpression();
        }

        auto left = parseExpression();
        auto op = peek().type(); // 先获取运算符类型

        if (op != TokenType::EQ && op != TokenType::NEQ &&
            op != TokenType::LT && op != TokenType::LTE &&
            op != TokenType::GT && op != TokenType::GTE)
        {
            error("预期比较运算符");
        }

        [[maybe_unused]] auto token = advance(); // 消费运算符
        auto right = parseExpression();

        return std::make_unique<BinaryExpression>(
            std::move(left),
            tokenTypeToBinaryOp(op), // 使用已经获取的运算符类型
            std::move(right));
    }

    std::unique_ptr<Expression> Parser::parseExpression()
    {
        auto expr = parseTerm();

        while (check(TokenType::PLUS) || check(TokenType::MINUS))
        {
            auto op = tokenTypeToBinaryOp(peek().type());
            [[maybe_unused]] auto token = advance(); // 消费运算符
            auto right = parseTerm();
            expr = std::make_unique<BinaryExpression>(
                std::move(expr),
                op,
                std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::parseTerm()
    {
        auto expr = parsePower(); // 先解析幂运算

        while (check(TokenType::MULTIPLY) || check(TokenType::DIVIDE))
        {
            auto op = tokenTypeToBinaryOp(peek().type());
            [[maybe_unused]] auto token = advance(); // 消费运算符
            auto right = parsePower();               // 递归解析幂运算
            expr = std::make_unique<BinaryExpression>(
                std::move(expr),
                op,
                std::move(right));
        }

        return expr;
    }

    // 添加新的解析幂运算的方法
    std::unique_ptr<Expression> Parser::parsePower()
    {
        auto expr = parseFactor();

        while (check(TokenType::POWER))
        {
            [[maybe_unused]] auto token = advance();
            auto right = parsePower();
            expr = std::make_unique<BinaryExpression>(
                std::move(expr),
                BinaryExpression::Op::Pow,
                std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::parseFactor()
    {
        Token token; // 先声明token

        if (check(TokenType::NUMBER))
        {
            token = advance();
            try
            {
                return std::make_unique<NumberExpression>(token.value<int64_t>());
            }
            catch (const std::bad_variant_access &)
            {
                error("无效的数字");
                return nullptr;
            }
        }

        if (check(TokenType::IDENTIFIER))
        {
            token = advance();
            try
            {
                return std::make_unique<IdentifierExpression>(token.value<std::string_view>());
            }
            catch (const std::bad_variant_access &)
            {
                error("无效的标识符");
                return nullptr;
            }
        }

        if (check(TokenType::LPAREN))
        {
            [[maybe_unused]] auto lparen = advance();
            auto expr = parseExpression();
            [[maybe_unused]] auto rparen = consume(TokenType::RPAREN, "表达式需要右括号')'");
            return expr;
        }

        error("预期表达式");
        return nullptr;
    }

    bool Parser::match(TokenType type) noexcept
    {
        if (!check(type))
        {
            return false;
        }
        [[maybe_unused]] auto token = advance(); // 修复advance()的[[nodiscard]]警告
        return true;
    }

    bool Parser::check(TokenType type) noexcept
    {
        return peek().type() == type;
    }

    Token Parser::consume(TokenType type, const std::string &message)
    {
        if (!check(type))
        {
            error(message);
        }
        return advance();
    }

    void Parser::error(const std::string &message)
    {
        had_error_ = true;
        std::stringstream ss;
        ss << "行" << lexer_.line() << "列" << lexer_.column() << ": " << message;
        ss << "\n当前token: " << static_cast<int>(peek().type());
        if (peek().hasValue())
        {
            if (peek().type() == TokenType::NUMBER)
            {
                ss << " (数字: " << peek().value<int64_t>() << ")";
            }
            else if (peek().type() == TokenType::IDENTIFIER)
            {
                ss << " (标识符: " << peek().value<std::string_view>() << ")";
            }
        }
        ss << "\n上下文: ";
        // 添加更多上下文信息，比如当前正在解析的语句类型等
        errors_.push_back(ss.str());
        throw std::runtime_error(ss.str());
    }

    void Parser::synchronize()
    {
        [[maybe_unused]] auto token = advance();
        while (peek().type() != TokenType::END_OF_FILE)
        {
            // 在分号处停止
            if (peek().type() == TokenType::SEMICOLON)
            {
                [[maybe_unused]] auto semi = advance();
                return;
            }

            // 在语句开始处停止
            switch (peek().type())
            {
            case TokenType::VAR:
            case TokenType::PROCEDURE:
            case TokenType::BEGIN:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::END:
                return;
            default:
                [[maybe_unused]] auto skip = advance();
            }
        }
    }

    BinaryExpression::Op Parser::tokenTypeToBinaryOp(TokenType type) const
    {
        switch (type)
        {
        case TokenType::PLUS:
            return BinaryExpression::Op::Add;
        case TokenType::MINUS:
            return BinaryExpression::Op::Sub;
        case TokenType::MULTIPLY:
            return BinaryExpression::Op::Mul;
        case TokenType::DIVIDE:
            return BinaryExpression::Op::Div;
        case TokenType::POWER:
            return BinaryExpression::Op::Pow;
        case TokenType::EQ:
            return BinaryExpression::Op::Eq;
        case TokenType::NEQ:
            return BinaryExpression::Op::Neq;
        case TokenType::LT:
            return BinaryExpression::Op::Lt;
        case TokenType::LTE:
            return BinaryExpression::Op::Lte;
        case TokenType::GT:
            return BinaryExpression::Op::Gt;
        case TokenType::GTE:
            return BinaryExpression::Op::Gte;
        default:
        {
            std::stringstream ss;
            ss << "无效的运算符: " << static_cast<int>(type);
            throw std::runtime_error(ss.str());
        }
        }
    }

} // namespace pl0