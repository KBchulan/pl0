#pragma once

#include "Token.h"

#include <array>
#include <limits>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace pl0 {

class TokenInterpreter {
public:
    explicit TokenInterpreter(std::string_view source) noexcept 
        : source_(source) {}

    [[nodiscard]] Token nextToken() noexcept;
    [[nodiscard]] Token peekToken() noexcept;
    
    [[nodiscard]] size_t line() const noexcept { return line_; }
    [[nodiscard]] size_t column() const noexcept { return column_; }

private:
    static constexpr std::array KEYWORDS = {
        std::pair{"const", TokenType::CONST},
        std::pair{"var", TokenType::VAR},
        std::pair{"procedure", TokenType::PROCEDURE},
        std::pair{"call", TokenType::CALL},
        std::pair{"begin", TokenType::BEGIN},
        std::pair{"end", TokenType::END},
        std::pair{"if", TokenType::IF},
        std::pair{"then", TokenType::THEN},
        std::pair{"while", TokenType::WHILE},
        std::pair{"do", TokenType::DO},
        std::pair{"odd", TokenType::ODD}
    };

    void skipWhitespace() noexcept;
    [[nodiscard]] std::optional<Token> tryReadNumber() noexcept;
    [[nodiscard]] std::optional<Token> tryReadIdentifier() noexcept;
    [[nodiscard]] std::optional<Token> tryReadOperator() noexcept;
    
    [[nodiscard]] char peek() const noexcept;
    [[nodiscard]] char peekNext() const noexcept;
    void advance() noexcept;

    std::string_view source_;
    mutable size_t current_ = 0;
    mutable size_t line_ = 1;
    mutable size_t column_ = 1;
    mutable Token peeked_token_{TokenType::ERROR};
    mutable bool has_peeked_ = false;
};

} // namespace pl0 