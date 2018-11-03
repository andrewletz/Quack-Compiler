// Andrew Letz
// Last Modified: 10/31/2018
// Acknowledgements: Dr. Michal Young for starter code

#ifndef ASTNODE_H
#define ASTNODE_H

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include "ASTEnum.h"

namespace AST {

    // Json conversion and pretty-printing can pass around a print context object
    // to keep track of indentation, and possibly other things.
    class AST_print_context {
        public:
            int indent_; // Number of spaces to place on left, after each newline
            AST_print_context() : indent_(0) {};
            void indent() { ++indent_; }
            void dedent() { --indent_; }
    };

    class ASTNode {
        public:
            Type type; // represents the type of this node
            std::map<Type, std::vector<ASTNode*> > children; // contains all of the children
            std::vector<Type> order; // tracks the order of input
            
            std::string name; // used for IDENT and STRCONST
            bool nameinit = false;

            int value; // used for INTCONST
            bool valueinit = false;

            // Constructor, Destructor
            ASTNode(Type type) : type(type) {};
            ASTNode(Type type, std::string name) : type(type), name(name), nameinit(true) {};
            ASTNode(Type type, int value) : type(type), value(value), valueinit(true) {};
            virtual ~ASTNode() {};

            // Member insertion and retrieval
            void insert(ASTNode* inputNode);
            ASTNode* get(Type type); // if you know there is only a single node of this type, use get
            std::vector<ASTNode *> getSeq(Type type); // if there can be multiple, use getSeq

            // JSON Printing functions
            void printSelf(std::ostream& out);
            void json(std::ostream& out, AST_print_context& ctx);
            void jsonSeq(std::ostream& out, AST_print_context& ctx);
            void json_indent(std::ostream& out, AST_print_context& ctx);
            void json_head(std::string node_kind, std::ostream& out, AST_print_context& ctx);
            void json_close(std::ostream& out, AST_print_context& ctx);
            void json_child(std::string field, ASTNode& child, std::ostream& out, AST_print_context& ctx, char sep=',');
        };

}

#endif //ASTNODE_H