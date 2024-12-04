#include "../include/Compiler.h"

#include <sstream>
#include <fstream>

namespace pl0
{

    namespace
    {
        struct TokenTypeInfo
        {
            std::string_view cn;
            std::string_view en;
        };

        TokenTypeInfo tokenTypeToString(TokenType type)
        {
            switch (type)
            {
            case TokenType::CONST:
                return {"CONST关键字", "CONST Keyword"};
            case TokenType::VAR:
                return {"VAR关键字", "VAR Keyword"};
            case TokenType::PROCEDURE:
                return {"PROCEDURE关键字", "PROCEDURE Keyword"};
            case TokenType::CALL:
                return {"CALL关键字", "CALL Keyword"};
            case TokenType::BEGIN:
                return {"BEGIN关键字", "BEGIN Keyword"};
            case TokenType::END:
                return {"END关键字", "END Keyword"};
            case TokenType::IF:
                return {"IF关键字", "IF Keyword"};
            case TokenType::THEN:
                return {"THEN关键字", "THEN Keyword"};
            case TokenType::WHILE:
                return {"WHILE关键字", "WHILE Keyword"};
            case TokenType::DO:
                return {"DO关键字", "DO Keyword"};
            case TokenType::ODD:
                return {"ODD关键字", "ODD Keyword"};

            case TokenType::PLUS:
                return {"加号(+)", "Plus(+)"};
            case TokenType::MINUS:
                return {"减号(-)", "Minus(-)"};
            case TokenType::MULTIPLY:
                return {"乘号(*)", "Multiply(*)"};
            case TokenType::DIVIDE:
                return {"除号(/)", "Divide(/)"};
            case TokenType::POWER:
                return {"幂运算(^)", "Power(^)"};
            case TokenType::EQ:
                return {"等于(=)", "Equal(=)"};
            case TokenType::NEQ:
                return {"不等于(#)", "NotEqual(#)"};
            case TokenType::LT:
                return {"小于(<)", "LessThan(<)"};
            case TokenType::LTE:
                return {"小于等于(<=)", "LessEqual(<=)"};
            case TokenType::GT:
                return {"大于(>)", "GreaterThan(>)"};
            case TokenType::GTE:
                return {"大于等于(>=)", "GreaterEqual(>=)"};
            case TokenType::LPAREN:
                return {"左括号(()", "LeftParen(()"};
            case TokenType::RPAREN:
                return {"右括号())", "RightParen())"};
            case TokenType::COMMA:
                return {"逗号(,)", "Comma(,)"};
            case TokenType::SEMICOLON:
                return {"分号(;)", "Semicolon(;)"};
            case TokenType::PERIOD:
                return {"句号(.)", "Period(.)"};
            case TokenType::ASSIGN:
                return {"赋值(:=)", "Assign(:=)"};

            case TokenType::IDENTIFIER:
                return {"标识符", "Identifier"};
            case TokenType::NUMBER:
                return {"数字", "Number"};
            case TokenType::END_OF_FILE:
                return {"文件结束", "EndOfFile"};
            case TokenType::ERROR:
                return {"错误", "Error"};

            default:
                return {"未知类型", "Unknown"};
            }
        }
    }

