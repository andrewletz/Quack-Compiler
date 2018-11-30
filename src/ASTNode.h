#ifndef ASTNODE_H
#define ASTNODE_H

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include "ASTEnum.h"
#include "Messages.h"

namespace AST {

    // Json conversion and pretty-printing can pass around a print context object
    // to keep track of indentation and possibly other things.
    class AST_print_context {
        public:
            int indent_; // Number of spaces to place on left after each newline
            AST_print_context() : indent_(0) {};
            void indent() { ++indent_; }
            void dedent() { --indent_; }
    };

    class Node {
        public:
            /* ============ */
            /* Data Members */
            /* ============ */

            Type type; // represents the type of this node
            Type subType = UNINITIALIZED;
            std::map<Type, std::vector<Node*> > children; // contains all of the children
            std::vector<Node*> rawChildren;
            std::vector<Type> order; // tracks the order of input
            
            std::string name; // used for IDENT and STRCONST
            bool nameinit = false;

            int value; // used for INTCONST
            bool valueinit = false;

            bool isLastNode = false;
            bool isLastOrder = false;

            bool skip = false; // for type checking (dont want to report the same error more than once)

            /* ========================== */
            /* Constructors & Destructors */
            /* ========================== */

            Node(Type type) : type(type) {}; // used for most nodes
            Node(Type type, std::string name) : type(type), name(name), nameinit(true) {}; // used for strconst
            Node(Type type, Type subType, std::string name) : type(type), subType(subType), name(name), nameinit(true) {}; // used for strconst
            Node(Type type, int value) : type(type), value(value), valueinit(true) {};
            virtual ~Node() {};

            /* ======================== */
            /* Inserting & Getting Data */
            /* ======================== */

            void insert(Node* inputNode);
            Node* get(Type type); // if you know there is only a single node of this type, use get
            Node* getBySubtype(Type type);
            Node* get(Type type, Type subType); // second argument is for subtype
            std::vector<Node *> getAll(Type type); // if there can be multiple, use getAll
            std::vector<Node *> getAll(Type type, Type subType); // second argument is for subtype

            /* ===================== */
            /* JSON Printing Methods */
            /* ===================== */

            void json_indent(std::ostream& out, AST_print_context& ctx);
            void json_head(std::string node_kind, std::ostream& out, AST_print_context& ctx);
            void json_close(std::ostream& out, AST_print_context& ctx);
            void json_child(std::string field, Node* child, std::ostream& out, AST_print_context& ctx, char sep=',');
            void jsonSeq(std::ostream& out, AST_print_context& ctx);

            /* ================ */
            /* Main JSON Method */
            /* ================ */

            void json(std::ostream& out, AST_print_context& ctx);
        };
}

#endif