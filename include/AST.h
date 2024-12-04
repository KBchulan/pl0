#pragma once
#include "Token.h"

#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <string_view>

namespace pl0
{
    // 前向声明
    class ASTVisitor;
    class Expression;
    class Statement;
    class Block;
    class Program;
    class ConstDeclaration;
    class VarDeclaration;
    class ProcedureDeclaration;
    class AssignStatement;
    class CallStatement;
    class BeginStatement;
    class IfStatement;
    class WhileStatement;
    class BinaryExpression;
    class NumberExpression;
    class IdentifierExpression;
    class UnaryExpression;

    // 基类
    template <typename Derived>
    class ASTNode
    {
    public:
        virtual void accept(ASTVisitor &visitor) const = 0;
        virtual ~ASTNode() = default;

        [[nodiscard]] constexpr size_t line() const noexcept { return line_; }
        [[nodiscard]] constexpr size_t column() const noexcept { return column_; }

    protected:
        size_t line_ = 0;
        size_t column_ = 0;
    };

    // 表达式基类
    class Expression : public ASTNode<Expression>
    {
    public:
        [[nodiscard]] virtual bool isConstant() const noexcept = 0;
        [[nodiscard]] virtual std::optional<int64_t> evaluateConstant() const = 0;
        ~Expression() override = default;
    };

    // 语句基类
    class Statement : public ASTNode<Statement>
    {
    public:
        ~Statement() override = default;
    };

    // 常量声明
    class ConstDeclaration : public ASTNode<ConstDeclaration>
    {
    public:
        ConstDeclaration(std::string_view name, int64_t value)
            : name_(name), value_(value) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] std::string_view name() const noexcept { return name_; }
        [[nodiscard]] int64_t value() const noexcept { return value_; }

