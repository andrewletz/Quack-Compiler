// Created by Michal Young on 9/15/18.

#include "Messages.h"
#include "location.hh"

std::string stageString(CompStage stage) {
    return StageString[stage];
}

namespace report {

// The error count is global
const int  error_limit = 4;
static bool debug = true;

std::map<CompStage, int> error_count {
    {LEXER, 0},
    {PARSER, 0},
    {CLASSHIERARCHY, 0},
    {INITBEFOREUSE, 0},
    {TYPEINFERENCE, 0},
    {CODEGENERATION, 0},
    {TYPECHECKER, 0}
};

void bail(CompStage stage) {
    switch (stage) {
        case 0:
            RED << "Bailing with exit code 4 (SCANNER)." << END;
            exit(4);
        case 1:
            RED << "Bailing with exit code 8 (PARSER)." << END;
            exit(8);
        case 2:
            RED << "Bailing with exit code 16 (CLASS_HIERARCHY)." << END;
            exit(16);
        case 3:
            RED << "Bailing with exit code 32 (INIT_BEFORE_USE)." << END;
            exit(32);
        case 4:
            RED << "Bailing with exit code 64 (TYPE_INFERENCE)." << END;
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
    RED << stageString(stage) << msg << " at " << loc << "" << END;
    if (++error_count[stage] > error_limit) {
        bail(stage);
    }
}

// An error that we can't locate in the input
void error(const std::string& msg, CompStage stage) {
    RED << stageString(stage) << msg << "" << END;
    if (++error_count[stage] > error_limit) {
        bail(stage);
    }
}

void trackError(CompStage stage) {
    if (++error_count[stage] > error_limit) {
        bail(stage);
    }
}

// Additional diagnostic message, does not count against error limit
void note(const std::string& msg, CompStage stage) {
    std::cerr << "\033[1;37m" << stageString(stage) << msg << "" << END;
}

// Additional diagnostic message, does not count against error limit
void gnote(const std::string& msg, CompStage stage) {
    if (debug) GRN << stageString(stage) << msg << "" << END;
}

// Additional diagnostic message, does not count against error limit
void ynote(const std::string& msg, CompStage stage) {
    if (debug) YLW << stageString(stage) << msg << "" << END;
}

// Additional diagnostic message, does not count against error limit
void rnote(const std::string& msg, CompStage stage) {
    RED << stageString(stage) << msg << "" << END;
}

// goes through the error_count map looking for where errors occurred.
// bails with the exit code of the place where the first error occurred.
// (if an error occurs in the scanner and in the parser, the program
// will bail with code 4, for scanner)
void dynamicBail() {
    bool shouldBail = false;
    bool firstErrorSet = false;
    CompStage firstError;

    for (auto stage : error_count) {
        if (stage.second > 0) {
            if (!firstErrorSet) {
                firstErrorSet = true;
                firstError = stage.first;
            }
            shouldBail = true;
        }
    }

    if (shouldBail) {
        bail(firstError);
    }
}

// Are we ok?
bool ok() {
    for (auto stage : error_count) {
        if (stage.second > 0) {
            return false;
        }
    }
    return true;
}

};
