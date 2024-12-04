#pragma once

#include "Parser.h"
#include "ASTPrinter.h"
#include "SemanticAnalyzer.h"
#include "TokenInterpreter.h"

#include <string>
#include <fstream>
#include <filesystem>

namespace pl0
{
    class Compiler
    {
    public:
        struct Result
        {
            bool success;
            std::vector<std::string> errors;
            std::vector<Token> tokens;
            std::unique_ptr<Program> ast;
            std::vector<std::string> semanticInfo;
        };

        [[nodiscard]] static Result compileFile(const std::filesystem::path &path);

        [[nodiscard]] static Result compileString(std::string_view source);

        static void outputResults(const Result &result, const std::filesystem::path &outputDir);

    private:
        static void outputTokens(const std::vector<Token> &tokens,
                                 const std::filesystem::path &path);
        static void outputAST(const Program &ast, const std::filesystem::path &path);
        static void outputSemanticInfo(const std::vector<std::string> &info,
                                       const std::filesystem::path &path);
    };

} // namespace pl0