//
// Created by Michal Young on 9/12/18.
//

#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <sstream>
#include <vector>

namespace AST {
    // Abstract syntax tree.  ASTNode is abstract base class for all other nodes.
    // This is not the final AST for Quack ... it's a quick-and-dirty version
    // that I copied over from the calculator example, and it isn't even the
    // final version from the calculator example.

    class ASTNode {
    public:
        virtual std::string str() = 0;
    };

    class EvalContext {
        /* To be filled in later */
    };

    /* A block is a sequence of statements or expressions.
     * For simplicity we'll just make it a sequence of ASTNode,
     * and leave it to the parser to build valid structures.
     */
    class Block : public ASTNode {
        std::vector<ASTNode*> stmts_;
    public:
        explicit Block() : stmts_{std::vector<ASTNode*>()} {}
        void append(ASTNode* stmt) { stmts_.push_back(stmt); }
         std::string str() override {
            std::stringstream ss;
            for (ASTNode *stmt: stmts_) {
                ss << stmt->str() << ";" << std::endl;
            }
            return ss.str();
        }
    };

    /* L_Expr nodes are AST nodes that can be evaluated for location.
     * Most can also be evaluated for value.  An example of an L_Expr
     * is an identifier, which can appear on the left hand or right hand
     * side of an assignment.  For example, in x = y, x is evaluated for
     * location and y is evaluated for value.
     *
     * For now, a location is just a name, because that's what we index
     * the symbol table with.  In a full compiler, locations can be
     * more complex, and typically in code generation we would have
     * LExpr evaluate to an address in a register.
     *
     * LExpr is abstract.  It's only concrete subclass for now is Ident,
     * but in a full OO language we would have LExprs that look like
     * a.b and a[2].
     */
    class LExpr : public ASTNode {
    public:
        virtual std::string l_eval(EvalContext& ctx) = 0;
    };

    /* An assignment has an lvalue (location to be assigned to)
     * and an expression.  We evaluate the expression and place
     * the value_ in the variable.
     */

    class Assign : public ASTNode {
        LExpr &lexpr_;
        ASTNode &rexpr_;
    public:
        Assign(LExpr &lexpr, ASTNode &rexpr) :
           lexpr_{lexpr}, rexpr_{rexpr} {}
        std::string str() override {
            std::stringstream ss;
            ss << lexpr_.str() << " = "
               << rexpr_.str() << ";";
            return ss.str();
        }

    };

    class If : public ASTNode {
        ASTNode &cond_; // The boolean expression to be evaluated
        Block &truepart_; // Execute this block if the condition is true
        Block &falsepart_; // Execute this block if the condition is false
    public:
        explicit If(ASTNode &cond, Block &truepart, Block &falsepart) :
            cond_{cond}, truepart_{truepart}, falsepart_{falsepart} { };
        std::string str() override {
            return "if " + cond_.str() + " {\n" +
                truepart_.str() + "\n" +
                "} else {\n" +
                falsepart_.str() + "\n" +
                "}\n";
        }

    };

    /* Identifiers like x and literals like 42 are the
     * leaves of the AST.  A literal can only be evaluated
     * for value_ (the 'eval' method), but an identifier
     * can also be evaluated for location (when we want to
     * store something in it).
     */
    class Ident : public LExpr {
        std::string text_;
    public:
        explicit Ident(std::string txt) : text_{txt} {}
        std::string str() override { return text_; }
        std::string l_eval(EvalContext& ctx) override { return text_; }
    };

    class IntConst : public ASTNode {
        int value_;
    public:
        explicit IntConst(int v) : value_{v} {}
        std::string str() override { return std::to_string(value_); }
    };

    // Virtual base class for +, -, *, /, etc
    class BinOp : public ASTNode {
    public:
        // each subclass must override the inherited
        // eval() method

    protected:
        ASTNode &left_;
        ASTNode &right_;
        std::string opsym;
        BinOp(std::string sym, ASTNode &l, ASTNode &r) :
                opsym{sym}, left_{l}, right_{r} {};
    public:
        std::string str() {
            std::stringstream ss;
            ss << "(" << left_.str() << " " << opsym << " "
               << right_.str() << ")";
            return ss.str();
        }
    };

    class Plus : public BinOp {
    public:
        Plus(ASTNode &l, ASTNode &r) :
                BinOp(std::string("+"),  l, r) {};
    };

    class Minus : public BinOp {
    public:
        Minus(ASTNode &l, ASTNode &r) :
            BinOp(std::string("-"),  l, r) {};
    };

    class Times : public BinOp {
    public:
        Times(ASTNode &l, ASTNode &r) :
                BinOp(std::string("*"),  l, r) {};
    };

    class Div : public BinOp {
    public:
        Div (ASTNode &l, ASTNode &r) :
                BinOp(std::string("/"),  l, r) {};
    };


}
#endif //ASTNODE_H
