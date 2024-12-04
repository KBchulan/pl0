#include "../include/SemanticAnalyzer.h"

#include <sstream>

namespace pl0
{
    namespace
    {
        // 辅助函数：生成错误消息
        std::string makeError(const std::string &message, std::string_view name)
        {
            std::stringstream ss;
            ss << message << ": " << name;
            return ss.str();
        }
    }

    bool SemanticAnalyzer::analyze(const Program &program)
    {
        try
        {
            program.accept(*this);
            return !had_error_;
        }
        catch (const std::exception &e)
        {
            addError(e.what());
            return false;
        }
    }

    void SemanticAnalyzer::visit(const Program &node)
    {
        info_.push_back("Analyzing program...");
        enterScope();
        node.block().accept(*this);
        leaveScope();
    }

    void SemanticAnalyzer::visit(const Block &node)
    {
        // 处理常量声明
        for (const auto &constDecl : node.consts())
        {
            constDecl->accept(*this);
        }

        // 处理变量声明
        for (const auto &varDecl : node.vars())
        {
            varDecl->accept(*this);
        }

        // 处理过程声明
        for (const auto &procDecl : node.procedures())
        {
            procDecl->accept(*this);
        }

        // 处理语句
        node.statement().accept(*this);
    }

    void SemanticAnalyzer::visit(const ConstDeclaration &node)
    {
        std::stringstream ss;
        ss << "Declaring constant: " << node.name() << " = " << node.value();
        info_.push_back(ss.str());

        Symbol symbol{
            .type = SymbolType::Constant,
            .value = node.value(),
            .level = current_level_,
            .index = 0 // 常量不需要地址
        };

        if (!declareSymbol(node.name(), symbol))
        {
            addError("Duplicate constant declaration: " + std::string(node.name()));
        }
    }

    void SemanticAnalyzer::visit(const VarDeclaration &node)
    {
        std::stringstream ss;
        ss << "Declaring variable: " << node.name() << " at level " << current_level_;
        info_.push_back(ss.str());

        Symbol symbol{
            .type = SymbolType::Variable,
            .value = std::nullopt,
            .level = current_level_,
            .index = var_index_++};

        if (!declareSymbol(node.name(), symbol))
        {
            addError("Duplicate variable declaration: " + std::string(node.name()));
        }
    }

    void SemanticAnalyzer::visit(const ProcedureDeclaration &node)
    {
        std::stringstream ss;
        ss << "Declaring procedure: " << node.name() << " at level " << current_level_;
        info_.push_back(ss.str());

        Symbol symbol{
            .type = SymbolType::Procedure,
            .value = std::nullopt,
            .level = current_level_,
            .index = 0 // 过程不需要地址
        };

        if (!declareSymbol(node.name(), symbol))
        {
            addError("Duplicate procedure declaration: " + std::string(node.name()));
            return;
        }

        enterScope();
        node.block().accept(*this);
        leaveScope();
    }

    void SemanticAnalyzer::visit(const AssignStatement &node)
    {
        auto symbol = lookupSymbol(node.name());
        if (!symbol)
        {
            addError(makeError("未声明的标识符", node.name()));
            return;
        }

        if (symbol->type != SymbolType::Variable)
        {
            addError(makeError("只能给变量赋值", node.name()));
            return;
        }

        node.expression().accept(*this);
    }

    void SemanticAnalyzer::visit(const CallStatement &node)
    {
        auto symbol = lookupSymbol(node.procName());
        if (!symbol)
        {
            addError(makeError("未声明的过程", node.procName()));
            return;
        }

        if (symbol->type != SymbolType::Procedure)
        {
            addError(makeError("只能调用过程", node.procName()));
        }
    }

    void SemanticAnalyzer::visit(const BeginStatement &node)
    {
        for (const auto &stmt : node.statements())
        {
            stmt->accept(*this);
        }
    }

    void SemanticAnalyzer::visit(const IfStatement &node)
    {
        node.condition().accept(*this);
        node.thenStmt().accept(*this);
    }

    void SemanticAnalyzer::visit(const WhileStatement &node)
    {
        node.condition().accept(*this);
        node.body().accept(*this);
    }

    void SemanticAnalyzer::visit(const BinaryExpression &node)
    {
        node.left().accept(*this);
        node.right().accept(*this);

        // 检查除零错误
        if (node.op() == BinaryExpression::Op::Div)
        {
            if (auto right_val = node.right().evaluateConstant())
            {
                if (*right_val == 0)
                {
                    addError("除数不能为零");
                }
            }
        }
    }

    void SemanticAnalyzer::visit(const UnaryExpression &node)
    {
        node.operand().accept(*this);
    }

    void SemanticAnalyzer::visit(const NumberExpression &node)
    {
        last_expression_value_ = node.value();
    }

    void SemanticAnalyzer::visit(const IdentifierExpression &node)
    {
        auto symbol = lookupSymbol(node.name());
        if (!symbol)
        {
            addError(makeError("未声明的标识符", node.name()));
            return;
        }

        if (symbol->type == SymbolType::Procedure)
        {
            addError(makeError("过程不能用作表达式", node.name()));
            return;
        }

        if (symbol->type == SymbolType::Constant)
        {
            last_expression_value_ = symbol->value;
        }
    }

    void SemanticAnalyzer::enterScope()
    {
        symbol_tables_.emplace_back();
        current_level_++;
        var_index_ = 0;
    }

    void SemanticAnalyzer::leaveScope()
    {
        if (!symbol_tables_.empty())
        {
            symbol_tables_.pop_back();
        }
        if (current_level_ > 0)
        {
            current_level_--;
        }
    }

    bool SemanticAnalyzer::declareSymbol(std::string_view name, Symbol symbol)
    {
        if (symbol_tables_.empty())
        {
            symbol_tables_.emplace_back();
        }

        auto &current_scope = symbol_tables_.back();
        if (current_scope.contains(name))
        {
            return false;
        }

        current_scope.emplace(name, symbol);
        return true;
    }

    std::optional<Symbol> SemanticAnalyzer::lookupSymbol(std::string_view name) const
    {
        for (auto it = symbol_tables_.rbegin(); it != symbol_tables_.rend(); ++it)
        {
            if (auto found = it->find(name); found != it->end())
            {
                return found->second;
            }
        }
        return std::nullopt;
    }

    void SemanticAnalyzer::addError(std::string message)
    {
        errors_.push_back(std::move(message));
        had_error_ = true;
    }

} // namespace pl0