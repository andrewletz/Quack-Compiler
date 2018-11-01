// Andrew Letz
// Last Modified: 10/31/2018
// Acknowledgements: Dr. Michal Young for starter code

#include "ASTNode.h"

namespace AST {
    // Abstract syntax tree.  ASTNode is abstract base class for all other nodes.


    // JSON representation of all the concrete node types.
    // This might be particularly useful if I want to do some
    // tree manipulation in Python or another language.  We'll
    // do this by emitting into a stream.

    // --- Utility functions used by node-specific json output methods


    /* Indent to a given level */
    void ASTNode::json_indent(std::ostream& out, AST_print_context& ctx) {
        if (ctx.indent_ > 0) {
            out << std::endl;
        }
        for (int i=0; i < ctx.indent_; ++i) {
            out << "    ";
        }
    }

    /* The head element looks like { "kind" : "block", */
    void ASTNode::json_head(std::string node_kind, std::ostream& out, AST_print_context& ctx) {
        json_indent(out, ctx);
        out << "{ \"kind\" : \"" << node_kind << "\"," ;
        ctx.indent();  // one level more for children
        return;
    }

    void ASTNode::json_close(std::ostream& out, AST_print_context& ctx) {
        // json_indent(out, ctx);
        out << "}";
        ctx.dedent();
    }

    void ASTNode::json_child(std::string field, ASTNode& child, std::ostream& out, AST_print_context& ctx, char sep) {
        json_indent(out, ctx);
        out << "\"" << field << "\" : ";
        child.json(out, ctx);
        out << sep;
    }

    void ASTNode::json(std::ostream& out, AST_print_context& ctx) {
        // json_head(kind_, out, ctx);
        // out << "\"elements_\" : [";
        // auto sep = "";
        // for (ASTNode *el: elements_) {
        //     out << sep;
        //     el->json(out, ctx);
        //     sep = ", ";
        // }
        // out << "]";
        // json_close(out, ctx);
    }

    void ASTNode::insert(Type type, ASTNode *node) {
        /* Docstring TBD */

        /* If this type of node is not in the map yet, need a new vector for it to be inserted */
        if (children.count(type) == 0) {
            std::vector<ASTNode *> insertedNodeVector;
            insertedNodeVector.push_back(node);

            children.insert(std::pair<Type, std::vector<ASTNode *> >(type, insertedNodeVector));
        }
        /* Else, find that type in the map and push this new node into its vector */
        else {
            std::map<Type, std::vector<ASTNode *> >::iterator it = children.find(type);
            if (it != children.end()) {
                it->second.push_back(node);
            }
        }

        /* If this type of node is being inserted for the first time,  it needs to go
         * into the "order" vector to know when it was inserted, this is for the json
         * printing
         */
        std::vector<Type>::iterator it = std::find(order.begin(), order.end(), type);
        if (it == order.end()) {
            order.push_back(type);
        }
    }

    // void Program::json(std::ostream &out, AST::AST_print_context &ctx) {
    //     json_head("Program", out, ctx);
    //     json_child("classes_", classes_, out, ctx);
    //     json_child("statements_", statements_, out, ctx, ' ');
    //     json_close(out, ctx);
    // }

    // void Ident::json(std::ostream& out, AST_print_context& ctx) {
    //     json_head("Ident", out, ctx);
    //     out << "\"text_\" : \"" << text_ << "\"";
    //     json_close(out, ctx);
    // }
}