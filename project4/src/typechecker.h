#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <stack>
#include "ASTNode.h"
#include "Messages.h"

// Qmethod is a struct storing everything you need 
// to know about a method for type checking. The info
// is gathered from the ast. Formal arguments are automatically
// put into the type and init vectors, and return is automatically
// put into the type vector upon creation. This is because in Quack
// we can assume arguments are initialized, and return type is static.

struct Qclass; // needs to forward declared as each struct holds the other in some form

struct Qmethod {
    AST::Node *node; // pointer to the node in the tree

    // The reference to the containing class of a method is a pointer because
    // it may not be fully initialized when passed into any given Qmethod.
    // This is okay because we don't use *clazz until after all initialization is complete.
    Qclass *clazz;
    
    std::string name;
    std::vector<std::string> init;
    std::map<std::string, std::string> type;
    std::vector<AST::Node*> stmts;
};

struct Qclass {
    AST::Node *node; // pointer to the node in the tree
    std::string name;
    std::string super;
    Qmethod constructor;
    std::vector<Qmethod> methods;

    // for use in init before use checking in non constructor methods
    std::set<std::string> instanceVars; 
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

        // grabs everything we need from the AST for type checking
        void initialize();

        // Type checking: phase one
        // - check for circular dependency
        // - check if class method definitions are compatible with parent's
        // (check if class extends no such super is done in initialize())
        bool classHierarchyCheck();
        bool methodsCompatibleCheck();

        // Type checking: phase two
        // - init before use on constructors
        // - check instance vars of children match parents
        // - init before use on methods
        bool initializeBeforeUseCheck();

        // Type checking: phase three
        // - type inference on constructors
        // - type inference on methods
        bool typeInferenceCheck();

        // Type check our entire program and report errors/bail appropriately
        // this returns true back to driver.cpp if we passed all checks
        bool checkProgram();

        // "helper" methods used throughout type checking
        bool isVarInit(Qmethod method, std::string ident);
        bool isSubclassOrEqual(std::string class1, std::string class2);
        std::string leastCommonAncestor(std::string class1, std::string class2);
        std::string getSuperClass(std::string class1);

        Qclass createQclass(AST::Node *clazz);
        Qmethod createQmethod(AST::Node *method, Qclass *containerClass);

        void printQclass(Qclass clazz);
        void printQmethod(Qmethod method);
    };

#endif