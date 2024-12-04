#include "../include/TokenInterpreter.h"

#include <limits>
#include <cctype>
#include <iostream>
#include <algorithm>

namespace pl0
{

    namespace
    {
        constexpr bool isDigit(char c) noexcept { return c >= '0' && c <= '9'; }
        constexpr bool isAlpha(char c) noexcept { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
        constexpr bool isAlphaNum(char c) noexcept { return isAlpha(c) || isDigit(c); }
    }

    Token TokenInterpreter::nextToken() noexcept
    {
        if (has_peeked_)
        {
            has_peeked_ = false;
            return std::move(peeked_token_);
        }

        skipWhitespace();

        if (current_ >= source_.length())
        {
            return Token{TokenType::END_OF_FILE};
        }

        if (auto number = tryReadNumber())
        {
            return *number;
        }

        if (auto identifier = tryReadIdentifier())
        {
            return *identifier;
        }

        if (auto op = tryReadOperator())
        {
            return *op;
        }

        char invalid = peek();
        advance();
        std::string error_msg = "无效的字符: ";
        error_msg += invalid;
        return Token{TokenType::ERROR};
    }

    Token TokenInterpreter::peekToken() noexcept
    {
        if (!has_peeked_)
        {
            peeked_token_ = nextToken();
            has_peeked_ = true;
        }
        return peeked_token_;
    }

    void TokenInterpreter::skipWhitespace() noexcept
    {
        while (current_ < source_.length() && std::isspace(static_cast<unsigned char>(source_[current_])))
        {
            if (source_[current_] == '\n')
            {
                line_++;
                column_ = 0;
            }
            else
            {
                column_++;
            }
            current_++;
        }
    }

    std::optional<Token> TokenInterpreter::tryReadNumber() noexcept
    {
        if (!isDigit(peek()))
        {
            return std::nullopt;
        }

        size_t start = current_;
        int64_t value = 0;

        while (isDigit(peek()))
        {
            char digit = peek();
            if (value > (std::numeric_limits<int64_t>::max() - (digit - '0')) / 10)
            {
                std::cerr << "数字溢出: " << source_.substr(start, current_ - start) << '\n';
                return Token{TokenType::ERROR};
            }
            value = value * 10 + (digit - '0');
            advance();
        }

        std::cerr << "解析数字: " << value << " 从位置 " << start << " 到 " << current_ << '\n';
        return Token{TokenType::NUMBER, value};
    }

    std::optional<Token> TokenInterpreter::tryReadIdentifier() noexcept
    {
        if (!isAlpha(peek()))
        {
            return std::nullopt;
        }

        size_t start = current_;
        while (isAlphaNum(peek()))
        {
            advance();
        }

        std::string_view identifier = source_.substr(start, current_ - start);

        if (identifier.empty() || std::all_of(identifier.begin(), identifier.end(),
                                              [](char c)
                                              { return std::isspace(static_cast<unsigned char>(c)); }))
        {
            return std::nullopt;
        }

        auto it = std::find_if(KEYWORDS.begin(), KEYWORDS.end(),
                               [identifier](const auto &pair)
                               { return pair.first == identifier; });

        if (it != KEYWORDS.end())
        {
            return Token{it->second};
        }

        return Token{TokenType::IDENTIFIER, identifier};
    }

    std::optional<Token> TokenInterpreter::tryReadOperator() noexcept
    {
        char c = peek();
        char next = peekNext();

        switch (c)
        {
        case '+':
            advance();
            return Token{TokenType::PLUS};
        case '-':
            advance();
            return Token{TokenType::MINUS};
        case '*':
            advance();
            return Token{TokenType::MULTIPLY};
        case '/':
            advance();
            return Token{TokenType::DIVIDE};
        case '(':
            advance();
            return Token{TokenType::LPAREN};
        case ')':
            advance();
            return Token{TokenType::RPAREN};
        case ',':
            advance();
            return Token{TokenType::COMMA};
        case ';':
            advance();
            return Token{TokenType::SEMICOLON};
        case '.':
            advance();
            return Token{TokenType::PERIOD};

        case ':':
            if (next == '=')
            {
                advance();
                advance();
                return Token{TokenType::ASSIGN};
            }
            break;

        case '=':
            advance();
            return Token{TokenType::EQ};

        case '#':
            advance();
            return Token{TokenType::NEQ};

        case '<':
            advance();
            if (peek() == '=')
            {
                advance();
                return Token{TokenType::LTE};
            }
            return Token{TokenType::LT};

        case '>':
            advance();
            if (peek() == '=')
            {
                advance();
                return Token{TokenType::GTE};
            }
            return Token{TokenType::GT};

        case '^':
            advance();
            return Token{TokenType::POWER};
        }

        return std::nullopt;
    }

    char TokenInterpreter::peek() const noexcept
    {
        if (current_ >= source_.length())
            return '\0';
        return source_[current_];
    }

    char TokenInterpreter::peekNext() const noexcept
    {
        if (current_ + 1 >= source_.length())
            return '\0';
        return source_[current_ + 1];
    }

    void TokenInterpreter::advance() noexcept
    {
        if (current_ < source_.length())
        {
            current_++;
            column_++;
        }
    }

} // namespace pl0