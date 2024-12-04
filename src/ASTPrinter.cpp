#include "../include/AST.h"
#include "../include/ASTPrinter.h"

namespace pl0
{
    void ASTPrinter::visit(const Program &node)
    {
        out_ << "Program\n";
        increaseLevel();
        node.block().accept(*this);
        decreaseLevel();
    }

    void ASTPrinter::visit(const Block &node)
    {
        indent();
        out_ << "Block\n";
        increaseLevel();

        // Constants
        if (!node.consts().empty())
        {
            indent();
            out_ << "Constants:\n";
            increaseLevel();
            for (const auto &constDecl : node.consts())
            {
                constDecl->accept(*this);
            }
            decreaseLevel();
        }

        // Variables
        if (!node.vars().empty())
        {
            indent();
            out_ << "Variables:\n";
            increaseLevel();
            for (const auto &varDecl : node.vars())
            {
                varDecl->accept(*this);
            }
            decreaseLevel();
        }

        // Procedures
        if (!node.procedures().empty())
        {
            indent();
            out_ << "Procedures:\n";
            increaseLevel();
            for (const auto &procDecl : node.procedures())
            {
                procDecl->accept(*this);
            }
            decreaseLevel();
        }

        // Statement
        indent();
        out_ << "Statement:\n";
        increaseLevel();
        node.statement().accept(*this);
        decreaseLevel();

        decreaseLevel();
    }

    void ASTPrinter::visit(const BeginStatement &node)
    {
        indent();
        out_ << "Begin\n";
        increaseLevel();
        for (const auto &stmt : node.statements())
        {
            stmt->accept(*this);
        }
        decreaseLevel();
    }

    void ASTPrinter::visit(const IfStatement &node)
    {
        indent();
        out_ << "If\n";
        increaseLevel();
        indent();
        out_ << "Condition:\n";
        increaseLevel();
        node.condition().accept(*this);
        decreaseLevel();
        indent();
        out_ << "Then:\n";
        increaseLevel();
        node.thenStmt().accept(*this);
        decreaseLevel();
        decreaseLevel();
    }

    void ASTPrinter::visit(const WhileStatement &node)
    {
        indent();
        out_ << "While\n";
        increaseLevel();
        indent();
        out_ << "Condition:\n";
        increaseLevel();
        node.condition().accept(*this);
        decreaseLevel();
        indent();
        out_ << "Body:\n";
        increaseLevel();
        node.body().accept(*this);
        decreaseLevel();
        decreaseLevel();
    }

    void ASTPrinter::visit(const BinaryExpression &node)
    {
        indent();
        out_ << "Binary Operation ";
        switch (node.op())
        {
        case BinaryExpression::Op::Add:
            out_ << "+";
            break;
        case BinaryExpression::Op::Sub:
            out_ << "-";
            break;
        case BinaryExpression::Op::Mul:
            out_ << "*";
            break;
        case BinaryExpression::Op::Div:
            out_ << "/";
            break;
        case BinaryExpression::Op::Pow:
            out_ << "^";
            break;
        case BinaryExpression::Op::Eq:
            out_ << "=";
            break;
        case BinaryExpression::Op::Neq:
            out_ << "#";
            break;
        case BinaryExpression::Op::Lt:
            out_ << "<";
            break;
        case BinaryExpression::Op::Lte:
            out_ << "<=";
            break;
        case BinaryExpression::Op::Gt:
            out_ << ">";
            break;
        case BinaryExpression::Op::Gte:
            out_ << ">=";
            break;
        }
        out_ << '\n';
        increaseLevel();
        indent();
        out_ << "Left:\n";
        increaseLevel();
        node.left().accept(*this);
        decreaseLevel();
        indent();
        out_ << "Right:\n";
        increaseLevel();
        node.right().accept(*this);
        decreaseLevel();
        decreaseLevel();
    }

    void ASTPrinter::visit(const UnaryExpression &node)
    {
        indent();
        out_ << "Unary Operation ";
        switch (node.op())
        {
        case UnaryExpression::Op::Neg:
            out_ << "-";
            break;
        case UnaryExpression::Op::Not:
            out_ << "!";
            break;
        }
        out_ << '\n';
        increaseLevel();
        node.operand().accept(*this);
        decreaseLevel();
    }

    void ASTPrinter::visit(const NumberExpression &node)
    {
        indent();
        out_ << node.value() << ": Number\n";
    }

    void ASTPrinter::visit(const IdentifierExpression &node)
    {
        indent();
        out_ << node.name() << ": Identifier\n";
    }

    void ASTPrinter::visit(const ConstDeclaration &node)
    {
        indent();
        out_ << node.name() << " = " << node.value() << ": Constant Declaration\n";
    }

    void ASTPrinter::visit(const VarDeclaration &node)
    {
        indent();
        out_ << node.name() << ": Variable Declaration\n";
    }

    void ASTPrinter::visit(const ProcedureDeclaration &node)
    {
        indent();
        out_ << node.name() << ": Procedure Declaration\n";
        increaseLevel();
        node.block().accept(*this);
        decreaseLevel();
    }

    void ASTPrinter::visit(const AssignStatement &node)
    {
        indent();
        out_ << node.name() << " := : Assignment Statement\n";
        increaseLevel();
        node.expression().accept(*this);
        decreaseLevel();
    }

    void ASTPrinter::visit(const CallStatement &node)
    {
        indent();
        out_ << node.procName() << ": Call Statement\n";
    }

} // namespace pl0