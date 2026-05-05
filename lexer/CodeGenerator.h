#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "AST.h"
#include <string>
#include <sstream>

using namespace std;

/**
 * @class CodeGenerator
 * @brief Generates Java code from the AST
 */
class CodeGenerator {
private:
    int indentLevel;

    // Helper functions
    string getIndent() const;
    string javaType(const string& cType);
    string generateExpression(const ExprPtr& expr);
    string generateBinaryOp(const shared_ptr<BinaryOp>& expr);
    string generateUnaryOp(const shared_ptr<UnaryOp>& expr);
    string generateFunctionCall(const shared_ptr<FunctionCall>& expr);
    string generateAssignment(const shared_ptr<Assignment>& expr);

    // Statement generation
    string generateStatement(const StmtPtr& stmt);
    string generateExpressionStatement(const shared_ptr<ExpressionStatement>& stmt);
    string generateIfStatement(const shared_ptr<IfStatement>& stmt);
    string generateWhileStatement(const shared_ptr<WhileStatement>& stmt);
    string generateForStatement(const shared_ptr<ForStatement>& stmt);
    string generateReturnStatement(const shared_ptr<ReturnStatement>& stmt);
    string generateVarDeclaration(const shared_ptr<VariableDeclaration>& stmt);

public:
    /**
     * @brief Constructs a CodeGenerator instance
     */
    CodeGenerator();

    /**
     * @brief Generates Java code from an AST
     * @param program The program AST
     * @return Java source code as a string
     */
    string generate(const shared_ptr<Program>& program);

    /**
     * @brief Generates a function declaration
     * @param func The function declaration node
     * @return Java function code
     */
    string generateFunction(const shared_ptr<FunctionDecl>& func);
};

#endif // CODE_GENERATOR_H
