#pragma once
#include "AST.h"

namespace pl0
{
    class Program;
    class Block;
    class ConstDeclaration;
    class VarDeclaration;
    class ProcedureDeclaration;
    class AssignStatement;
    class CallStatement;
    class BeginStatement;
    class IfStatement;
    class WhileStatement;
    class BinaryExpression;
    class UnaryExpression;
    class NumberExpression;
    class IdentifierExpression;

    template <typename T>
    concept Visitable = requires(T t, ASTVisitor &visitor) {
        { t.accept(visitor) } -> std::same_as<void>;
    };

    class ASTVisitor
    {
    public:
        virtual ~ASTVisitor() = default;

        virtual void visit(const Program &node) = 0;
        virtual void visit(const Block &node) = 0;
        virtual void visit(const ConstDeclaration &node) = 0;
        virtual void visit(const VarDeclaration &node) = 0;
        virtual void visit(const ProcedureDeclaration &node) = 0;

        virtual void visit(const AssignStatement &node) = 0;
        virtual void visit(const CallStatement &node) = 0;
        virtual void visit(const BeginStatement &node) = 0;
        virtual void visit(const IfStatement &node) = 0;
        virtual void visit(const WhileStatement &node) = 0;

        virtual void visit(const BinaryExpression &node) = 0;
        virtual void visit(const UnaryExpression &node) = 0;
        virtual void visit(const NumberExpression &node) = 0;
        virtual void visit(const IdentifierExpression &node) = 0;
    };

} // namespace pl0