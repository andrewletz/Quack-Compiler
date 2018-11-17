#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "ASTNode.h"

struct Qmethod {
    AST::Node *node;
    std::string name;
    std::vector<std::string> init;
    std::map<std::string, std::string> type;
    std::vector<AST::Node*> stmts;
};

struct Qclass {
    AST::Node *node;
    std::string super;
    Qmethod constructor;
    std::vector<Qmethod> methods;
};

class Typechecker {
    public:
        /* ============ */
        /* Data Members */
        /* ============ */
        // passed in from the parser, is the root of our AST
        AST::Node *root;
        // map from class name -> struct
        std::map<std::string, Qclass> classes;

        // we create a main class to wrap our program's statements
        Qclass statements;

        /* ========================== */
        /* Constructors & Destructors */
        /* ========================== */

        Typechecker(AST::Node *root) : root(root) { initialize(); };
        virtual ~Typechecker() {};

        /* ======= */
        /* Methods */
        /* ======= */

        void initialize();
        bool classHierarchyCheck();
    };

#endif