#ifndef ASTENUM_H
#define ASTENUM_H

#include <set>

enum Type {   
                // Non-Terminal Node Types (And Some Extras)
                PROGRAM, BLOCK,
                CLASSES, CLASS,
                METHODS, METHOD, CONSTRUCTOR, CALL,
                FORMAL_ARGS, FORMAL_ARGS_EXTRA, ACTUAL_ARGS_EXTRA, ACTUAL_ARGS,
                ASSIGN, RETURN, BINOP, AND, OR, NOT, DOT, LOAD, IF,
                L_EXPR, IDENT, INTCONST, STRCONST, WHILE, 
                TYPECASE, TYPE_ALTERNATIVE,  TYPE_ALTERNATIVES,
            };

static const char * TypeString[] = {   
                // Nonterminal Node Types (And Some Extras)
                "program_", "block_",
                "classes_", "class_",
                "methods_", "method_", "constructor_", "call_",
                "formal_arg_", "formal_args_", "actual_args_extra_", "actual_args_",
                "assign_", "return_", "binop_", "and_", "or_", "not_", "dot_", "load_", "if_",
                "l_expr_", "ident_", "intconst_", "strconst_", "while_", 
                "typecase_", "type_alternative_",  "type_alternatives_",
            };

extern std::set<Type> SeqTypes;
extern bool isSeqType(Type type);

extern std::string typeString(Type type);

#endif