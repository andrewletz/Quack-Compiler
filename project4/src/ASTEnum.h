#ifndef ASTENUM_H
#define ASTENUM_H

#include <set>

enum Type {   
                // Non-Terminal Node Types (And Some Extras)
                UNINITIALIZED,
                PROGRAM, BLOCK,
                CLASSES, CLASS,
                METHODS, METHOD, CONSTRUCTOR, CALL,
                FORMAL_ARGS, ACTUAL_ARGS_EXTRA, ACTUAL_ARGS,
                ASSIGN, RETURN, BINOP, AND, OR, NOT, DOT, LOAD, IF,
                L_EXPR, IDENT, INTCONST, STRCONST, WHILE, 
                TYPECASE, TYPE_ALTERNATIVE,  TYPE_ALTERNATIVES,

                CLASS_NAME, CLASS_ARG, VAR_IDENT, TYPE_IDENT, SUPER_NAME,
                METHOD_NAME, RETURN_TYPE, STATEMENTS,
            };

static const char * TypeString[] = {   
                // Nonterminal Node Types (And Some Extras)
                "UNINITIALIZED", 
                "program_", "block_",
                "classes_", "class_",
                "methods_", "method_", "constructor_", "call_",
                "formal_args_", "actual_args_extra_", "actual_args_",
                "assign_", "return_", "binop_", "and_", "or_", "not_", "dot_", "load_", "if_",
                "l_expr_", "ident_", "intconst_", "strconst_", "while_", 
                "typecase_", "type_alternative_",  "type_alternatives_",

                "class_name_", "class_arg_", "var_name_", "var_type_", "super_name_",
                "method_name_", "return_type_", "statements_",
            };

extern std::set<Type> SeqTypes;
extern bool isSeqType(Type type);

extern std::string typeString(Type type);

#endif