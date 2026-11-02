#ifndef STRIDEREADER_H
#define STRIDEREADER_H

#include <string>
#include <string_view>
#include <cstdint>
#include <cstddef>
#include <charconv>

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


// -----------------------------------------------------------------------------
// StrideReader Inlined Implementation
// -----------------------------------------------------------------------------
inline StrideReader::StrideReader(std::string_view source)
    : m_source(source),
      m_cur(source.data()),
      m_end(source.data() + source.size()),
      m_line(1),
      m_lineStart(source.data()),
      m_hasError(false),
      m_errorLine(1),
      m_errorCol(1),
      m_hasPeeked(false) {}

inline bool StrideReader::error(std::string_view msg, size_t line, size_t col) {
    m_hasError = true;
    m_errorMsg = std::string(msg);
    m_errorLine = line;
    m_errorCol = col;
    return false;
}

inline void StrideReader::skipWhitespace() {
    while (m_cur < m_end) {
        char c = *m_cur;
        if (c == ' ' || c == '\t' || c == '\r') {
            m_cur++;
        } else if (c == '\n') {
            m_cur++;
            m_line++;
            m_lineStart = m_cur;
        } else if (c == '#') {
            while (m_cur < m_end && *m_cur != '\n') {
                m_cur++;
            }
        } else {
            break;
        }
    }
}

inline StrideReader::Token StrideReader::scanToken() {
    skipWhitespace();
    Token tok;
    tok.line = m_line;
    tok.col = static_cast<size_t>(m_cur - m_lineStart) + 1;

    if (m_cur >= m_end) {
        tok.type = TokenType::EndOfFile;
        return tok;
    }

    char c = *m_cur;
    if (c == '{') { m_cur++; tok.type = TokenType::OpenBrace; return tok; }
    if (c == '}') { m_cur++; tok.type = TokenType::CloseBrace; return tok; }
    if (c == '[') { m_cur++; tok.type = TokenType::OpenBracket; return tok; }
    if (c == ']') { m_cur++; tok.type = TokenType::CloseBracket; return tok; }
    if (c == ':') { m_cur++; tok.type = TokenType::Colon; return tok; }
    if (c == ',') { m_cur++; tok.type = TokenType::Comma; return tok; }

    // String literals
    if (c == '"' || c == '\'') {
        char quote = c;
        m_cur++;
        const char* strStart = m_cur;
        while (m_cur < m_end && *m_cur != quote && *m_cur != '\n') {
            if (*m_cur == '\\' && m_cur + 1 < m_end) {
                m_cur += 2;
            } else {
                m_cur++;
            }
        }
        tok.type = TokenType::String;
        tok.strVal = std::string_view(strStart, m_cur - strStart);
        if (m_cur < m_end && *m_cur == quote) {
            m_cur++;
        }
        return tok;
    }

    // Numbers: integer or floating point
    if ((c >= '0' && c <= '9') ||
        ((c == '-' || c == '+') && m_cur + 1 < m_end &&
         ((*(m_cur + 1) >= '0' && *(m_cur + 1) <= '9') || *(m_cur + 1) == '.'))) {
        const char* numStart = m_cur;
        bool isHex = false;
        bool isBin = false;
        bool isReal = false;

        if (c == '+' || c == '-') m_cur++;

        if (m_cur + 1 < m_end && *m_cur == '0' && (*(m_cur + 1) == 'x' || *(m_cur + 1) == 'X')) {
            isHex = true;
            m_cur += 2;
            while (m_cur < m_end && ((*m_cur >= '0' && *m_cur <= '9') ||
                                     (*m_cur >= 'a' && *m_cur <= 'f') ||
                                     (*m_cur >= 'A' && *m_cur <= 'F'))) {
                m_cur++;
            }
        } else if (m_cur + 1 < m_end && *m_cur == '0' && (*(m_cur + 1) == 'b' || *(m_cur + 1) == 'B')) {
            isBin = true;
            m_cur += 2;
            while (m_cur < m_end && (*m_cur == '0' || *m_cur == '1')) {
                m_cur++;
            }
        } else {
            while (m_cur < m_end && (*m_cur >= '0' && *m_cur <= '9')) {
                m_cur++;
            }
            if (m_cur < m_end && *m_cur == '.' && (m_cur + 1 >= m_end || *(m_cur + 1) != '.')) {
                isReal = true;
                m_cur++;
                while (m_cur < m_end && (*m_cur >= '0' && *m_cur <= '9')) {
                    m_cur++;
                }
            }
            if (m_cur < m_end && (*m_cur == 'e' || *m_cur == 'E')) {
                isReal = true;
                m_cur++;
                if (m_cur < m_end && (*m_cur == '+' || *m_cur == '-')) m_cur++;
                while (m_cur < m_end && (*m_cur >= '0' && *m_cur <= '9')) {
                    m_cur++;
                }
            }
        }

        const char* numEnd = m_cur;
        if (isHex) {
            int64_t val = 0;
            std::from_chars(numStart + 2, numEnd, val, 16);
            tok.type = TokenType::Int;
            tok.intVal = val;
        } else if (isBin) {
            int64_t val = 0;
            std::from_chars(numStart + 2, numEnd, val, 2);
            tok.type = TokenType::Int;
            tok.intVal = val;
        } else if (isReal) {
            double val = 0.0;
            std::from_chars(numStart, numEnd, val);
            tok.type = TokenType::Real;
            tok.realVal = val;
        } else {
            int64_t val = 0;
            std::from_chars(numStart, numEnd, val, 10);
            tok.type = TokenType::Int;
            tok.intVal = val;
        }
        return tok;
    }

    // Identifiers or boolean/none keywords
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        const char* idStart = m_cur;
        while (m_cur < m_end && ((*m_cur >= 'a' && *m_cur <= 'z') ||
                                 (*m_cur >= 'A' && *m_cur <= 'Z') ||
                                 (*m_cur >= '0' && *m_cur <= '9') ||
                                 *m_cur == '_')) {
            m_cur++;
        }
        std::string_view id(idStart, m_cur - idStart);
        if (id == "on" || id == "true") {
            tok.type = TokenType::Bool;
            tok.boolVal = true;
        } else if (id == "off" || id == "false") {
            tok.type = TokenType::Bool;
            tok.boolVal = false;
        } else if (id == "none") {
            tok.type = TokenType::None;
        } else {
            tok.type = TokenType::Identifier;
            tok.strVal = id;
        }
        return tok;
    }

    // Fallback unknown token
    tok.type = TokenType::String;
    tok.strVal = std::string_view(m_cur, 1);
    m_cur++;
    return tok;
}

