#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "ASTNode.h"

class Typechecker {
    public:
        /* ============ */
        /* Data Members */
        /* ============ */
        AST::ASTNode *root;

        /* ========================== */
        /* Constructors & Destructors */
        /* ========================== */

        Typechecker(AST::ASTNode *root) : root(root) {};
        virtual ~Typechecker() {};

        /* ======= */
        /* Methods */
        /* ======= */

        bool classHierarchyCheck();
    };

#endif