//
// This file was created by hand, only because I was in a hurry.
// The right way to create a file like this is to write a script
// in Awk, Python, or (gasp) Perl to extract the token definitions
// from quack.tab.hxx and emit the header file.  Maybe a couple
// hours work to write and debug, whereas this should take me about
// ten minutes ... but the script would be reusable across many projects,
// whereas this hand-written file will likely break the first time I modify
// the bison file, especially if I add a token.
//

#ifndef QUACKPARSER_NAMES_H
#define QUACKPARSER_NAMES_H

static struct
CLASS = 258,
        DEF = 259,
        EXTENDS = 260,
        IF = 261,
        ELIF = 262,
        ELSE = 263,
        WHILE = 264,
        RETURN = 265,
        TYPECASE = 266,
        ATLEAST = 267,
        ATMOST = 268,
        EQUALS = 269,
        AND = 270,
        OR = 271,
        NOT = 272,
        IDENT = 273,
        INT_LIT = 274,
        STRING_LIT = 275,
        NEG = 276
};



#endif //QUACKPARSER_NAMES_H