inline StrideReader::Token StrideReader::peekToken() {
    if (!m_hasPeeked) {
        m_peekedToken = scanToken();
        m_hasPeeked = true;
    }
    return m_peekedToken;
}

inline StrideReader::Token StrideReader::nextToken() {
    if (m_hasPeeked) {
        m_hasPeeked = false;
        return m_peekedToken;
    }
    return scanToken();
}

inline void StrideReader::consumeToken() {
    if (m_hasPeeked) {
        m_hasPeeked = false;
    } else {
        scanToken();
    }
}

inline bool StrideReader::parse(StrideHandler& handler) {
    m_hasError = false;
    while (!m_hasError) {
        Token tok = peekToken();
        if (tok.type == TokenType::EndOfFile) break;

        if (tok.type == TokenType::Identifier) {
            consumeToken();
            std::string_view typeName = tok.strVal;
            Token next = peekToken();
            if (next.type == TokenType::OpenBrace) {
                consumeToken();
                handler.onBlockStart(typeName, "");
                if (!parseBlockBody(handler)) return false;
                handler.onBlockEnd();
            } else if (next.type == TokenType::Identifier) {
                Token nameTok = nextToken();
                Token braceTok = nextToken();
                if (braceTok.type != TokenType::OpenBrace) {
                    return error("Expected '{' after block name", braceTok.line, braceTok.col);
                }
                handler.onBlockStart(typeName, nameTok.strVal);
                if (!parseBlockBody(handler)) return false;
                handler.onBlockEnd();
            } else if (next.type == TokenType::Colon) {
                consumeToken();
                handler.onProperty(typeName);
                if (!parseValue(handler)) return false;
            } else {
                return error("Unexpected token at top level", tok.line, tok.col);
            }
        } else {
            return error("Expected block or property at top level", tok.line, tok.col);
        }
    }
    return !m_hasError;
}

