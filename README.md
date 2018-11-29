## Quack-Compiler
from Quack (programming language) -> C

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
to get the *qcc* executable. usage: ./qcc [filename] [--json=true]\* [--no-debug]\*

The [--json=true] flag will print the program's abstract syntax tree in JSON format to stdout.
The [--no-debug] flag will hide compilation information (compile-stage completion messages).

Type
```
./clean.sh
```
to get rid of the build directory and the qcc executable.

<hr>
