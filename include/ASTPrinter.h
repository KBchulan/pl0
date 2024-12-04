#pragma once
#include "ASTVisitor.h"

#include <string>
#include <ostream>

namespace pl0
{
    class ASTPrinter : public ASTVisitor
    {
    public:
        explicit ASTPrinter(std::ostream &out) : out_(out) {}

        void visit(const Program &node) override;
        void visit(const Block &node) override;
        void visit(const ConstDeclaration &node) override;
        void visit(const VarDeclaration &node) override;
        void visit(const ProcedureDeclaration &node) override;
        void visit(const AssignStatement &node) override;
        void visit(const CallStatement &node) override;
        void visit(const BeginStatement &node) override;
        void visit(const IfStatement &node) override;
        void visit(const WhileStatement &node) override;
        void visit(const BinaryExpression &node) override;
        void visit(const UnaryExpression &node) override;
        void visit(const NumberExpression &node) override;
        void visit(const IdentifierExpression &node) override;

    private:
        void indent() { out_ << std::string(level_ * 2, ' '); }
        void increaseLevel() { ++level_; }
        void decreaseLevel() { --level_; }

        std::ostream &out_;
        size_t level_ = 0;
    };

} // namespace pl0