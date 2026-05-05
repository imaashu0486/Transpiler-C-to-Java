#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGenerator.h"

using namespace std;

/**
 * @brief Reads a file and returns its contents as a string
 * @param filename The path to the file to read
 * @return The file contents as a string
 */
string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        exit(1);
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

/**
 * @brief Writes a string to a file
 * @param filename The path to the output file
 * @param content The content to write
 */
void writeFile(const string& filename, const string& content) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "' for writing" << endl;
        exit(1);
    }
    
    file << content;
    file.close();
    cout << "Successfully wrote transpiled code to '" << filename << "'" << endl;
}

/**
 * @brief Transpiles a C source file to Java
 * @param inputFile Path to the C source file
 * @param outputFile Path to the output Java file
 */
void transpile(const string& inputFile, const string& outputFile) {
    try {
        // Step 1: Read C source code
        cout << "Reading C source file: " << inputFile << endl;
        string source = readFile(inputFile);
        
        // Step 2: Lexical analysis
        cout << "Performing lexical analysis..." << endl;
        Lexer lexer(source);
        vector<Token> tokens = lexer.tokenize();
        cout << "Tokenization complete. Generated " << tokens.size() << " tokens." << endl;
        
        // Step 3: Syntax analysis
        cout << "Performing syntax analysis..." << endl;
        Parser parser(tokens);
        shared_ptr<Program> ast = parser.parse();
        cout << "Parsing complete. Generated " << ast->functions.size() << " functions." << endl;
        
        // Step 4: Code generation
        cout << "Generating Java code..." << endl;
        CodeGenerator generator;
        string javaCode = generator.generate(ast);
        
        // Step 5: Write output
        cout << "Writing Java code to output file..." << endl;
        writeFile(outputFile, javaCode);
        
        cout << "\nTranspilation completed successfully!" << endl;
        
    } catch (const ParseError& e) {
        cerr << "Parse error: " << e.what() << endl;
        exit(1);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        exit(1);
    }
}

/**
 * @brief Prints usage information
 */
void printUsage(const string& programName) {
    cout << "C to Java Transpiler" << endl;
    cout << "Usage: " << programName << " <input.c> [output.java]" << endl;
    cout << "  input.c    : Path to C source file" << endl;
    cout << "  output.java: Path to output Java file (default: output.java)" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    string inputFile = argv[1];
    string outputFile = (argc == 3) ? argv[2] : "output.java";
    
    transpile(inputFile, outputFile);
    
    return 0;
}
