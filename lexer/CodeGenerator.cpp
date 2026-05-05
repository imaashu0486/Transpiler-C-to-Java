#include "CodeGenerator.h"
#include <memory>
#include <sstream>

using namespace std;

// ============================================================
// Constructor
// ============================================================

CodeGenerator::CodeGenerator() : indentLevel(0) {}


// ============================================================
// Helper functions
// ============================================================

string CodeGenerator::getIndent() const {
    return string(indentLevel * 4, ' ');
}

string CodeGenerator::javaType(const string& cType) {
    if (cType == "int" || cType == "float" || cType == "double" || cType == "char") {
        return cType;
    }
    if (cType == "void") {
        return "void";
    }
    return "Object"; // Default for unknown types
}


// ============================================================
// Expression generation
// ============================================================

string CodeGenerator::generateExpression(const ExprPtr& expr) {
    if (!expr) return "";
    
    if (auto numLit = dynamic_pointer_cast<NumberLiteral>(expr)) {
        return numLit->value;
    }
    
    if (auto strLit = dynamic_pointer_cast<StringLiteral>(expr)) {
        return "\"" + strLit->value + "\"";
    }
    
    if (auto ident = dynamic_pointer_cast<Identifier>(expr)) {
        return ident->name;
    }
    
    if (auto binOp = dynamic_pointer_cast<BinaryOp>(expr)) {
        return generateBinaryOp(binOp);
    }
    
    if (auto unOp = dynamic_pointer_cast<UnaryOp>(expr)) {
        return generateUnaryOp(unOp);
    }
    
    if (auto funcCall = dynamic_pointer_cast<FunctionCall>(expr)) {
        return generateFunctionCall(funcCall);
    }
    
    if (auto assign = dynamic_pointer_cast<Assignment>(expr)) {
        return generateAssignment(assign);
    }
    
    return "";
}

string CodeGenerator::generateBinaryOp(const shared_ptr<BinaryOp>& expr) {
    string left = generateExpression(expr->left);
    string right = generateExpression(expr->right);
    return "(" + left + " " + expr->op + " " + right + ")";
}

string CodeGenerator::generateUnaryOp(const shared_ptr<UnaryOp>& expr) {
    string operand = generateExpression(expr->operand);
    return expr->op + operand;
}

string CodeGenerator::generateFunctionCall(const shared_ptr<FunctionCall>& expr) {
    // Special handling for printf
    if (expr->name == "printf") {
        stringstream ss;
        ss << "System.out.print(";
        
        for (size_t i = 0; i < expr->arguments.size(); ++i) {
            if (i > 0) ss << " + ";
            ss << generateExpression(expr->arguments[i]);
        }
        
        ss << ")";
        return ss.str();
    }
    
    // Regular function call
    stringstream ss;
    ss << expr->name << "(";
    
    for (size_t i = 0; i < expr->arguments.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << generateExpression(expr->arguments[i]);
    }
    
    ss << ")";
    return ss.str();
}

string CodeGenerator::generateAssignment(const shared_ptr<Assignment>& expr) {
    string value = generateExpression(expr->value);
    return expr->variable + " = " + value;
}


// ============================================================
// Statement generation
// ============================================================

string CodeGenerator::generateStatement(const StmtPtr& stmt) {
    if (!stmt) return "";
    
    if (auto exprStmt = dynamic_pointer_cast<ExpressionStatement>(stmt)) {
        return generateExpressionStatement(exprStmt);
    }
    
    if (auto ifStmt = dynamic_pointer_cast<IfStatement>(stmt)) {
        return generateIfStatement(ifStmt);
    }
    
    if (auto whileStmt = dynamic_pointer_cast<WhileStatement>(stmt)) {
        return generateWhileStatement(whileStmt);
    }
    
    if (auto forStmt = dynamic_pointer_cast<ForStatement>(stmt)) {
        return generateForStatement(forStmt);
    }
    
    if (auto retStmt = dynamic_pointer_cast<ReturnStatement>(stmt)) {
        return generateReturnStatement(retStmt);
    }
    
    if (auto varDecl = dynamic_pointer_cast<VariableDeclaration>(stmt)) {
        return generateVarDeclaration(varDecl);
    }
    
    return "";
}

