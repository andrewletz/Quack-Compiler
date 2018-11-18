#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "ASTNode.h"
#include <stack>

// Qmethod is a struct storing everything you need 
// to know about a method for type checking. The info
// is gathered from the ast. Formal arguments are automatically
// put into the type and init vectors, and return is automatically
// put into the type vector upon creation. This is because in Quack
// we can assume arguments are initialized, and return type is static.
struct Qmethod {
    AST::Node *node;
    std::string name;
    std::vector<std::string> init;
    std::map<std::string, std::string> type;
    std::vector<AST::Node*> stmts;
};

struct Qclass {
    AST::Node *node;
    std::string name;
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
        std::map<std::string, std::vector<std::string>> class_hierarchy;

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
        bool isVarInit(Qmethod method, std::string ident);
        bool classHierarchyCheck();

        Qclass createQclass(AST::Node *clazz);
        Qmethod createQmethod(AST::Node *method);

        void printQclass(Qclass clazz);
        void printQmethod(Qmethod method);
    };

#endif