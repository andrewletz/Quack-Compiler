#
# Makefile for building scanner.  You may prefer to use cmake, especially
# if you use CLion.  I don't know cmake well enough to give you a good
# portable CMakelists.txt
#
# Local tool paths and commands  (Varies by installation)
CCOPTS = -L /usr/local/lib
REFLEX_LIB = /usr/local/lib/libreflex.a
REFLEX_INCLUDE = /usr/local/include/reflex
REFLEX = reflex
BISON = bison
CC = g++ -std=c++11

default: parser

ast: testast

quack.tab.cxx quack.tab.hxx location.hh position.hh stack.hh: quack.yxx
	$(BISON) -d quack.yxx

ASTNode.o:
	$(CC) -c ASTNode.cpp -o ASTNode.o

%.o: %.cpp lex.yy.h
	$(CC) -c -I $(REFLEX_INCLUDE) $^

lex.yy.cpp lex.yy.h: quack.lxx  quack.tab.hxx
	$(REFLEX) --bison-cc --bison-locations --header-file quack.lxx

testast: ASTTester.cpp ASTNode.h ASTNode.o
	$(CC) -o testast $^

parser: quack.tab.cxx lex.yy.cpp lex.yy.h parser.cpp ASTNode.cpp ASTNode.h Messages.h Messages.cpp CodegenContext.cpp CodegenContext.h EvalContext.h
	$(CC) -o parser $^ $(REFLEX_LIB)

# There should be more dependencies for the header files, but I'm lazy.

clean:
	rm -f *.o
	rm -f lex.yy.cpp lex.yy.h
	rm -f lex.yy.h.gch
	rm -f lexer parser testast
	rm -f quack.tab.hxx quack.tab.cxx
	rm -f stack.hh position.hh location.hh

