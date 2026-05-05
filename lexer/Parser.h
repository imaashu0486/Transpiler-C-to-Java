#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "AST.h"
#include <memory>
#include <stdexcept>

using namespace std;

/**
 * @class ParseError
 * @brief Exception thrown when parsing fails
 */
class ParseError : public runtime_error {
public:
    ParseError(const string& message) : runtime_error(message) {}
};

/**
 * @class Parser
 * @brief Recursive descent parser that converts tokens into an AST
 */
class Parser {
private:
    vector<Token> tokens;
    size_t pos;

    // Helper functions
    Token peek() const;
    Token peekNext() const;
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const string& message);

    // Parsing functions (expression parsing with precedence)
    ExprPtr parseExpression();
    ExprPtr parseAssignment();
    ExprPtr parseLogicalOr();
    ExprPtr parseLogicalAnd();
    ExprPtr parseEquality();
    ExprPtr parseComparison();
    ExprPtr parseAdditive();
    ExprPtr parseMultiplicative();
    ExprPtr parseUnary();
    ExprPtr parsePrimary();

    // Statement parsing
    StmtPtr parseStatement();
    StmtPtr parseIfStatement();
    StmtPtr parseWhileStatement();
    StmtPtr parseForStatement();
    StmtPtr parseReturnStatement();
    StmtPtr parseVarDeclaration();
    StmtPtr parseExpressionStatement();

    // Function parsing
    shared_ptr<FunctionDecl> parseFunctionDecl();

    // Utility functions
    string parseType();
    vector<StmtPtr> parseBlock();

public:
    /**
     * @brief Constructs a Parser instance
     * @param tokens Vector of tokens from the lexer
     */
    Parser(const vector<Token>& tokens);

    /**
     * @brief Parses the token stream into an AST
     * @return Shared pointer to the root Program node
     */
    shared_ptr<Program> parse();
};

#endif // PARSER_H
