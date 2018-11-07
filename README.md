## Quack-Compiler
from Quack (programming language) -> C

| Component  | Status |
| ---------- | ------ |
| Lexer | Complete |
| Parser | Complete |
| AST | Complete-ish |
| Type checker | In progress |
| Code generator | Incomplete |

### Compilation Instructions

## Lexer (/project1)
Type
```
make
```
to get the *lexer* executable. Usage: ./lexer [filename]


Type
```
make clean
```
to get rid of stuff.

<hr>

## Parser (/project2)
Type
```
make

*(on ix-dev, use 'cmake .')
```
to get the *parser* executable. Usage: ./parser < [filename]


type
```
make clean
```
to get rid of stuff

<hr>

## AST (/project3)
Type
```
./build.sh
```
to get the *parser* executable. usage: ./parser < [filename]

This will print the program's abstract syntax tree in JSON format to stdout.

Type
```
./clean.sh
```
to get rid of stuff.

<hr>
