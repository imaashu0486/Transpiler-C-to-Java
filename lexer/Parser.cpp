#include "Parser.h"
#include <iostream>

using namespace std;

// ============================================================
// Constructor
// ============================================================

Parser::Parser(const vector<Token>& tokens)
    : tokens(tokens), pos(0) {}


// ============================================================
// Helper functions
// ============================================================

Token Parser::peek() const {
    if (pos >= tokens.size()) {
        return {TokenType::END_OF_FILE, "", 0};
    }
    return tokens[pos];
}

Token Parser::peekNext() const {
    if (pos + 1 >= tokens.size()) {
        return {TokenType::END_OF_FILE, "", 0};
    }
    return tokens[pos + 1];
}

Token Parser::advance() {
    return tokens[pos++];
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    return peek().type == type;
}

Token Parser::consume(TokenType type, const string& message) {
    if (check(type)) {
        return advance();
    }
    throw ParseError(message + " at line " + to_string(peek().line));
}


// ============================================================
// Type parsing
// ============================================================

string Parser::parseType() {
    if (match(TokenType::INT)) return "int";
    if (match(TokenType::FLOAT)) return "float";
    if (match(TokenType::DOUBLE)) return "double";
    if (match(TokenType::CHAR)) return "char";
    if (match(TokenType::VOID)) return "void";
    throw ParseError("Expected type at line " + to_string(peek().line));
}


// ============================================================
// Expression parsing (operator precedence)
// ============================================================

ExprPtr Parser::parseExpression() {
    return parseAssignment();
}

ExprPtr Parser::parseAssignment() {
    ExprPtr expr = parseLogicalOr();
    
    if (check(TokenType::ASSIGN)) {
        if (auto ident = dynamic_pointer_cast<Identifier>(expr)) {
            advance(); // consume =
            ExprPtr value = parseAssignment();
            return make_shared<Assignment>(ident->name, value);
        } else {
            throw ParseError("Invalid assignment target at line " + to_string(peek().line));
        }
    }
    
    return expr;
}

