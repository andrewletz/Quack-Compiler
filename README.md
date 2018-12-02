## Quack-Compiler
A fully functioning Quack compiler that generates assembly-like C code from a given Quack program.

Created by Andrew Letz and [Nate Letz](https://github.com/touchette).

| Component  | Status |
| ---------- | ------ |
| Lexer | Complete |
| Parser | Complete |
| AST | Complete |
| Initialization before use | Complete |
| Type-checker & type inference | Complete |
| Code generator | Complete |

<hr>

### Quack language decisions
At a few points in developing the compiler, a decision needed to be made regarding what is allowed in the language. This is a list of the decisions I made (almost all of them in the type checking phase).
* Method and class arguments are treated as explicit declarations
* You may not have a field with the same name as a class or method
* You may not have a method with the same name as a class
* You may not include an explicit "return" statement in a constructor
* You cannot pass "this" as an argument in a constructor (as the object may not be completely initialized yet)
* You cannot assign a variable to "this" in a class
* You may not use the keyword "this" inside of the main program statements
* You cannot use the dot operator (field access) on a builtin. eg, "z".foo or 2.bar
* Typecase variables that are introduced must not share a name with any existing variable in that scope
* Dots of form this.this are prohibited

#### Notes about using this compiler
* If the method in which an error occurs has the same name of the class it is in, that means it occurred in the constructor. eg, "invalid type assigned to variable x in method Classname() in class Classname()"
* If an error occurs in either the method or class $MAIN(), that means the error occurred in the main statements area.
* Some errors may be reported more than once in type checking/inference.

<hr>

### Compilation Instructions
Type
```
./build.sh
```
to get the *qcc* executable. 

**note*: you may have to run the command chmod +x build.sh in order to run the script

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

### Demo programs

* good_simple_inherited_method_printing.qk -> shows off dynamic dispatch on executing an inherited method
* good_Pt.qk -> shows off dynamic dispatch on executing Obj's built-in PRINT with Pt's overriden STR
* good_simple_if_and_short_circuit.qk -> shows both if statements and short circuit evaluation
* good_typecase.qk -> shows off typecase working properly, works on all types of objects (obj, nothing, int, user-defined class)
* good_control_flow.qk -> shows off all types of control flow as well as printing based on execution path taken
* good_the_kitchen_sink -> shows off most things in the language, uncomment things to see some errors
* good_typewalk.qk -> shows off crawling up the class hierarchy to infer the right type for a method (abstract semantics)
* bad_typewalk.qk -> same principle as good_typewalk.qk, but with a small error at the top of the class hierarchy

<hr>

#### generateast.sh script
Once you have have the compiled *qcc* binary, you can type
```
./generateast.sh [qcc executable] [filename]
```
to automatically generate the AST image for that file. Output file will be a .png with the same name as the .qk file.

\****note***: you must have json_to_dot.py in the same directory as the script.

\****another note***: you must have graphviz installed on your machine to use this script (gives access to the "dot" command).

#### quack_compiler_testbench.sh
This script was created by another student, Zayd Hammoudeh.

More information can be found on the repository managed by him and the professor Michal Young:

https://github.com/UO-cis561/quack-tests-static

<hr>

#### Credits
Professor Michal Young for several helpful repos that can be found here:

https://github.com/UO-cis561
