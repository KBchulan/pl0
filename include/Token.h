#pragma once

#include <variant>
#include <concepts>
#include <string_view>

namespace pl0
{
    enum class TokenType {
        // 关键字
        CONST, VAR, PROCEDURE, CALL, BEGIN, END, IF, THEN, WHILE, DO, ODD,

        // 运算符和分隔符
        PLUS, MINUS, MULTIPLY, DIVIDE, POWER, EQ, NEQ, LT, LTE, GT, GTE,
        LPAREN, RPAREN, COMMA, SEMICOLON, PERIOD, ASSIGN,

        // 其他
        IDENTIFIER, NUMBER, END_OF_FILE, ERROR
    };

    // 使用concepts来约束模板参数
    template <typename T>
    concept TokenValue = std::same_as<T, std::string_view> ||
                         std::same_as<T, int64_t>;

    // Token类使用variant来存储不同类型的值
    class Token
    {
    public:
        constexpr Token() noexcept : type_(TokenType::ERROR) {}
        constexpr Token(TokenType type) noexcept : type_(type) {}

        template <TokenValue T>
        constexpr Token(TokenType type, T value) noexcept
            : type_(type), value_(std::move(value)) {}

        [[nodiscard]] constexpr TokenType type() const noexcept { return type_; }

        template <TokenValue T>
        [[nodiscard]] constexpr const T &value() const
        {
            if constexpr (std::is_same_v<T, int64_t>)
            {
                if (std::holds_alternative<int64_t>(value_))
                {
                    return std::get<int64_t>(value_);
                }
                throw std::bad_variant_access();
            }
            else if constexpr (std::is_same_v<T, std::string_view>)
            {
                if (std::holds_alternative<std::string_view>(value_))
                {
                    return std::get<std::string_view>(value_);
                }
                throw std::bad_variant_access();
            }
            throw std::bad_variant_access();
        }

        [[nodiscard]] constexpr bool hasValue() const noexcept
        {
            return !value_.valueless_by_exception();
        }

    private:
        TokenType type_;
        std::variant<std::monostate, std::string_view, int64_t> value_;
    };

} // namespace pl0