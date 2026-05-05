#include "Lexer.h"

#include <cctype>
#include <unordered_map>

using namespace std;

/**
 * @file Lexer.cpp
 * @brief Implements the Lexer class for lexical analysis.
 *
 * This file contains the implementation of the lexer responsible
 * for converting restricted C source code into a sequence of tokens.
 * It ignores whitespace, comments, and preprocessor directives.
 */

// ============================================================
// Constructor
// ============================================================

/**
 * @brief Constructs a Lexer instance.
 * @param source Full source code as a single string.
 */
Lexer::Lexer(const string& source)
    : source(source), pos(0), line(1) {}


// ============================================================
// Character-level helpers
// ============================================================

/**
 * @brief Returns the current character without consuming it.
 * @return Current character or '\\0' if end of input.
 */
char Lexer::peek() {
    if (pos >= source.size()) {
        return '\0';
    }
    return source[pos];
}

/**
 * @brief Returns the next character without consuming it.
 * @return Next character or '\\0' if out of bounds.
 */
char Lexer::peekNext() {
    if (pos + 1 >= source.size()) {
        return '\0';
    }
    return source[pos + 1];
}

/**
 * @brief Consumes and returns the current character.
 *
 * Advances the lexer position and updates line count
 * if a newline is encountered.
 *
 * @return The consumed character.
 */
char Lexer::advance() {
    char current = peek();
    pos++;

    if (current == '\n') {
        line++;
    }

    return current;
}

/**
 * @brief Consumes the current character if it matches the expected one.
 * @param expected The expected character.
 * @return true if the character matched and was consumed, false otherwise.
 */
bool Lexer::match(char expected) {
    if (peek() != expected) {
        return false;
    }
    pos++;
    return true;
}


// ============================================================
// Skipping helpers
// ============================================================

/**
 * @brief Skips whitespace characters.
 *
 * Handles spaces, tabs, and newlines.
 */
void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

/**
 * @brief Skips preprocessor directives.
 *
 * Ignores lines beginning with '#', such as:
 * - #include
 * - #define
 */
void Lexer::skipPreprocessor() {
    while (peek() != '\n' && peek() != '\0') {
        advance();
    }
}

/**
 * @brief Skips C-style comments.
 *
 * Supports:
 * - Single-line comments (//)
 * - Multi-line comments (/* ... *\/)
 */
void Lexer::skipComment() {

    // Single-line comment
    if (peek() == '/' && peekNext() == '/') {
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
        return;
    }

    // Multi-line comment
    if (peek() == '/' && peekNext() == '*') {
        advance(); // consume '/'
        advance(); // consume '*'

        while (!(peek() == '*' && peekNext() == '/') && peek() != '\0') {
            advance();
        }

        // Consume closing */
        if (peek() != '\0') {
            advance();
            advance();
        }
    }
}


// ============================================================
// Token creation helpers
// ============================================================

/**
 * @brief Creates a numeric literal token.
 * @return Token representing a number.
 */
Token Lexer::makeNumber() {
    string value;

    while (isdigit(peek())) {
        value += advance();
    }

    return {TokenType::NUMBER, value, line};
}

/**
 * @brief Creates an identifier or keyword token.
 *
 * Checks whether the lexeme matches a reserved keyword.
 *
 * @return Token representing an identifier or keyword.
 */
Token Lexer::makeIdentifier() {
    string value;

    while (isalnum(peek()) || peek() == '_') {
        value += advance();
    }

    static unordered_map<string, TokenType> keywords = {
        {"int", TokenType::INT},
        {"float", TokenType::FLOAT},
        {"double", TokenType::DOUBLE},
        {"char", TokenType::CHAR},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"for", TokenType::FOR},
        {"while", TokenType::WHILE},
        {"return", TokenType::RETURN},
        {"printf", TokenType::PRINTF}
    };

    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return {it->second, value, line};
    }

    return {TokenType::IDENTIFIER, value, line};
}

/**
 * @brief Creates a string literal token.
 * @return Token representing a string literal.
 */
Token Lexer::makeString() {
    string value;

    advance(); // consume opening quote

    while (peek() != '"' && peek() != '\0') {
        value += advance();
    }

    advance(); // consume closing quote
    return {TokenType::STRING, value, line};
}


// ============================================================
// Main tokenization routine
// ============================================================

/**
 * @brief Tokenizes the entire source code.
 *
 * Iterates through the input source and produces
 * a sequence of tokens until end-of-file is reached.
 *
 * @return Vector of tokens including END_OF_FILE.
 */
vector<Token> Lexer::tokenize() {
    vector<Token> tokens;

    while (peek() != '\0') {

        skipWhitespace();

        // Ignore preprocessor directives
        if (peek() == '#') {
            skipPreprocessor();
            continue;
        }

        // Ignore comments
        if (peek() == '/' && (peekNext() == '/' || peekNext() == '*')) {
            skipComment();
            continue;
        }

        char c = peek();

        // Numeric literal
        if (isdigit(c)) {
            tokens.push_back(makeNumber());
            continue;
        }

        // Identifier or keyword
        if (isalpha(c) || c == '_') {
            tokens.push_back(makeIdentifier());
            continue;
        }

        // Operators and delimiters
        switch (advance()) {

            case '+': tokens.push_back({TokenType::PLUS, "+", line}); break;
            case '-': tokens.push_back({TokenType::MINUS, "-", line}); break;
            case '*': tokens.push_back({TokenType::STAR, "*", line}); break;
            case '/': tokens.push_back({TokenType::SLASH, "/", line}); break;
            case '%': tokens.push_back({TokenType::MOD, "%", line}); break;

            case '=':
                tokens.push_back(match('=') ?
                    Token{TokenType::EQ, "==", line} :
                    Token{TokenType::ASSIGN, "=", line});
                break;

            case '!':
                if (match('=')) {
                    tokens.push_back({TokenType::NEQ, "!=", line});
                } else {
                    tokens.push_back({TokenType::NOT, "!", line});
                }
                break;

            case '<':
                tokens.push_back(match('=') ?
                    Token{TokenType::LTE, "<=", line} :
                    Token{TokenType::LT, "<", line});
                break;

            case '>':
                tokens.push_back(match('=') ?
                    Token{TokenType::GTE, ">=", line} :
                    Token{TokenType::GT, ">", line});
                break;
            
            case '&':
                if (match('&')) {
                    tokens.push_back({TokenType::AND, "&&", line});
                } else {
                    tokens.push_back({TokenType::UNKNOWN, "&", line});
                }
                break;

            case '|':
                if (match('|')) {
                    tokens.push_back({TokenType::OR, "||", line});
                } else {
                    tokens.push_back({TokenType::UNKNOWN, "|", line});
                }
                break;

            case '(':
                tokens.push_back({TokenType::LPAREN, "(", line}); break;
            case ')':
                tokens.push_back({TokenType::RPAREN, ")", line}); break;
            case '{':
                tokens.push_back({TokenType::LBRACE, "{", line}); break;
            case '}':
                tokens.push_back({TokenType::RBRACE, "}", line}); break;
            case ';':
                tokens.push_back({TokenType::SEMICOLON, ";", line}); break;
            case ',':
                tokens.push_back({TokenType::COMMA, ",", line}); break;

            case '"':
                pos--; // rewind so makeString sees opening quote
                tokens.push_back(makeString());
                break;

            default:
                tokens.push_back({TokenType::UNKNOWN, string(1, c), line});
        }

        skipWhitespace();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}