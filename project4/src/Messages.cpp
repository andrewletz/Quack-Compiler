// Created by Michal Young on 9/15/18.

#include "Messages.h"
#include "location.hh"

std::string stageString(CompStage stage) {
    return StageString[stage];
}

namespace report {

// The error count is global
static int error_count = 0; // How many errors so far? */
const int  error_limit = 5;
static bool debug = true;

void bail(CompStage stage) {
    switch (stage) {
        case 0:
            std::cerr << "\033[1;91mToo many lexer errors, bailing with exit code 4.\033[0m" << std::endl;
            exit(4);
        case 1:
            std::cerr << "\033[1;91mToo many parser errors, bailing with exit code 8.\033[0m" << std::endl;
            exit(8);
        case 2:
            std::cerr << "\033[1;91mClass hierarchy errors found, bailing with exit code 16.\033[0m" << std::endl;
            exit(16);
        case 3:
            std::cerr << "\033[1;91mInitialization before use errors found, bailing with exit code 32.\033[0m" << std::endl;
            exit(32);
        case 4:
            std::cerr << "\033[1;91mType inference errors found, bailing with exit code 64.\033[0m" << std::endl;
            exit(64);
        default:
            exit(1);
    }
}

void setDebug(bool flag) {
    debug = flag;
}

// An error that we can locate in the input
// Note: An error message should look like this to work well
// with IDEs and other tools:
// /path/to/file:32:9: error: expression is not assignable
void error_at(const yy::location& loc, const std::string& msg, CompStage stage) {
    std::cerr << "\033[1;91m" << stageString(stage) << msg << " at " << loc << "\033[0m" << std::endl;
    if (++error_count > error_limit) {
        bail(stage);
    }
}

// An error that we can't locate in the input
void error(const std::string& msg, CompStage stage) {
    std::cerr << "\033[1;91m" << stageString(stage) << msg << "\033[0m" << std::endl;
    if (++error_count > error_limit) {
        bail(stage);
    }
}

// Additional diagnostic message, does not count against error limit
void note(const std::string& msg, CompStage stage) {
    std::cerr << "\033[1;37m" << stageString(stage) << msg << "\033[0m" << std::endl;
}

// Additional diagnostic message, does not count against error limit
void gnote(const std::string& msg, CompStage stage) {
    if (debug) std::cerr << "\033[1;92m" << stageString(stage) << msg << "\033[0m" << std::endl;
}

// Additional diagnostic message, does not count against error limit
void ynote(const std::string& msg, CompStage stage) {
    if (debug) std::cerr << "\033[1;93m" << stageString(stage) << msg << "\033[0m" << std::endl;
}

// Additional diagnostic message, does not count against error limit
void rnote(const std::string& msg, CompStage stage) {
    std::cerr << "\033[1;91m" << stageString(stage) << msg << "\033[0m" << std::endl;
}

// Are we ok?
bool ok() {
    return (error_count == 0);
}

};