inline bool StrideReader::parseBlockBody(StrideHandler& handler) {
    while (!m_hasError) {
        Token tok = peekToken();
        if (tok.type == TokenType::CloseBrace) {
            consumeToken();
            return true;
        }
        if (tok.type == TokenType::Comma) {
            consumeToken();
            continue;
        }
        if (tok.type == TokenType::EndOfFile) {
            return error("Unclosed block, expected '}'", tok.line, tok.col);
        }

        if (tok.type == TokenType::Identifier) {
            consumeToken();
            std::string_view name1 = tok.strVal;
            Token next = peekToken();

            if (next.type == TokenType::Colon) {
                consumeToken();
                handler.onProperty(name1);
                if (!parseValue(handler)) return false;
            } else if (next.type == TokenType::OpenBrace) {
                consumeToken();
                handler.onBlockStart(name1, "");
                if (!parseBlockBody(handler)) return false;
                handler.onBlockEnd();
            } else if (next.type == TokenType::Identifier) {
                Token nameTok = nextToken();
                Token braceTok = nextToken();
                if (braceTok.type != TokenType::OpenBrace) {
                    return error("Expected '{' after block name", braceTok.line, braceTok.col);
                }
                handler.onBlockStart(name1, nameTok.strVal);
                if (!parseBlockBody(handler)) return false;
                handler.onBlockEnd();
            } else {
                return error("Unexpected token in block", next.line, next.col);
            }
        } else {
            return error("Expected property or block definition", tok.line, tok.col);
        }
    }
    return true;
}

inline bool StrideReader::parseValue(StrideHandler& handler) {
    Token tok = peekToken();
    if (tok.type == TokenType::String) {
        consumeToken();
        handler.onString(tok.strVal);
        return true;
    }
    if (tok.type == TokenType::Int) {
        consumeToken();
        handler.onInt(tok.intVal);
        return true;
    }
    if (tok.type == TokenType::Real) {
        consumeToken();
        handler.onReal(tok.realVal);
        return true;
    }
    if (tok.type == TokenType::Bool) {
        consumeToken();
        handler.onBool(tok.boolVal);
        return true;
    }
    if (tok.type == TokenType::None) {
        consumeToken();
        handler.onNone();
        return true;
    }
    if (tok.type == TokenType::OpenBracket) {
        consumeToken();
        handler.onListStart();
        while (!m_hasError) {
            Token item = peekToken();
            if (item.type == TokenType::CloseBracket) {
                consumeToken();
                handler.onListEnd();
                return true;
            }
            if (item.type == TokenType::Comma) {
                consumeToken();
                continue;
            }
            if (item.type == TokenType::EndOfFile) {
                return error("Unclosed list, expected ']'", item.line, item.col);
            }
            if (!parseValue(handler)) return false;
            Token after = peekToken();
            if (after.type == TokenType::Comma) {
                consumeToken();
            }
        }
        return error("Unclosed list, expected ']'", tok.line, tok.col);
    }
    if (tok.type == TokenType::Identifier) {
        consumeToken();
        std::string_view typeName = tok.strVal;
        Token next = peekToken();
        if (next.type == TokenType::OpenBrace) {
            consumeToken();
            handler.onBlockStart(typeName, "");
            if (!parseBlockBody(handler)) return false;
            handler.onBlockEnd();
            return true;
        }
        if (next.type == TokenType::Identifier) {
            Token nameTok = nextToken();
            Token braceTok = nextToken();
            if (braceTok.type == TokenType::OpenBrace) {
                handler.onBlockStart(typeName, nameTok.strVal);
                if (!parseBlockBody(handler)) return false;
                handler.onBlockEnd();
                return true;
            }
            return error("Expected '{' for block value", braceTok.line, braceTok.col);
        }
        // Bare identifier treated as string value
        handler.onString(typeName);
        return true;
    }

    return error("Unexpected value token", tok.line, tok.col);
}

} // namespace strd

#endif // STRIDEREADER_H
