#ifndef STRIDEREADER_H
#define STRIDEREADER_H

#include <string>
#include <string_view>
#include <cstdint>
#include <cstddef>

namespace strd {

/**
 * @brief SAX-style handler interface for streaming Stride data events.
 */
class StrideHandler {
public:
    virtual ~StrideHandler() = default;

    /**
     * @brief Called when a block declaration begins.
     * @param typeName The type identifier of the block (e.g. "gameUnit")
     * @param name The optional instance name of the block (e.g. "Unit_1", or empty if anonymous)
     */
    virtual void onBlockStart(std::string_view typeName, std::string_view name) {}

    /**
     * @brief Called when a block declaration finishes ('}').
     */
    virtual void onBlockEnd() {}

    /**
     * @brief Called when a property key is encountered before its value (e.g. "health:").
     */
    virtual void onProperty(std::string_view key) {}

    /**
     * @brief Called when a list begins ('[').
     */
    virtual void onListStart() {}

    /**
     * @brief Called when a list finishes (']').
     */
    virtual void onListEnd() {}

    /**
     * @brief Called when a string literal or unquoted identifier value is read.
     */
    virtual void onString(std::string_view val) {}

    /**
     * @brief Called when an integer value (decimal, hex 0x, or binary 0b) is read.
     */
    virtual void onInt(int64_t val) {}

    /**
     * @brief Called when a floating-point real value is read.
     */
    virtual void onReal(double val) {}

    /**
     * @brief Called when a boolean switch value ('on', 'off', 'true', 'false') is read.
     */
    virtual void onBool(bool val) {}

    /**
     * @brief Called when a 'none' value is read.
     */
    virtual void onNone() {}

    /**
     * @brief Called when a parsing or lexical error occurs.
     */
    virtual void onError(std::string_view errorMsg, size_t line, size_t col) {}
};

/**
 * @brief High-performance streaming reader for Stride data payloads.
 *
 * StrideReader scans and tokenizes Stride data directly from a string_view in a single pass
 * with zero heap allocations, emitting streaming SAX events to a StrideHandler.
 */
class StrideReader {
public:
    explicit StrideReader(std::string_view source);

    /**
     * @brief Parses the input source and emits events to the provided handler.
     * @return true on successful parsing with zero errors, false otherwise.
     */
    bool parse(StrideHandler& handler);

    bool hasError() const { return m_hasError; }
    const std::string& getErrorMessage() const { return m_errorMsg; }
    size_t getErrorLine() const { return m_errorLine; }
    size_t getErrorColumn() const { return m_errorCol; }

private:
    enum class TokenType {
        EndOfFile,
        Identifier,
        String,
        Int,
        Real,
        Bool,
        None,
        OpenBrace,    // {
        CloseBrace,   // }
        OpenBracket,  // [
        CloseBracket, // ]
        Colon,        // :
        Comma         // ,
    };

    struct Token {
        TokenType type{TokenType::EndOfFile};
        std::string_view strVal;
        int64_t intVal{0};
        double realVal{0.0};
        bool boolVal{false};
        size_t line{1};
        size_t col{1};
    };

    std::string_view m_source;
    const char* m_cur;
    const char* m_end;
    size_t m_line{1};
    const char* m_lineStart;

    bool m_hasError{false};
    std::string m_errorMsg;
    size_t m_errorLine{1};
    size_t m_errorCol{1};

    bool m_hasPeeked{false};
    Token m_peekedToken;

    bool error(std::string_view msg, size_t line, size_t col);
    void skipWhitespace();
    Token scanToken();
    Token peekToken();
    Token nextToken();
    void consumeToken();

    bool parseBlockBody(StrideHandler& handler);
    bool parseValue(StrideHandler& handler);
};

} // namespace strd

#endif // STRIDEREADER_H
