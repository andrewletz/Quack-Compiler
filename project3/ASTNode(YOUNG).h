//
// Created by Michal Young on 9/12/18.
//

#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <sstream>
#include <vector>
#include "CodegenContext.h"

namespace AST {
    // Abstract syntax tree.  ASTNode is abstract base class for all other nodes.

    // Json conversion and pretty-printing can pass around a print context object
    // to keep track of indentation, and possibly other things.
    class AST_print_context {
    public:
        int indent_; // Number of spaces to place on left, after each newline
        AST_print_context() : indent_{0} {};
        void indent() { ++indent_; }
        void dedent() { --indent_; }
    };

    class ASTNode {
    public:
        virtual void json(std::ostream& out, AST_print_context& ctx) = 0;  // Json string representation
        std::string str() {
            std::stringstream ss;
            AST_print_context ctx;
            json(ss, ctx);
            return ss.str();
        }
    protected:
        void json_indent(std::ostream& out, AST_print_context& ctx);
        void json_head(std::string node_kind, std::ostream& out, AST_print_context& ctx);
        void json_close(std::ostream& out, AST_print_context& ctx);
        void json_child(std::string field, ASTNode& child, std::ostream& out, AST_print_context& ctx, char sep=',');
    };

    class Stub : public ASTNode {
        std::string name_;
    public:
        explicit Stub(std::string name) : name_{name} {}
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    /*
     * Abstract base class for nodes that have sequences
     * of children, e.g., block of statements, sequence of
     * classes.  These may be able to share some operations,
     * especially when applying a method to the sequence just
     * means applying the method to each element of the sequence.
     */
    class Seq : public ASTNode {
    protected:
        std::string kind_;
        std::vector<ASTNode*> elements_;
    public:
        Seq(std::string kind) : kind_{kind}, elements_{std::vector<ASTNode*>()} {}
        void append(ASTNode* el) { elements_.push_back(el); }
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    /* A block is a sequence of statements or expressions.
     * For simplicity we'll just make it a sequence of ASTNode,
     * and leave it to the parser to build valid structures.
     */
    class Block : public Seq {
    public:
        explicit Block() : Seq("Block") {}
     };

    /* A Quack program begins with a sequence of zero or more
     * class definitions.
     */
    class Classes : public Seq {
    public:
        explicit Classes() : Seq("Classes") {}
    };

    /* A program has a set of classes (in any order) and a block of
     * statements.
     */
    class Program : public ASTNode {
        Seq& classes_;
        Seq& statements_;
    public:
        explicit Program(Seq& classes, Seq& statements) :
            classes_{classes}, statements_{statements} {}
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    /* Formal arguments list is a list of
     * identifier: type pairs.
     */
    class Formal : public ASTNode {
        ASTNode& var_;
        ASTNode& type_;
    public:
        explicit Formal(ASTNode& var, ASTNode& type_) :
            var_{var}, type_{type_} {};
        void json(std::ostream& out, AST_print_context&ctx) override;
    };

    class Formals : public Seq {
    public:
        explicit Formals() : Seq("Formals") {}
    };

    class Method : public ASTNode {
        ASTNode& name_;
        Seq& formals_;
        ASTNode& returns_;
        Seq& statements_;
    public:
        explicit Method(ASTNode& name, Seq& formals, ASTNode& returns, Seq& statements) :
          name_{name}, formals_{formals}, returns_{returns}, statements_{statements} {}
        void json(std::ostream& out, AST_print_context&ctx) override;
    };

    class Methods : public Seq {
    public:
        explicit Methods() : Seq("Methods") {}
    };



    /* L_Expr nodes are AST nodes that can be evaluated for location.
     * Most can also be evaluated for value_.  An example of an L_Expr
     * is an identifier, which can appear on the left_ hand or right_ hand
     * side of an assignment.  For example, in x = y, x is evaluated for
     * location and y is evaluated for value_.
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
        /* Abstract base class */
    };

    /* When an expression is an LExpr, we
     * the LExpr denotes a location, and we
     * need to load it.
     */
    class Load : public ASTNode {
        ASTNode& loc_;
    public:
        Load(ASTNode& loc) : loc_{loc} {}
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    /* An assignment has an lvalue (location to be assigned to)
     * and an expression.  We evaluate the expression and place
     * the value_ in the variable.
     */

