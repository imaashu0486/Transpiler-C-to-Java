cmake_minimum_required(VERSION 3.10)
project(CToJavaTranspiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    lexer/Lexer.cpp
    lexer/Parser.cpp
    lexer/CodeGenerator.cpp
    lexer/main.cpp
)

# Headers
set(HEADERS
    lexer/Lexer.h
    lexer/Parser.h
    lexer/CodeGenerator.h
    lexer/AST.h
)

# Create executable
add_executable(transpiler ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(transpiler PRIVATE lexer)