    Compiler::Result Compiler::compileFile(const std::filesystem::path &path)
    {
        std::ifstream file(path);
        if (!file)
        {
            return Result{false, {std::string("无法打开文件: ") + path.string()}};
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return compileString(buffer.str());
    }

    Compiler::Result Compiler::compileString(std::string_view source)
    {
        Result result{true};

        try
        {
            // 词法分析
            TokenInterpreter lexer{source};
            std::vector<Token> tokens;

            while (true)
            {
                auto token = lexer.nextToken();
                if (token.type() == TokenType::ERROR)
                {
                    result.success = false;
                    result.errors.push_back("词法分析错误");
                    return result;
                }
                tokens.push_back(token);
                if (token.type() == TokenType::END_OF_FILE)
                {
                    break;
                }
            }
            result.tokens = std::move(tokens);

            // 语法分析
            Parser parser{TokenInterpreter{source}};
            result.ast = parser.parse();
            if (!result.ast)
            {
                result.success = false;
                result.errors = parser.getErrors();
                return result;
            }

            // 语义分析
            SemanticAnalyzer analyzer;
            if (!analyzer.analyze(*result.ast))
            {
                result.success = false;
                result.errors = analyzer.getErrors();
            }
            result.semanticInfo = analyzer.getInfo();
        }
        catch (const std::exception &e)
        {
            result.success = false;
            result.errors.push_back(e.what());
        }

        return result;
    }

    void Compiler::outputResults(const Result &result, const std::filesystem::path &outputDir)
    {
        std::filesystem::create_directories(outputDir);

        {
            std::ofstream file(outputDir / "tokens.txt");
            file << "Lexical Analysis Result:\n";
            file << "=======================\n\n";
            for (const auto &token : result.tokens)
            {
                switch (token.type())
                {
                case TokenType::NUMBER:
                    file << token.value<int64_t>() << ": Number\n";
                    break;
                case TokenType::IDENTIFIER:
                    file << token.value<std::string_view>() << ": Identifier\n";
                    break;
                case TokenType::CONST:
                    file << "const: Keyword\n";
                    break;
                case TokenType::VAR:
                    file << "var: Keyword\n";
                    break;
                case TokenType::PROCEDURE:
                    file << "procedure: Keyword\n";
                    break;
                case TokenType::CALL:
                    file << "call: Keyword\n";
                    break;
                case TokenType::BEGIN:
                    file << "begin: Keyword\n";
                    break;
                case TokenType::END:
                    file << "end: Keyword\n";
                    break;
                case TokenType::IF:
                    file << "if: Keyword\n";
                    break;
                case TokenType::THEN:
                    file << "then: Keyword\n";
                    break;
                case TokenType::WHILE:
                    file << "while: Keyword\n";
                    break;
                case TokenType::DO:
                    file << "do: Keyword\n";
                    break;
                case TokenType::ODD:
                    file << "odd: Keyword\n";
                    break;

                case TokenType::PLUS:
                    file << "+: Operator\n";
                    break;
                case TokenType::MINUS:
                    file << "-: Operator\n";
                    break;
                case TokenType::MULTIPLY:
                    file << "*: Operator\n";
                    break;
                case TokenType::DIVIDE:
                    file << "/: Operator\n";
                    break;
                case TokenType::POWER:
                    file << "^: Operator\n";
                    break;
                case TokenType::EQ:
                    file << "=: Operator\n";
                    break;
                case TokenType::NEQ:
                    file << "#: Operator\n";
                    break;
                case TokenType::LT:
                    file << "<: Operator\n";
                    break;
                case TokenType::LTE:
                    file << "<=: Operator\n";
                    break;
                case TokenType::GT:
                    file << ">: Operator\n";
                    break;
                case TokenType::GTE:
                    file << ">=: Operator\n";
                    break;
                case TokenType::ASSIGN:
                    file << ":=: Operator\n";
                    break;

                case TokenType::LPAREN:
                    file << "(: Delimiter\n";
                    break;
                case TokenType::RPAREN:
                    file << "): Delimiter\n";
                    break;
                case TokenType::COMMA:
                    file << ",: Delimiter\n";
                    break;
                case TokenType::SEMICOLON:
                    file << ";: Delimiter\n";
                    break;
                case TokenType::PERIOD:
                    file << ".: Delimiter\n";
                    break;

                case TokenType::END_OF_FILE:
                    file << "EOF: End of File\n";
                    break;
                case TokenType::ERROR:
                    file << "ERROR: Invalid Token\n";
                    break;
                default:
                    file << "Unknown Token Type\n";
                    break;
                }
            }
        }

        {
            std::ofstream file(outputDir / "ast.txt");
            file << "Abstract Syntax Tree:\n";
            file << "===================\n\n";
            if (result.ast)
            {
                ASTPrinter printer(file);
                result.ast->accept(printer);
            }
            else
            {
                file << "AST construction failed\n";
            }
        }

        {
            std::ofstream file(outputDir / "semantic.txt");
            file << "Semantic Analysis Result:\n";
            file << "=======================\n\n";

            if (!result.semanticInfo.empty())
            {
                file << "Analysis Information:\n";
                for (const auto &info : result.semanticInfo)
                {
                    file << "- " << info << '\n';
                }
                file << '\n';
            }

            if (!result.errors.empty())
            {
                file << "Semantic Errors:\n";
                for (const auto &error : result.errors)
                {
                    file << "- " << error << '\n';
                }
            }
            else
            {
                file << "No semantic errors found.\n";
            }
        }

        if (!result.errors.empty())
        {
            std::ofstream file(outputDir / "errors.txt");
            file << "编译错误：\n";
            file << "=========\n\n";
            for (const auto &error : result.errors)
            {
                file << error << "\n\n";
            }
        }
    }

} // namespace pl0