    class Assign : public ASTNode {
        ASTNode &lexpr_;
        ASTNode &rexpr_;
    public:
        Assign(ASTNode &lexpr, ASTNode &rexpr) :
           lexpr_{lexpr}, rexpr_{rexpr} {}
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    /* 'return' statement returns value from method */
    class Return : public ASTNode {
        ASTNode &expr_;
    public:
        Return(ASTNode& expr) : expr_{expr}  {}
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    class If : public ASTNode {
        ASTNode &cond_; // The boolean expression to be evaluated
        Seq &truepart_; // Execute this block if the condition is true
        Seq &falsepart_; // Execute this block if the condition is false
    public:
        explicit If(ASTNode& cond, Seq& truepart, Seq& falsepart) :
            cond_{cond}, truepart_{truepart}, falsepart_{falsepart} { };
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    class Typecase : public ASTNode {
        ASTNode& expr_; // An expression we want to downcast to a more specific class
        Seq& cases_;    // A case for each potential type
    public:
        explicit Typecase(ASTNode& expr, Seq& cases) :
           expr_{expr}, cases_{cases} {};
        void json(std::ostream& out, AST_print_context& ctx) override;

    };

    class Type_Alternatives : public Seq {
    public:
        explicit Type_Alternatives() : Seq("Type_Alternatives") {}
    };

    class Type_Alternative : public ASTNode {
        ASTNode& ident_;
        ASTNode& class_;
        Seq& block_;
    public:
        explicit Type_Alternative(ASTNode& ident, ASTNode& clazz, Seq& block) :
          ident_{ident}, class_{clazz}, block_{block} {}
        void json(std::ostream& out, AST_print_context& ctx) override;
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
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    /* A class has a name, a list of arguments, and a body
    * consisting of a block (essentially the constructor)
    * and a list of methods.
    */
    class Class : public ASTNode {
        Ident& name_;
        Ident& super_;
        ASTNode& constructor_;
        Seq& methods_;
        // + list of methods, with constructor as just one
        //   more method
    public:
        explicit Class(Ident& name, Ident& super,
                 ASTNode& constructor, Seq& methods) :
            name_{name},  super_{super},
            constructor_{constructor}, methods_{methods} {};
        void json(std::ostream& out, AST_print_context& ctx) override;
    };


    class IntConst : public ASTNode {
        int value_;
    public:
        explicit IntConst(int v) : value_{v} {}
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    class StrConst : public ASTNode {
        std::string value_;
    public:
        explicit StrConst(std::string v) : value_{v} {}
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    /* Method calls are central to type checking and code
     * generation ... and for us, the operators +, -, etc
     * are method calls to specially named methods.
     */
    class Call : public ASTNode {
        ASTNode& receiver_;     /* Expression computing the receiver object */
        ASTNode& method_;  /* Identifier of the method */
        Seq& actuals_;     /* List of actual arguments */
    public:
        explicit Call(ASTNode& receiver, ASTNode& method, Seq& actuals) :
          receiver_{receiver}, method_{method}, actuals_{actuals} {};
        // Convenience factory for the special case of a method
        // created for a binary operator (+, -, etc).
        static Call* binop(std::string opname, ASTNode& receiver, ASTNode& arg);
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    /* Constructors are different from other method calls. They
     * are static (not looked up in the vtable), have no receiver
     * object, and have their own type-checking rules.
     */
    class Construct : public ASTNode {
        ASTNode& method_; /* Method name is same as class name */
        Seq& actuals_;    /* Actual arguments to constructor */
    public:
        explicit Construct(ASTNode& method, Seq& actuals) :
          method_{method}, actuals_{actuals} {}
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

    class Actuals : public Seq {
    public:
        explicit Actuals() : Seq("Actuals") {}
    };

    // Virtual base class for binary operations.
    // Does NOT include +, -, *, /, etc, which
    // are method calls.
    // Does include And, Or, Dot, ...
   class BinOp : public ASTNode {
    protected:
        std::string opsym;
        ASTNode &left_;
        ASTNode &right_;
        BinOp(std::string sym, ASTNode &l, ASTNode &r) :
                opsym{sym}, left_{l}, right_{r} {};
    public:
        void json(std::ostream& out, AST_print_context& ctx) override;
    };

   class And : public BinOp {
   public:
       explicit And(ASTNode& left, ASTNode& right) :
          BinOp("And", left, right) {}
   };

    class Or : public BinOp {
    public:
        explicit Or(ASTNode& left, ASTNode& right) :
                BinOp("Or", left, right) {}
    };



    /* Can a field de-reference (expr . IDENT) be a binary
     * operation?  It can be evaluated to a location (l_exp),
     * whereas an operation like * and + cannot, but maybe that's
     * ok.  We'll tentatively group it with Binop and consider
     * changing it later if we need to make the distinction.
     */

    class Dot : public BinOp {
    public:
        Dot (ASTNode &l, ASTNode &r) :
        BinOp(std::string("Dot"), l, r) {}
    };


}
#endif //ASTNODE_H