ExprPtr Parser::parseLogicalOr() {
    ExprPtr expr = parseLogicalAnd();
    
    while (match(TokenType::OR)) {
        string op = tokens[pos - 1].value;
        ExprPtr right = parseLogicalAnd();
        expr = make_shared<BinaryOp>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::parseLogicalAnd() {
    ExprPtr expr = parseEquality();
    
    while (match(TokenType::AND)) {
        string op = tokens[pos - 1].value;
        ExprPtr right = parseEquality();
        expr = make_shared<BinaryOp>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::parseEquality() {
    ExprPtr expr = parseComparison();
    
    while (check(TokenType::EQ) || check(TokenType::NEQ)) {
        string op = advance().value;
        ExprPtr right = parseComparison();
        expr = make_shared<BinaryOp>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::parseComparison() {
    ExprPtr expr = parseAdditive();
    
    while (check(TokenType::LT) || check(TokenType::GT) || 
           check(TokenType::LTE) || check(TokenType::GTE)) {
        string op = advance().value;
        ExprPtr right = parseAdditive();
        expr = make_shared<BinaryOp>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::parseAdditive() {
    ExprPtr expr = parseMultiplicative();
    
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        string op = advance().value;
        ExprPtr right = parseMultiplicative();
        expr = make_shared<BinaryOp>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::parseMultiplicative() {
    ExprPtr expr = parseUnary();
    
    while (check(TokenType::STAR) || check(TokenType::SLASH) || 
           check(TokenType::MOD)) {
        string op = advance().value;
        ExprPtr right = parseUnary();
        expr = make_shared<BinaryOp>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::parseUnary() {
    if (check(TokenType::NOT) || check(TokenType::MINUS) || check(TokenType::PLUS)) {
        string op = advance().value;
        ExprPtr operand = parseUnary();
        return make_shared<UnaryOp>(op, operand);
    }
    
    return parsePrimary();
}

ExprPtr Parser::parsePrimary() {
    // Number literal
    if (check(TokenType::NUMBER)) {
        string value = advance().value;
        return make_shared<NumberLiteral>(value);
    }

    // String literal
    if (check(TokenType::STRING)) {
        string value = advance().value;
        return make_shared<StringLiteral>(value);
    }

    // Identifier or function call (including printf keyword)
    if (check(TokenType::IDENTIFIER) || check(TokenType::PRINTF)) {
        string name = advance().value;
        
        // Check if it's a function call
        if (check(TokenType::LPAREN)) {
            advance(); // consume (
            auto funcCall = make_shared<FunctionCall>(name);
            
            if (!check(TokenType::RPAREN)) {
                do {
                    funcCall->arguments.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            
            consume(TokenType::RPAREN, "Expected ')' in function call");
            return funcCall;
        }
        
        return make_shared<Identifier>(name);
    }

    // Parenthesized expression
    if (match(TokenType::LPAREN)) {
        ExprPtr expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    throw ParseError("Unexpected token '" + peek().value + "' at line " + to_string(peek().line));
}


// ============================================================
// Statement parsing
// ============================================================

StmtPtr Parser::parseStatement() {
    if (check(TokenType::IF)) {
        return parseIfStatement();
    }
    if (check(TokenType::WHILE)) {
        return parseWhileStatement();
    }
    if (check(TokenType::FOR)) {
        return parseForStatement();
    }
    if (check(TokenType::RETURN)) {
        return parseReturnStatement();
    }
    if (check(TokenType::INT) || check(TokenType::FLOAT) || 
        check(TokenType::DOUBLE) || check(TokenType::CHAR)) {
        return parseVarDeclaration();
    }
    
    return parseExpressionStatement();
}

StmtPtr Parser::parseIfStatement() {
    consume(TokenType::IF, "Expected 'if'");
    consume(TokenType::LPAREN, "Expected '(' after 'if'");
    ExprPtr condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after if condition");
    
    auto ifStmt = make_shared<IfStatement>(condition);
    ifStmt->thenBlock = parseBlock();
    
    if (match(TokenType::ELSE)) {
        ifStmt->elseBlock = parseBlock();
    }
    
    return ifStmt;
}

StmtPtr Parser::parseWhileStatement() {
    consume(TokenType::WHILE, "Expected 'while'");
    consume(TokenType::LPAREN, "Expected '(' after 'while'");
    ExprPtr condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after while condition");
    
    auto whileStmt = make_shared<WhileStatement>(condition);
    whileStmt->body = parseBlock();
    
    return whileStmt;
}

StmtPtr Parser::parseForStatement() {
    consume(TokenType::FOR, "Expected 'for'");
    consume(TokenType::LPAREN, "Expected '(' after 'for'");
    
    ExprPtr init = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        init = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for init");
    
    ExprPtr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for condition");
    
    ExprPtr update = nullptr;
    if (!check(TokenType::RPAREN)) {
        update = parseExpression();
    }
    consume(TokenType::RPAREN, "Expected ')' after for clauses");
    
    auto forStmt = make_shared<ForStatement>(init, condition, update);
    forStmt->body = parseBlock();
    
    return forStmt;
}

StmtPtr Parser::parseReturnStatement() {
    consume(TokenType::RETURN, "Expected 'return'");
    
    ExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    
    return make_shared<ReturnStatement>(value);
}

StmtPtr Parser::parseVarDeclaration() {
    string type = parseType();
    string name = consume(TokenType::IDENTIFIER, "Expected variable name").value;
    
    ExprPtr initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return make_shared<VariableDeclaration>(type, name, initializer);
}

StmtPtr Parser::parseExpressionStatement() {
    ExprPtr expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return make_shared<ExpressionStatement>(expr);
}


// ============================================================
// Block parsing
// ============================================================

vector<StmtPtr> Parser::parseBlock() {
    consume(TokenType::LBRACE, "Expected '{'");
    
    vector<StmtPtr> statements;
    while (!check(TokenType::RBRACE) && !check(TokenType::END_OF_FILE)) {
        statements.push_back(parseStatement());
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    return statements;
}


// ============================================================
// Function parsing
// ============================================================

shared_ptr<FunctionDecl> Parser::parseFunctionDecl() {
    string returnType = parseType();
    string name = consume(TokenType::IDENTIFIER, "Expected function name").value;
    
    consume(TokenType::LPAREN, "Expected '(' after function name");
    
    auto func = make_shared<FunctionDecl>(returnType, name);
    
    if (!check(TokenType::RPAREN)) {
        do {
            string paramType = parseType();
            string paramName = consume(TokenType::IDENTIFIER, "Expected parameter name").value;
            func->parameters.push_back({paramType, paramName});
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    func->body = parseBlock();
    
    return func;
}


// ============================================================
// Main parse function
// ============================================================

shared_ptr<Program> Parser::parse() {
    auto program = make_shared<Program>();
    
    while (!check(TokenType::END_OF_FILE)) {
        try {
            program->functions.push_back(parseFunctionDecl());
        } catch (const ParseError& e) {
            cerr << "Parse error: " << e.what() << endl;
            // Skip to next function or EOF
            while (!check(TokenType::END_OF_FILE) && !check(TokenType::INT) && 
                   !check(TokenType::FLOAT) && !check(TokenType::DOUBLE) && 
                   !check(TokenType::CHAR) && !check(TokenType::VOID)) {
                advance();
            }
        }
    }
    
    return program;
}
