// Andrew Letz
// Last Modified: 10/31/2018
// Acknowledgements: Dr. Michal Young for starter code

#ifndef ASTENUM_H
#define ASTENUM_H

#include <set>

enum Type {   
                // Nonterminal Node Types
                PROGRAM, BLOCK,
                CLASSES, CLASS,
                METHODS, METHOD, CONSTRUCTOR, CALL,
                FORMAL_ARG, FORMAL_ARGS, ACTUAL_ARGS,
                ASSIGN, RETURN, BINOP, AND, OR, DOT, LOAD, IF,
                L_EXPR, IDENT, INTCONST, STRCONST,
                TYPECASE, TYPE_ALTERNATIVE,  TYPE_ALTERNATIVES,

                // Identifier/named field Types
                NAME
            };

static const char * TypeString[] = {   
                // Nonterminal Node Types
                "Program", "block_",
                "classes_", "Class",
                "methods_", "Method", "Constructor", "Call",
                "Formal", "formal_args_", "actual_args_",
                "Assign", "Return", "binop_", "and_", "or_", "dot_", "Load", "If",
                "l_expr_", "Ident", "IntConst", "StrConst",
                "Typecase", "Type_Alternative",  "type_alternatives_",

                // Identifier/named field Types
                "name_"
            };

extern std::set<Type> HeadTypes;
extern bool isHeadType(Type type);

extern std::set<Type> SeqTypes;
extern bool isSeqType(Type type);

extern std::string typeString(Type type);

#endif //ASTNODE_H