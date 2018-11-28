//
// Created by Michal Young on 9/14/18.
//

// This should probably be functions (with a static variable for the count)
// rather than a class, or else it has to be a singleton because we want
// one global count. 

#ifndef AST_MESSAGES_H
#define AST_MESSAGES_H

#include "location.hh"
#include <map>
#include <string>

// using preprocessor directives isn't a good thing...  
// but I couldn't think of a better way to do this all over the code.
#define OUT std::cout << "\033[39m"
#define RED std::cerr << "\033[1;91m"
#define YLW std::cerr << "\033[1;93m"
#define GRN std::cerr << "\033[1;92m"
#define END "\033[0m" << std::endl

enum CompStage {
        LEXER, PARSER, CLASSHIERARCHY, INITBEFOREUSE, TYPEINFERENCE, CODEGENERATION,
        TYPECHECKER, PROMPT
};

static const char * StageString[] = {
        "Lexer: ", "Parser: ", "Type Checker: ", "Type Checker: ", "Type Checker: ", "Code Generation: ", 
        "Type Checker: ", "",
};

extern std::string stageString(CompStage stage);

// Error reporting in one place, so that we can count number of errors,
// potentially killing the program if there are too many, and also
// localize decisions like where the error reports go (stdout, stderr, etc)

namespace report {

    // Halt execution if there are too many errors
    void bail(CompStage stage);

    // enables / disables ynotes and gnotes
    void setDebug(bool flag);

    // An error that we can locate in the input
    void error_at(const yy::location& loc, const std::string& msg, CompStage stage);

    // An error that we can't locate in the input
    void error(const std::string& msg, CompStage stage);

    // Track an error without reporting a message
    void trackError(CompStage stage);

    // Additional diagnostic message, does not count against error limit
    void note(const std::string& msg, CompStage stage);

    // Same as note but has green output color
    void gnote(const std::string& msg, CompStage stage);

    // Same as note but has yellow output color
    void ynote(const std::string& msg, CompStage stage);

    // Same as note but has red output color
    void rnote(const std::string& msg, CompStage stage);

    // Is everything ok, or have we encountered errors?
    bool ok();

    // for use in the driver after all stages are complete
    void dynamicBail();
};


#endif //AST_MESSAGES_H