    private:
        std::string_view name_;
        int64_t value_;
    };

    // 变量声明
    class VarDeclaration : public ASTNode<VarDeclaration>
    {
    public:
        explicit VarDeclaration(std::string_view name) : name_(name) {}

        void accept(ASTVisitor &visitor) const override;
        [[nodiscard]] std::string_view name() const noexcept { return name_; }

    private:
        std::string_view name_;
    };

    // 过程声明
    class ProcedureDeclaration : public ASTNode<ProcedureDeclaration>
    {
    public:
        ProcedureDeclaration(std::string_view name, std::unique_ptr<Block> block)
            : name_(name), block_(std::move(block)) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] std::string_view name() const noexcept { return name_; }
        [[nodiscard]] const Block &block() const noexcept { return *block_; }

    private:
        std::string_view name_;
        std::unique_ptr<Block> block_;
    };

    // 块节点
    class Block : public ASTNode<Block>
    {
    public:
        Block(std::vector<std::unique_ptr<ConstDeclaration>> consts,
              std::vector<std::unique_ptr<VarDeclaration>> vars,
              std::vector<std::unique_ptr<ProcedureDeclaration>> procs,
              std::unique_ptr<Statement> statement)
            : consts_(std::move(consts)), vars_(std::move(vars)), procedures_(std::move(procs)), statement_(std::move(statement)) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] const auto &consts() const noexcept { return consts_; }
        [[nodiscard]] const auto &vars() const noexcept { return vars_; }
        [[nodiscard]] const auto &procedures() const noexcept { return procedures_; }
        [[nodiscard]] const auto &statement() const noexcept { return *statement_; }

    private:
        std::vector<std::unique_ptr<ConstDeclaration>> consts_;
        std::vector<std::unique_ptr<VarDeclaration>> vars_;
        std::vector<std::unique_ptr<ProcedureDeclaration>> procedures_;
        std::unique_ptr<Statement> statement_;
    };

    // 程序节点
    class Program : public ASTNode<Program>
    {
    public:
        explicit Program(std::unique_ptr<Block> block)
            : block_(std::move(block)) {}

        void accept(ASTVisitor &visitor) const override;
        [[nodiscard]] const Block &block() const noexcept { return *block_; }

    private:
        std::unique_ptr<Block> block_;
    };

    // 具体语句类型
    class AssignStatement : public Statement
    {
    public:
        AssignStatement(std::string_view name, std::unique_ptr<Expression> expr)
            : name_(name), expr_(std::move(expr)) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] std::string_view name() const noexcept { return name_; }
        [[nodiscard]] const Expression &expression() const noexcept { return *expr_; }

    private:
        std::string_view name_;
        std::unique_ptr<Expression> expr_;
    };

    // 调用语句
    class CallStatement : public Statement
    {
    public:
        explicit CallStatement(std::string_view proc_name)
            : proc_name_(proc_name) {}

        void accept(ASTVisitor &visitor) const override;
        [[nodiscard]] std::string_view procName() const noexcept { return proc_name_; }

    private:
        std::string_view proc_name_;
    };

    // Begin语句
    class BeginStatement : public Statement
    {
    public:
        explicit BeginStatement(std::vector<std::unique_ptr<Statement>> statements)
            : statements_(std::move(statements)) {}

        void accept(ASTVisitor &visitor) const override;
        [[nodiscard]] const auto &statements() const noexcept { return statements_; }

    private:
        std::vector<std::unique_ptr<Statement>> statements_;
    };

    // If语句
    class IfStatement : public Statement
    {
    public:
        IfStatement(std::unique_ptr<Expression> condition,
                    std::unique_ptr<Statement> then_stmt)
            : condition_(std::move(condition)), then_stmt_(std::move(then_stmt)) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] const Expression &condition() const noexcept { return *condition_; }
        [[nodiscard]] const Statement &thenStmt() const noexcept { return *then_stmt_; }

    private:
        std::unique_ptr<Expression> condition_;
        std::unique_ptr<Statement> then_stmt_;
    };

    // While语句
    class WhileStatement : public Statement
    {
    public:
        WhileStatement(std::unique_ptr<Expression> condition,
                       std::unique_ptr<Statement> body)
            : condition_(std::move(condition)), body_(std::move(body)) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] const Expression &condition() const noexcept { return *condition_; }
        [[nodiscard]] const Statement &body() const noexcept { return *body_; }

    private:
        std::unique_ptr<Expression> condition_;
        std::unique_ptr<Statement> body_;
    };

    // 具体表达式类型
    class BinaryExpression : public Expression
    {
    public:
        enum class Op {
            Add, Sub, Mul, Div, Pow,
            Eq, Neq, Lt, Lte, Gt, Gte
        };

        BinaryExpression(std::unique_ptr<Expression> left, Op op,
                         std::unique_ptr<Expression> right)
            : left_(std::move(left)), op_(op), right_(std::move(right)) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] bool isConstant() const noexcept override;
        [[nodiscard]] std::optional<int64_t> evaluateConstant() const override;

        [[nodiscard]] const Expression &left() const noexcept { return *left_; }
        [[nodiscard]] const Expression &right() const noexcept { return *right_; }
        [[nodiscard]] Op op() const noexcept { return op_; }

    private:
        std::unique_ptr<Expression> left_;
        Op op_;
        std::unique_ptr<Expression> right_;
    };

    // 数字表达式
    class NumberExpression : public Expression
    {
    public:
        explicit NumberExpression(int64_t value) : value_(value) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] bool isConstant() const noexcept override { return true; }
        [[nodiscard]] std::optional<int64_t> evaluateConstant() const override
        {
            return value_;
        }

        [[nodiscard]] int64_t value() const noexcept { return value_; }

    private:
        int64_t value_;
    };

    // 标识符表达式
    class IdentifierExpression : public Expression
    {
    public:
        explicit IdentifierExpression(std::string_view name) : name_(name) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] bool isConstant() const noexcept override { return false; }
        [[nodiscard]] std::optional<int64_t> evaluateConstant() const override
        {
            return std::nullopt;
        }

        [[nodiscard]] std::string_view name() const noexcept { return name_; }

    private:
        std::string_view name_;
    };

    // 一元表达式
    class UnaryExpression : public Expression
    {
    public:
        enum class Op
        {
            Neg,
            Not
        }; // 负号和逻辑非

        UnaryExpression(Op op, std::unique_ptr<Expression> operand)
            : op_(op), operand_(std::move(operand)) {}

        void accept(ASTVisitor &visitor) const override;

        [[nodiscard]] bool isConstant() const noexcept override
        {
            return operand_->isConstant();
        }

        [[nodiscard]] std::optional<int64_t> evaluateConstant() const override
        {
            auto val = operand_->evaluateConstant();
            if (!val)
                return std::nullopt;

            switch (op_)
            {
            case Op::Neg:
                return -(*val);
            case Op::Not:
                return !(*val);
            }
            return std::nullopt;
        }

        [[nodiscard]] const Expression &operand() const noexcept { return *operand_; }
        [[nodiscard]] Op op() const noexcept { return op_; }

    private:
        Op op_;
        std::unique_ptr<Expression> operand_;
    };

} // namespace pl0