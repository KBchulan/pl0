#pragma once

#include "AST.h"
#include "TokenInterpreter.h"

#include <memory>
#include <vector>
#include <string>
#include <optional>

namespace pl0
{

    // 前向声明所有AST节点类型
    class Program;
    class Block;
    class ConstDeclaration;
    class VarDeclaration;
    class ProcedureDeclaration;
    class Statement;
    class Expression;

    class Parser
    {
    public:
        explicit Parser(TokenInterpreter lexer) noexcept
            : lexer_(std::move(lexer)) {}

        // 主解析入口
        [[nodiscard]] std::unique_ptr<Program> parse();
        [[nodiscard]] const std::vector<std::string> &getErrors() const noexcept
        {
            return errors_;
        }

    private:
        // 递归下降解析方法
        [[nodiscard]] std::unique_ptr<Program> parseProgram();
        [[nodiscard]] std::unique_ptr<Block> parseBlock();
        [[nodiscard]] std::vector<std::unique_ptr<ConstDeclaration>> parseConstDeclarations();
        [[nodiscard]] std::vector<std::unique_ptr<VarDeclaration>> parseVarDeclarations();
        [[nodiscard]] std::vector<std::unique_ptr<ProcedureDeclaration>> parseProcedures();

        // 语句解析
        [[nodiscard]] std::unique_ptr<Statement> parseStatement();
        [[nodiscard]] std::unique_ptr<Statement> parseAssignStatement();
        [[nodiscard]] std::unique_ptr<Statement> parseCallStatement();
        [[nodiscard]] std::unique_ptr<Statement> parseBeginStatement();
        [[nodiscard]] std::unique_ptr<Statement> parseIfStatement();
        [[nodiscard]] std::unique_ptr<Statement> parseWhileStatement();

        // 表达式解析
        [[nodiscard]] std::unique_ptr<Expression> parseExpression();
        [[nodiscard]] std::unique_ptr<Expression> parseTerm();
        [[nodiscard]] std::unique_ptr<Expression> parsePower();
        [[nodiscard]] std::unique_ptr<Expression> parseFactor();
        [[nodiscard]] std::unique_ptr<Expression> parseCondition();
        [[nodiscard]] std::unique_ptr<Expression> parseComparison();

        // 辅助方法
        [[nodiscard]] Token peek() noexcept { return lexer_.peekToken(); }
        [[nodiscard]] Token advance() noexcept { return lexer_.nextToken(); }
        [[nodiscard]] bool match(TokenType type) noexcept;
        [[nodiscard]] bool check(TokenType type) noexcept;
        [[nodiscard]] Token consume(TokenType type, const std::string &message);
        void synchronize();

        // 错误处理
        [[noreturn]] void error(const std::string &message);
        void addError(const std::string &message)
        {
            errors_.push_back(message);
            had_error_ = true;
        }

        // 位置信息
        [[nodiscard]] size_t currentLine() const noexcept { return lexer_.line(); }
        [[nodiscard]] size_t currentColumn() const noexcept { return lexer_.column(); }

        // 成员变量
        TokenInterpreter lexer_;
        std::vector<std::string> errors_;
        bool had_error_ = false;

        // 添加这个辅助方法的声明
        [[nodiscard]] BinaryExpression::Op tokenTypeToBinaryOp(TokenType type) const;
    };

} // namespace pl0