## Quack-Compiler
Generates assembly-like C code from a given Quack program.

| Component  | Status |
| ---------- | ------ |
| Lexer | Complete |
| Parser | Complete |
| AST | Complete |
| Type checker | Nearing completion |
| Code generator | In progress |

<hr>

### Compilation Instructions
Type
```
./build.sh
```
to get the *qcc* executable. 

Usage
```
./qcc [filename] [-json]\* [-verbose]\* [-debug]\* [-ast]\*
```

#### Compiler Flags
**[-json]** flag will print the program's abstract syntax tree in JSON format to stdout.

**[-debug]** flag will compile-stage start/completion messages.

**[-ast]** will skip the initialization before use and type checking stages, allowing you to generate the AST image for an invalid program.

**[-verbose]** will print program information such as the inferred types of variables and class information. 

Type
```
./clean.sh
```
to get rid of the build directory and the qcc executable.

<hr>

#### generateast.sh script
Once you have have the compiled *qcc* binary, you can type
```
./generateast.sh [filename]
```
to automatically generate the AST image for that file. Output file will be a .png with the same name as the .qk file.

\****note***: you must have json_to_dot.py in the same directory as the script and qcc. 

\****another note***: you must have graphviz installed on your machine to use this script (gives access to the "dot" command).

<hr>
