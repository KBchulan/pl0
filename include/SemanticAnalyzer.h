#pragma once
#include "ASTVisitor.h"

#include <stack>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

namespace pl0
{
    enum class SymbolType
    {
        Constant,
        Variable,
        Procedure
    };

    struct Symbol
    {
        SymbolType type;
        std::optional<int64_t> value;
        size_t level;
        size_t index;
    };

    class SemanticAnalyzer : public ASTVisitor
    {
    public:
        SemanticAnalyzer() = default;
        ~SemanticAnalyzer() override = default;

        [[nodiscard]] bool analyze(const Program &program);
        [[nodiscard]] const std::vector<std::string> &getErrors() const noexcept { return errors_; }
        [[nodiscard]] const std::vector<std::string> &getInfo() const noexcept { return info_; }

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
        void enterScope();
        void leaveScope();
        bool declareSymbol(std::string_view name, Symbol symbol);
        std::optional<Symbol> lookupSymbol(std::string_view name) const;

        void addError(std::string message);

        std::vector<std::unordered_map<std::string_view, Symbol>> symbol_tables_;
        size_t current_level_ = 0;

        std::vector<std::string> errors_;
        std::vector<std::string> info_;

        std::optional<int64_t> last_expression_value_;
        bool in_procedure_ = false;
        size_t var_index_ = 0;
        bool had_error_ = false;
    };

} // namespace pl0