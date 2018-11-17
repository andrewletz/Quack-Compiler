#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "ASTNode.h"

struct Qclass {
    AST::Node a;
};

class Typechecker {
    public:
        /* ============ */
        /* Data Members */
        /* ============ */
        AST::Node *root;

        /* ========================== */
        /* Constructors & Destructors */
        /* ========================== */

        Typechecker(AST::Node *root) : root(root) {};
        virtual ~Typechecker() {};

        /* ======= */
        /* Methods */
        /* ======= */

        bool classHierarchyCheck();
    };

#endif