string CodeGenerator::generateExpressionStatement(const shared_ptr<ExpressionStatement>& stmt) {
    return getIndent() + generateExpression(stmt->expression) + ";\n";
}

string CodeGenerator::generateIfStatement(const shared_ptr<IfStatement>& stmt) {
    stringstream ss;
    ss << getIndent() << "if (" << generateExpression(stmt->condition) << ") {\n";
    
    indentLevel++;
    for (const auto& s : stmt->thenBlock) {
        ss << generateStatement(s);
    }
    indentLevel--;
    
    if (!stmt->elseBlock.empty()) {
        ss << getIndent() << "} else {\n";
        indentLevel++;
        for (const auto& s : stmt->elseBlock) {
            ss << generateStatement(s);
        }
        indentLevel--;
    }
    
    ss << getIndent() << "}\n";
    return ss.str();
}

string CodeGenerator::generateWhileStatement(const shared_ptr<WhileStatement>& stmt) {
    stringstream ss;
    ss << getIndent() << "while (" << generateExpression(stmt->condition) << ") {\n";
    
    indentLevel++;
    for (const auto& s : stmt->body) {
        ss << generateStatement(s);
    }
    indentLevel--;
    
    ss << getIndent() << "}\n";
    return ss.str();
}

string CodeGenerator::generateForStatement(const shared_ptr<ForStatement>& stmt) {
    stringstream ss;
    ss << getIndent() << "for (";
    
    if (stmt->init) {
        ss << generateExpression(stmt->init);
    }
    ss << "; ";
    
    if (stmt->condition) {
        ss << generateExpression(stmt->condition);
    }
    ss << "; ";
    
    if (stmt->update) {
        ss << generateExpression(stmt->update);
    }
    ss << ") {\n";
    
    indentLevel++;
    for (const auto& s : stmt->body) {
        ss << generateStatement(s);
    }
    indentLevel--;
    
    ss << getIndent() << "}\n";
    return ss.str();
}

string CodeGenerator::generateReturnStatement(const shared_ptr<ReturnStatement>& stmt) {
    stringstream ss;
    ss << getIndent() << "return";
    
    if (stmt->value) {
        ss << " " << generateExpression(stmt->value);
    }
    
    ss << ";\n";
    return ss.str();
}

string CodeGenerator::generateVarDeclaration(const shared_ptr<VariableDeclaration>& stmt) {
    stringstream ss;
    ss << getIndent() << javaType(stmt->type) << " " << stmt->name;
    
    if (stmt->initializer) {
        ss << " = " << generateExpression(stmt->initializer);
    }
    
    ss << ";\n";
    return ss.str();
}


// ============================================================
// Function generation
// ============================================================

string CodeGenerator::generateFunction(const shared_ptr<FunctionDecl>& func) {
    stringstream ss;
    
    // Generate function signature
    ss << getIndent() << "public static " << javaType(func->returnType) << " " 
       << func->name << "(";
    
    for (size_t i = 0; i < func->parameters.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << javaType(func->parameters[i].first) << " " << func->parameters[i].second;
    }
    
    ss << ") {\n";
    
    indentLevel++;
    for (const auto& stmt : func->body) {
        ss << generateStatement(stmt);
    }
    indentLevel--;
    
    ss << getIndent() << "}\n\n";
    
    return ss.str();
}


// ============================================================
// Main code generation
// ============================================================

string CodeGenerator::generate(const shared_ptr<Program>& program) {
    stringstream ss;
    
    // Generate class header
    ss << "public class TranspiledProgram {\n\n";
    
    indentLevel = 1;
    
    // Generate all functions
    for (const auto& func : program->functions) {
        ss << generateFunction(func);
    }
    
    indentLevel = 0;
    ss << "}\n";
    
    return ss.str();
}
