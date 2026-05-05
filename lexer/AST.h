#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

// Forward declarations
class ASTNode;
class Expression;
class Statement;
class FunctionDecl;

typedef shared_ptr<ASTNode> ASTNodePtr;
typedef shared_ptr<Expression> ExprPtr;
typedef shared_ptr<Statement> StmtPtr;

/**
 * @class ASTNode
 * @brief Base class for all AST nodes
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

/**
 * @class Expression
 * @brief Base class for all expressions
 */
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

/**
 * @class NumberLiteral
 * @brief Represents a numeric literal (e.g., 42, 3.14)
 */
class NumberLiteral : public Expression {
public:
    string value;
    NumberLiteral(const string& val) : value(val) {}
};

/**
 * @class StringLiteral
 * @brief Represents a string literal (e.g., "hello")
 */
class StringLiteral : public Expression {
public:
    string value;
    StringLiteral(const string& val) : value(val) {}
};

/**
 * @class Identifier
 * @brief Represents a variable or function identifier
 */
class Identifier : public Expression {
public:
    string name;
    Identifier(const string& n) : name(n) {}
};

/**
 * @class BinaryOp
 * @brief Represents a binary operation (e.g., a + b)
 */
class BinaryOp : public Expression {
public:
    ExprPtr left;
    string op;
    ExprPtr right;
    BinaryOp(ExprPtr l, const string& o, ExprPtr r) 
        : left(l), op(o), right(r) {}
};

/**
 * @class UnaryOp
 * @brief Represents a unary operation (e.g., -a, !b)
 */
class UnaryOp : public Expression {
public:
    string op;
    ExprPtr operand;
    UnaryOp(const string& o, ExprPtr operand) 
        : op(o), operand(operand) {}
};

/**
 * @class FunctionCall
 * @brief Represents a function call (e.g., printf(...))
 */
class FunctionCall : public Expression {
public:
    string name;
    vector<ExprPtr> arguments;
    FunctionCall(const string& n) : name(n) {}
};

/**
 * @class Assignment
 * @brief Represents a variable assignment (e.g., x = 5)
 */
class Assignment : public Expression {
public:
    string variable;
    ExprPtr value;
    Assignment(const string& var, ExprPtr val) 
        : variable(var), value(val) {}
};

/**
 * @class Statement
 * @brief Base class for all statements
 */
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

/**
 * @class ExpressionStatement
 * @brief A statement containing an expression
 */
class ExpressionStatement : public Statement {
public:
    ExprPtr expression;
    ExpressionStatement(ExprPtr expr) : expression(expr) {}
};

/**
 * @class IfStatement
 * @brief Represents an if statement (optional else)
 */
class IfStatement : public Statement {
public:
    ExprPtr condition;
    vector<StmtPtr> thenBlock;
    vector<StmtPtr> elseBlock;
    IfStatement(ExprPtr cond) : condition(cond) {}
};

/**
 * @class WhileStatement
 * @brief Represents a while loop
 */
class WhileStatement : public Statement {
public:
    ExprPtr condition;
    vector<StmtPtr> body;
    WhileStatement(ExprPtr cond) : condition(cond) {}
};

/**
 * @class ForStatement
 * @brief Represents a for loop
 */
class ForStatement : public Statement {
public:
    ExprPtr init;
    ExprPtr condition;
    ExprPtr update;
    vector<StmtPtr> body;
    ForStatement(ExprPtr i, ExprPtr c, ExprPtr u) 
        : init(i), condition(c), update(u) {}
};

/**
 * @class ReturnStatement
 * @brief Represents a return statement
 */
class ReturnStatement : public Statement {
public:
    ExprPtr value;
    ReturnStatement(ExprPtr val = nullptr) : value(val) {}
};

/**
 * @class VariableDeclaration
 * @brief Represents a variable declaration
 */
class VariableDeclaration : public Statement {
public:
    string type;
    string name;
    ExprPtr initializer;
    VariableDeclaration(const string& t, const string& n, ExprPtr init = nullptr) 
        : type(t), name(n), initializer(init) {}
};

/**
 * @class FunctionDecl
 * @brief Represents a function declaration
 */
class FunctionDecl : public ASTNode {
public:
    string returnType;
    string name;
    vector<pair<string, string>> parameters; // (type, name)
    vector<StmtPtr> body;
    
    FunctionDecl(const string& retType, const string& n) 
        : returnType(retType), name(n) {}
};

/**
 * @class Program
 * @brief Represents the entire program (collection of functions)
 */
class Program : public ASTNode {
public:
    vector<shared_ptr<FunctionDecl>> functions;
};

#endif // AST_H
