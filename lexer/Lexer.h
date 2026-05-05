#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

using namespace std;

/**
 * @enum TokenType
 * @brief Enumeration of all possible token types in C
 */
enum class TokenType {
    // Literals
    NUMBER,
    STRING,
    IDENTIFIER,

    // Keywords
    INT,
    FLOAT,
    DOUBLE,
    CHAR,
    IF,
    ELSE,
    FOR,
    WHILE,
    RETURN,
    PRINTF,
    VOID,
    MAIN,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    MOD,
    ASSIGN,
    EQ,      // ==
    NEQ,     // !=
    LT,      // <
    GT,      // >
    LTE,     // <=
    GTE,     // >=
    AND,     // &&
    OR,      // ||
    NOT,     // !

    // Delimiters
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    COMMA,

    // Special
    END_OF_FILE,
    UNKNOWN
};

/**
 * @struct Token
 * @brief Represents a single token with type, value, and line information
 */
struct Token {
    TokenType type;
    string value;
    int line;
};

/**
 * @class Lexer
 * @brief Lexical analyzer that converts source code into tokens
 */
class Lexer {
private:
    string source;
    size_t pos;
    int line;

    // Character-level helpers
    char peek();
    char peekNext();
    char advance();
    bool match(char expected);

    // Skipping helpers
    void skipWhitespace();
    void skipComment();
    void skipPreprocessor();

    // Token creation helpers
    Token makeNumber();
    Token makeIdentifier();
    Token makeString();

public:
    /**
     * @brief Constructs a Lexer instance
     * @param source Full source code as a single string
     */
    Lexer(const string& source);

    /**
     * @brief Tokenizes the entire source code
     * @return Vector of tokens including END_OF_FILE
     */
    vector<Token> tokenize();
};

#endif // LEXER_H
