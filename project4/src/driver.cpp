#include "lex.yy.h"
#include "ASTNode.h"
#include "EvalContext.h"
#include "Messages.h"
#include "typechecker.h"
#include <fstream>

class Driver {
  public:

    // Constructor -
    //    reflex::Input : used to create the lexer, which feeds tokens to the parser 
    //    parser        : defined in  quack.tab.hxx & cxx. arguments defined in quack.yxx.
    //    lexer         : used to override the default lexer in bison (yylex) when parsing
    //    &root         : given to the parser and initialized to the root Node when parsing.
    //                    used to pass the Node back here.

    explicit Driver(reflex::Input in) : lexer(in), parser(new yy::parser(lexer, &root)) {
        report::ynote("starting...", LEXER);
        root = nullptr;
    }

    // Destructor
    virtual ~Driver() { delete parser; }

    // parse: attempts to parse the given program (tokenized by input lexer).
    //        root is an Node ** in quack.yxx, used to pass the root back here.
    AST::Node* parse() {
        report::ynote("starting...", PARSER);
        parser->set_debug_level(0); // 0 = no debugging, 1 = full tracing

        // parse() is defined by Bison. 0 = parse success
        int result = parser->parse();
        if (result == 0 && report::ok()) {
            if (root == nullptr) {
                report::error("Root really shouldn't be null here.", PARSER);
                report::bail(PARSER);
            }
            return root; // program was legal
        } else {
            return nullptr; // if the parse fails, we don't want an AST
        }
    }

  private:
    // yy::Lexer is generated by RE\flex, located inside lex.yy.h & cpp.
    // It takes a reflex::Input, and an ostream, which defaults to std::cout.
    yy::Lexer lexer;
    yy::parser *parser; // our parser class generated by Bison and holds our parse() function
    AST::Node *root;

};

void printUsage() {
    report::rnote("Usage: ./qcc [filename].qk", PROMPT);
    report::rnote("\t*use flag: --json=true for JSON output", PROMPT);
    report::rnote("\t*use flag: --no-debug to hide compile stage messages", PROMPT);
}

int main(int argc, char *argv[]) {
    if (argc > 4) {
        report::rnote("Invalid number of arguments.", PROMPT);
        printUsage();
        report::bail(PROMPT);
    }

    std::string filename;
    bool json = false;

    // Get our filename arg and optional json flag
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--json=true") == 0) {
            json = true;
            report::setDebug(false);
        } else if (std::strcmp(argv[i], "--no-debug") == 0) {
            report::setDebug(false);
        } else {
            filename = std::string(argv[i]);
        }
    }

    // Open file to pass to Driver
    std::ifstream file;
    file.open(filename);

    if (!file.is_open()) {
        std::cerr << "\033[1;91m" << "Invalid file or flag \"" << filename << "\"\033[0m" << std::endl;
        printUsage();
        exit(1);
    }

    Driver driver(file);
    
    // Parse and get AST into *root
    AST::Node* root = driver.parse();
    if (root != nullptr) {
        report::gnote("complete.", LEXER);
        report::gnote("complete.", PARSER);
        AST::AST_print_context context;

        if (json) {
            root->json(std::cout, context);
            std::cout << std::endl;
        } 

        // begin type checking on our non-null AST
        report::ynote("starting...", TYPECHECKER);
        Typechecker typeChecker(root);
        bool programValid = typeChecker.checkProgram();

        if (programValid) report::gnote("complete.", TYPECHECKER);
        // if programValid is false it should have bailed in the type checker

        exit(0);

    } else {
        // either the parse has failed, or no AST was built.
        report::rnote("compilation failed - abstract syntax tree could not be generated!", PARSER);
        report::bail(PARSER);
    }

    file.close();
}