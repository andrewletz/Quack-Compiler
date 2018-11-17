#include "ASTNode.h"

/* ================================== */
/* Classes & Functions from ASTEnum.h */
/* ================================== */

std::set<Type> SeqTypes = {
                BLOCK, CLASSES, FORMAL_ARGS, METHODS, TYPE_ALTERNATIVES, ACTUAL_ARGS, STATEMENTS,
            };

bool isSeqType(Type type) {
    const bool is_in = SeqTypes.find(type) != SeqTypes.end();
    return is_in;
}

std::string typeString(Type type) {
   return TypeString[type];
}

namespace AST {
    // AST = Abstract Syntax Tree.  Node is abstract base class for all other nodes in the tree.

    /* ======================== */
    /* Inserting & Getting Data */
    /* ======================== */

    void Node::insert(Node *node) {
        // Check for a NULL node before inserting to avoid an insane amount of problems
        if (node == NULL) { return; }

        // If this type of node is not in the map yet, need a new vector for it to be inserted
        if (this->children.count(node->type) == 0) {
            std::vector<Node *> insertedNodeVector;
            node->isLastNode = true;
            insertedNodeVector.push_back(node);

            this->children.insert(std::map<Type, std::vector<Node *> >::value_type(node->type, insertedNodeVector));
        }
        // Else, find that type in the map and push this new node into its vector
        else {
            std::map<Type, std::vector<Node *> >::iterator it = this->children.find(node->type);
            if (it != this->children.end()) {
                if (it->second.size() != 0) {
                    it->second[it->second.size() - 1]->isLastNode = false;
                }

                node->isLastNode = true;
                it->second.push_back(node);
            }
        }

         // If this type of node is being inserted for the first time,  it needs to go
         // into the "order" vector to know when it was inserted, this is for the json
         // printing
        std::vector<Type>::iterator it = std::find(order.begin(), order.end(), node->type);
        if (it == order.end()) {
            order.push_back(node->type);
        }
    }

    Node * Node::get(Type type) {
        // Finds a given type of node in the map, returns the first node in its vector.
        // This is used when getting types of nodes that only have one identifier, such
        // as a class name, etc.
         
        std::map<Type, std::vector<Node *> >::iterator it = this->children.find(type);
        if (it != this->children.end()) {
            return it->second.front();
        } else { }
    }

    Node * Node::get(Type type, Type subType) {
        std::map<Type, std::vector<Node *> >::iterator it = this->children.find(type);
        if (it != this->children.end()) {
            for (Node* node : it->second) {
                if (node->subType == subType) {
                    return node;
                }
            }
        } else { }
    }

    std::vector<Node *> Node::getAll(Type type) {
        // Finds a given type of node in the map, returns its associated vector.
        // This is used when getting types of nodes that are sequences, such as
        // function arguments, etc.

        std::map<Type, std::vector<Node *> >::iterator it = children.find(type);
        if (it != this->children.end()) {
            return it->second;
        } else { }
    }

    std::vector<Node *> Node::getAll(Type type, Type subType) {
        std::map<Type, std::vector<Node *> >::iterator it = children.find(type);
        if (it != this->children.end()) {
            std::vector<Node *> retVec;
            for (Node* node : it->second) {
                if (node->subType == subType) {
                    retVec.push_back(node);
                }
            }
            return retVec;
        } else { }
    }

    /* ===================== */
    /* JSON Printing Methods */
    /* ===================== */

    void Node::json_indent(std::ostream& out, AST_print_context& ctx) {
        if (ctx.indent_ > 0) {
            out << std::endl;
        }
        for (int i=0; i < ctx.indent_; ++i) {
            out << "    ";
        }
    }

    void Node::json_head(std::string node_kind, std::ostream& out, AST_print_context& ctx) {
        json_indent(out, ctx);
        out << "{ \"kind\" : \"" << node_kind << "\", " ;
        ctx.indent();
        return;
    }

    void Node::json_close(std::ostream& out, AST_print_context& ctx) {
        out << "}";
        ctx.dedent();
    }

    void Node::json_child(std::string field, Node* child, std::ostream& out, AST_print_context& ctx, char sep) {
        out << sep;
        json_indent(out, ctx);
        out << "\"" << field << "\" : ";
        child->json(out, ctx);
    }

    void Node::jsonSeq(std::ostream& out, AST_print_context& ctx) {
        json_head(typeString(this->type), out, ctx);
        out << "\"elements_\" : [";
        for (Type t : this->order) {
            std::vector<Node*> subchildren = this->getAll(t);
            for (Node* node : subchildren) {
                node->json(out, ctx);
                if (!node->isLastNode) {
                    out << ",";
                } else if (this->type == BLOCK && 
                           node->isLastNode &&
                           (node->type != this->order[this->order.size() - 1])) {
                    out << ",";
                }
                else {
                    out << "";
                }
            }
        }
        out << "]";
        json_close(out, ctx);
    }

    /* ================ */
    /* Main JSON Method */
    /* ================ */

    void Node::json(std::ostream& out, AST_print_context& ctx) {

        if (isSeqType(this->type)) {
            jsonSeq(out, ctx);
        } else {
            json_head(typeString(this->type), out, ctx);
            
            if (this->nameinit) {
                out << "\"text_\" : \"" << this->name << "\"";
            } else if (this->valueinit) {
                out << "\"value_\" : \"" << this->value << "\"";
            }

            auto sep = ' ';
            for (Type t : this->order) {
                std::vector<Node*> subchildren = this->getAll(t);
                for (Node* node : subchildren) {
                    if (node->subType != UNINITIALIZED) {
                        this->json_child(typeString(node->subType), node, out, ctx, sep);
                    } else {
                        this->json_child(typeString(node->type), node, out, ctx, sep);
                    }
                    
                    sep = ',';
                    // if (this->nameinit) {
                    //     out << "\"text_\" : \"" << node->name << "\"";
                    // } else if (this->valueinit) {
                    //     out << "\"value_\" : \"" << node->value << "\"";
                    // }
                }
            }
            json_close(out, ctx);
        }
    }
}