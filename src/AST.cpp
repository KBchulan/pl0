#include "../include/AST.h"
#include "../include/ASTVisitor.h"

namespace pl0
{
    // Program
    void Program::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // Block
    void Block::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // ConstDeclaration
    void ConstDeclaration::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // VarDeclaration
    void VarDeclaration::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // ProcedureDeclaration
    void ProcedureDeclaration::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // AssignStatement
    void AssignStatement::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // CallStatement
    void CallStatement::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // BeginStatement
    void BeginStatement::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // IfStatement
    void IfStatement::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // WhileStatement
    void WhileStatement::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // BinaryExpression
    void BinaryExpression::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    bool BinaryExpression::isConstant() const noexcept
    {
        return left_->isConstant() && right_->isConstant();
    }

    std::optional<int64_t> BinaryExpression::evaluateConstant() const
    {
        auto left_val = left_->evaluateConstant();
        auto right_val = right_->evaluateConstant();

        if (!left_val || !right_val)
            return std::nullopt;

        switch (op_)
        {
        case Op::Add:
            return *left_val + *right_val;
        case Op::Sub:
            return *left_val - *right_val;
        case Op::Mul:
            return *left_val * *right_val;
        case Op::Div:
            if (*right_val == 0)
                return std::nullopt;
            return *left_val / *right_val;
        case Op::Pow:
        {
            if (*right_val < 0)
                return std::nullopt;
            int64_t result = 1;
            for (int64_t i = 0; i < *right_val; ++i)
            {
                result *= *left_val;
            }
            return result;
        }
        case Op::Eq:
            return *left_val == *right_val;
        case Op::Neq:
            return *left_val != *right_val;
        case Op::Lt:
            return *left_val < *right_val;
        case Op::Lte:
            return *left_val <= *right_val;
        case Op::Gt:
            return *left_val > *right_val;
        case Op::Gte:
            return *left_val >= *right_val;
        }
        return std::nullopt;
    }

    // NumberExpression
    void NumberExpression::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

    // IdentifierExpression
    void IdentifierExpression::accept(ASTVisitor &visitor) const
    {
        visitor.visit(*this);
    }

} // namespace pl0