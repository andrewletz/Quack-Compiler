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

    // mainly for type inference/method compatibility
    std::vector<std::string> args;
    std::map<std::string, std::string> argtype;

    // tracking of initialized variables and their types
    std::vector<std::string> init;
    std::vector<std::string> explicitVars;
    std::map<std::string, std::string> type;
    
    std::vector<AST::Node*> stmts;
};

struct Qclass {
    AST::Node *node; // pointer to the node in the tree
    std::string name;
    std::string super;
    Qmethod *constructor;
    std::vector<Qmethod*> methods;    
    // for use in init before use checking in non constructor methods
    std::vector<std::string> instanceVars;
    std::vector<std::string> explicitFields;
    std::map<std::string, std::string> instanceVarType;
};

class Typechecker {
    public:
        /* ============ */
        /* Data Members */
        /* ============ */
        // passed in from the parser, is the root of our AST
        AST::Node *root;
        AST::Node *stubs;

        // map from class name -> struct
        std::map<std::string, Qclass*> classes;
        std::map<std::string, std::vector<std::string>> class_hierarchy;

        // we create a main class to wrap our program's statements
        Qclass* main;

        /* ========================== */
        /* Constructors & Destructors */
        /* ========================== */

        Typechecker(AST::Node *root, AST::Node *stubs) : root(root), stubs(stubs) { initialize(); };
        virtual ~Typechecker() {};

        /* ======= */
        /* Methods */
        /* ======= */

        // grabs everything we need from the AST for type checking
        void initializeClasses(AST::Node *astRoot);
        void initialize();

        // Type checking: phase one
        // - check for circular dependency
        // - check if class method definitions are compatible with parent's
        bool classHierarchyCheck();
        bool methodSignaturesTypecheck();
        bool methodsCompatibleCheck();

        // Type checking: phase two
        // - init before use on constructors
        // - check instance vars of children match parents
        // - init before use on methods
        bool initCheckStmt(Qmethod *method, AST::Node *stmt, 
                            std::vector<std::string> &var_init,
                            std::vector<std::string> &field_init,
                            bool isConstructor, bool isMainStatements);
        bool initCheckQmethod(Qmethod *method, bool isConstructor, bool isMainStatements);
        bool initializeBeforeUseCheck();

        // Type checking: phase three
        // - type inference on constructors
        // - type inference on methods
        std::string typeInferStmt(Qmethod *method, AST::Node *stmt, bool &changed, bool &ret_flag);
        bool typeInferQmethod(Qmethod *method, bool &changed);
        bool typeInferenceCheck();
        bool fieldsCompatibleCheck();

        // Type check our entire program and report errors/bail appropriately
        // this returns true back to driver.cpp if we passed all checks
        bool checkProgram();

        // "helper" methods used throughout type checking
        bool isInstanceVar(Qmethod *method, std::string ident);
        bool isInstanceVarExplicit(Qmethod *method, std::string ident);

        bool isVarInit(Qmethod *method, std::string ident);
        bool isVarArg(Qmethod *method, std::string ident);
        bool isVarExplicit(Qmethod *method, std::string ident);
        bool isBuiltin(std::string classname);
        bool doesClassExist(std::string classname);
        bool isSubclassOrEqual(std::string class1, std::string class2);
        std::string leastCommonAncestor(std::string class1, std::string class2);
        std::string getSuperClass(std::string class1);

        Qclass* createQclass(AST::Node *clazz);
        Qmethod* createQmethod(AST::Node *method, Qclass *containerClass, bool isConstructor);

        void printQclass(Qclass *clazz);
        void printQmethod(Qmethod *method);
    };

#endif