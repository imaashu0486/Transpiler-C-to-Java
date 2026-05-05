# C to Java Transpiler

A production-ready, source-to-source transpiler that converts a restricted subset of C into readable Java.

## Highlights
- End-to-end pipeline: Lexer → Parser → AST → Java code generation
- Recursive descent parser with operator precedence
- Clean, formatted Java output wrapped in a class
- Cross-platform build (Windows, Linux, macOS)

## Quick Start

### Build (Windows)
```bash
build.bat
```

### Build (Linux/macOS)
```bash
./build.sh
```

### Run
```bash
# Default output: output.java in the current directory
./transpiler input.c

# Custom output
./transpiler input.c output.java
```

## Output Location
The generated Java file is saved in the directory where the transpiler is executed.

## Features

### Supported C Subset
- **Types**: `int`, `float`, `double`, `char`, `void`
- **Control Flow**: `if`, `else`, `for`, `while`, `return`
- **Operators**: `+`, `-`, `*`, `/`, `%`, `=`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, `||`, `!`
- **Functions**: declarations, parameters, calls
- **Literals**: numeric and string
- **Comments**: `//` and `/* ... */`
- **Preprocessor**: lines starting with `#` are ignored

### Java Generation
- Generates `public class TranspiledProgram`
- Converts C functions to `public static` Java methods
- Converts `printf(...)` to `System.out.print(...)`
- Maintains indentation and readable formatting

## Project Structure
```
Transpiler/
├── CMakeLists.txt
├── build.bat
├── build.sh
├── README.md
├── lexer/
│   ├── AST.h
│   ├── CodeGenerator.cpp
│   ├── CodeGenerator.h
│   ├── Lexer.cpp
│   ├── Lexer.h
│   ├── Parser.cpp
│   ├── Parser.h
│   └── main.cpp
└── examples/
    ├── example1_simple.c
    ├── example2_functions.c
    ├── example3_loops.c
    └── example4_conditions.c
```

## Build With CMake (Optional)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Example
**C input:**
```c
int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(5, 3);
    printf("Result: ");
    printf(result);
    return 0;
}
```

**Generated Java:**
```java
public class TranspiledProgram {

    public static int add(int a, int b) {
        return (a + b);
    }

    public static int main() {
        int result = add(5, 3);
        System.out.print("Result: ");
        System.out.print(result);
        return 0;
    }

}
```

## Limitations
The transpiler targets a restricted subset of C and does not currently support:
- Pointers and memory management
- Arrays and structs
- Function pointers
- Switch statements, ternary operator
- Pre/post increment operators (`++`, `--`)
- Complex initializers

## License
Provided as-is for educational and commercial use.
