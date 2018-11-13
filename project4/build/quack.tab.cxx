// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.

#line 37 "quack.tab.cxx" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "quack.tab.hxx"

// User implementation prologue.

#line 51 "quack.tab.cxx" // lalr1.cc:412
// Unqualified %code blocks.
#line 19 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:413

    #include "lex.yy.h"
    #undef yylex
    #define yylex lexer.yylex  // Within Bison's parse() we should invoke lexer.yylex(), not the global yylex()
    void dump(AST::ASTNode* n);

#line 60 "quack.tab.cxx" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
#line 146 "quack.tab.cxx" // lalr1.cc:479

  /// Build a parser object.
  parser::parser (yy::Lexer& lexer_yyarg, AST::ASTNode** root_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      lexer (lexer_yyarg),
      root (root_yyarg)
  {}

  parser::~parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  parser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  parser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  parser::symbol_number_type
  parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.type_get ());
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 82 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { *root = new AST::ASTNode(PROGRAM); (yylhs.value.node) = *root; (yylhs.value.node)->insert((yystack_[1].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 589 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 3:
#line 86 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yystack_[1].value.node)->insert((yystack_[0].value.node)); (yylhs.value.node) = (yystack_[1].value.node); }
#line 595 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 4:
#line 87 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(CLASSES); }
#line 601 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 5:
#line 91 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = (yystack_[1].value.node); (yystack_[1].value.node)->insert((yystack_[0].value.node)); }
#line 607 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 6:
#line 95 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(CLASS); (yylhs.value.node)->insert((yystack_[3].value.node));(yylhs.value.node)->insert((yystack_[1].value.node)); }
#line 613 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 7:
#line 96 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(CLASS); (yylhs.value.node)->insert((yystack_[5].value.node)); (yylhs.value.node)->insert((yystack_[3].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 619 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 8:
#line 100 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BLOCK); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node)); }
#line 625 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 9:
#line 104 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(FORMAL_ARGS); (yylhs.value.node)->insert((yystack_[3].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 631 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 10:
#line 105 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = NULL; }
#line 637 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 11:
#line 109 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(FORMAL_ARGS_EXTRA); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 643 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 12:
#line 110 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = NULL; }
#line 649 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 13:
#line 114 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BLOCK); (yylhs.value.node)->insert((yystack_[1].value.node)); }
#line 655 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 14:
#line 118 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yystack_[1].value.node)->insert((yystack_[0].value.node)); (yylhs.value.node) = (yystack_[1].value.node); }
#line 661 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 15:
#line 119 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BLOCK); }
#line 667 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 16:
#line 123 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(IF); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 673 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 17:
#line 124 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(IF); (yylhs.value.node)->insert((yystack_[4].value.node)); (yylhs.value.node)->insert((yystack_[3].value.node)); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 679 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 18:
#line 125 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(WHILE); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 685 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 19:
#line 126 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(ASSIGN); (yylhs.value.node)->insert((yystack_[3].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node)); }
#line 691 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 20:
#line 127 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(ASSIGN); (yylhs.value.node)->insert((yystack_[5].value.node)); (yylhs.value.node)->insert((yystack_[3].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node)); }
#line 697 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 21:
#line 128 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = (yystack_[1].value.node); }
#line 703 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 22:
#line 129 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(RETURN); (yylhs.value.node)->insert((yystack_[1].value.node)); }
#line 709 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 23:
#line 130 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(RETURN); }
#line 715 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 24:
#line 131 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = (yystack_[0].value.node); }
#line 721 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 25:
#line 135 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yystack_[1].value.node)->insert((yystack_[0].value.node)); (yylhs.value.node) = (yystack_[1].value.node); }
#line 727 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 26:
#line 136 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(IF); }
#line 733 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 27:
#line 140 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(IF); (yylhs.value.node)->insert((yystack_[1].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 739 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 28:
#line 144 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(TYPECASE); (yylhs.value.node)->insert((yystack_[3].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node)); }
#line 745 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 29:
#line 148 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yystack_[1].value.node)->insert((yystack_[0].value.node)); (yylhs.value.node) = (yystack_[1].value.node); }
#line 751 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 30:
#line 149 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(TYPE_ALTERNATIVES); }
#line 757 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 31:
#line 153 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(TYPE_ALTERNATIVE); (yylhs.value.node)->insert((yystack_[3].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 763 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 32:
#line 157 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(STRCONST, std::string((yystack_[0].value.str))); }
#line 769 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 33:
#line 158 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(INTCONST, (yystack_[0].value.num)); }
#line 775 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 34:
#line 159 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = (yystack_[0].value.node); }
#line 781 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 35:
#line 160 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "UMINUS"); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 787 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 36:
#line 161 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "MULT"); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 793 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 37:
#line 162 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "DIV"); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 799 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 38:
#line 163 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "PLUS"); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 805 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 39:
#line 164 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "MINUS"); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 811 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 40:
#line 165 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "EQ"); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 817 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 41:
#line 166 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "ATMOST"); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 823 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 42:
#line 167 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "LESSER"); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node));}
#line 829 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 43:
#line 168 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "ATLEAST"); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 835 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 44:
#line 169 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(BINOP, "GREATER"); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 841 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 45:
#line 170 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(AND); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 847 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 46:
#line 171 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(NOT); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 853 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 47:
#line 172 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(OR); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 859 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 48:
#line 173 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = (yystack_[1].value.node); }
#line 865 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 49:
#line 174 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(DOT); (yylhs.value.node)->insert((yystack_[5].value.node)); (yylhs.value.node)->insert((yystack_[3].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node));}
#line 871 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 50:
#line 175 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(CALL); (yylhs.value.node)->insert((yystack_[3].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node)); }
#line 877 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 51:
#line 179 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(ACTUAL_ARGS); (yylhs.value.node)->insert((yystack_[1].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 883 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 52:
#line 180 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = NULL; }
#line 889 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 53:
#line 184 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(ACTUAL_ARGS_EXTRA); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 895 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 54:
#line 185 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = NULL; }
#line 901 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 55:
#line 189 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(L_EXPR); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 907 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 56:
#line 190 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(L_EXPR); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 913 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 57:
#line 194 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(METHOD); (yylhs.value.node)->insert((yystack_[4].value.node)); (yylhs.value.node)->insert((yystack_[2].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 919 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 58:
#line 195 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(METHOD); (yylhs.value.node)->insert((yystack_[6].value.node)); (yylhs.value.node)->insert((yystack_[4].value.node)); (yylhs.value.node)->insert((yystack_[1].value.node)); (yylhs.value.node)->insert((yystack_[0].value.node)); }
#line 925 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 59:
#line 199 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yystack_[1].value.node)->insert((yystack_[0].value.node)); (yylhs.value.node) = (yystack_[1].value.node); }
#line 931 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 60:
#line 200 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(METHODS); }
#line 937 "quack.tab.cxx" // lalr1.cc:859
    break;

  case 61:
#line 204 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:859
    { (yylhs.value.node) = new AST::ASTNode(IDENT, (yystack_[0].value.str)); }
#line 943 "quack.tab.cxx" // lalr1.cc:859
    break;


#line 947 "quack.tab.cxx" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char parser::yypact_ninf_ = -36;

  const signed char parser::yytable_ninf_ = -1;

  const short int
  parser::yypact_[] =
  {
     -36,     1,    18,   -36,    25,   -36,   -19,   118,   -36,    27,
     -36,   -36,   -36,   -36,   120,   120,    36,   120,   120,   120,
     120,   -36,   -36,   137,   -16,    28,    25,   118,   201,   -36,
     201,   -36,   153,   220,   272,   -36,   239,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,    25,   -36,
      25,   120,   120,    16,    14,     8,   -36,   -36,   -36,   -36,
     -36,   -36,    26,    26,    26,   272,   272,    26,    26,   -11,
     -11,    31,    31,    32,    29,   169,   257,    30,    56,    25,
      25,   -36,   -36,    95,    17,    -8,   120,   120,   -36,    34,
     -36,    25,   -36,    37,   -36,   120,    38,   -36,   -36,   -36,
      40,    33,   185,   120,   -36,    47,    25,   201,   -36,    25,
     -36,   -36,   257,    25,    41,   -36,    38,    49,   -13,   -36,
      25,    25,   -36,   -36,    38,   -36
  };

  const unsigned char
  parser::yydefact_[] =
  {
       4,     0,    15,     1,     0,     3,     0,     2,    61,     0,
      15,     5,    32,    33,     0,     0,     0,     0,     0,     0,
       0,    14,    24,     0,    34,    55,    10,    60,     0,    34,
       0,    23,     0,     0,    46,    35,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
       0,     0,    52,     0,     0,     0,    15,    26,    18,    22,
      30,    48,    43,    41,    40,    45,    47,    42,    44,    39,
      38,    36,    37,    56,     0,     0,    54,     0,     6,     0,
       0,     8,    59,     0,    16,     0,    52,     0,    19,    51,
      50,     0,    12,     0,    13,     0,     0,    25,    28,    29,
       0,     0,     0,     0,     7,     9,    10,     0,    17,     0,
      49,    20,    53,     0,     0,    27,     0,     0,     0,    31,
       0,     0,    57,    11,     0,    58
  };

  const signed char
  parser::yypgoto_[] =
  {
     -36,   -36,   -36,   -36,   -36,   -36,   -35,   -36,   -28,    -7,
     -36,   -36,   -36,   -36,   -36,   -36,    -9,    -2,   -36,    -3,
     -36,   -36,    -4
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     1,     2,     5,     6,    11,    53,   105,    57,     7,
      21,    84,    97,    22,    85,    99,    23,    77,    89,    29,
      82,    55,    25
  };

  const unsigned char
  parser::yytable_[] =
  {
       9,     3,    58,    27,    24,    28,    30,    32,    33,    34,
      35,    36,     8,    10,    80,    46,    47,    48,    50,    56,
      51,   121,    54,     4,    24,    98,    95,    96,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    12,
      13,    81,    75,    76,    73,     8,    74,    78,    79,    83,
      44,    45,    46,    47,    48,    18,     8,    26,    52,    48,
      19,    90,    86,    91,   110,    87,    20,   106,   108,   103,
      56,   114,   118,    31,   109,    92,    93,    76,   102,   115,
      24,   100,   113,   120,   101,     0,   107,   104,   119,     0,
     122,     0,     0,     0,   112,     0,   125,     0,    12,    13,
       0,     0,    54,    14,     0,   116,    15,    16,    17,   117,
       0,     0,     0,     0,    18,     8,   123,   124,     0,    19,
       0,    12,    13,    12,    13,    20,    14,     0,    94,    15,
      16,    17,     0,     0,     0,     0,     0,    18,     8,    18,
       8,     0,    19,     0,    19,     0,     0,     0,    20,     0,
      20,    37,    38,    39,    40,    41,     0,     0,     0,    42,
      43,    44,    45,    46,    47,    48,     0,    37,    38,    39,
      40,    41,     0,     0,    49,    42,    43,    44,    45,    46,
      47,    48,     0,    37,    38,    39,    40,    41,     0,     0,
      59,    42,    43,    44,    45,    46,    47,    48,     0,    37,
      38,    39,    40,    41,     0,     0,    88,    42,    43,    44,
      45,    46,    47,    48,     0,    37,    38,    39,    40,    41,
       0,     0,   111,    42,    43,    44,    45,    46,    47,    48,
       0,     0,     0,    56,    37,    38,    39,    40,    41,     0,
       0,     0,    42,    43,    44,    45,    46,    47,    48,     0,
       0,     0,    60,    37,    38,    39,    40,    41,     0,     0,
       0,    42,    43,    44,    45,    46,    47,    48,     0,     0,
      61,    37,    38,    39,    40,    41,     0,     0,     0,    42,
      43,    44,    45,    46,    47,    48,    37,    38,    39,     0,
       0,     0,     0,     0,    42,    43,    44,    45,    46,    47,
      48
  };

  const signed char
  parser::yycheck_[] =
  {
       4,     0,    30,    10,     7,    14,    15,    16,    17,    18,
      19,    20,    20,    32,     6,    26,    27,    28,    34,    32,
      36,    34,    26,     5,    27,    33,     9,    10,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,     3,
       4,    33,    51,    52,    48,    20,    50,    31,    34,    56,
      24,    25,    26,    27,    28,    19,    20,    30,    30,    28,
      24,    31,    30,     7,    31,    36,    30,    30,    96,    35,
      32,   106,    31,    37,    34,    79,    80,    86,    87,   107,
      83,    85,    35,    34,    86,    -1,    95,    91,   116,    -1,
     118,    -1,    -1,    -1,   103,    -1,   124,    -1,     3,     4,
      -1,    -1,   106,     8,    -1,   109,    11,    12,    13,   113,
      -1,    -1,    -1,    -1,    19,    20,   120,   121,    -1,    24,
      -1,     3,     4,     3,     4,    30,     8,    -1,    33,    11,
      12,    13,    -1,    -1,    -1,    -1,    -1,    19,    20,    19,
      20,    -1,    24,    -1,    24,    -1,    -1,    -1,    30,    -1,
      30,    14,    15,    16,    17,    18,    -1,    -1,    -1,    22,
      23,    24,    25,    26,    27,    28,    -1,    14,    15,    16,
      17,    18,    -1,    -1,    37,    22,    23,    24,    25,    26,
      27,    28,    -1,    14,    15,    16,    17,    18,    -1,    -1,
      37,    22,    23,    24,    25,    26,    27,    28,    -1,    14,
      15,    16,    17,    18,    -1,    -1,    37,    22,    23,    24,
      25,    26,    27,    28,    -1,    14,    15,    16,    17,    18,
      -1,    -1,    37,    22,    23,    24,    25,    26,    27,    28,
      -1,    -1,    -1,    32,    14,    15,    16,    17,    18,    -1,
      -1,    -1,    22,    23,    24,    25,    26,    27,    28,    -1,
      -1,    -1,    32,    14,    15,    16,    17,    18,    -1,    -1,
      -1,    22,    23,    24,    25,    26,    27,    28,    -1,    -1,
      31,    14,    15,    16,    17,    18,    -1,    -1,    -1,    22,
      23,    24,    25,    26,    27,    28,    14,    15,    16,    -1,
      -1,    -1,    -1,    -1,    22,    23,    24,    25,    26,    27,
      28
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,    39,    40,     0,     5,    41,    42,    47,    20,    60,
      32,    43,     3,     4,     8,    11,    12,    13,    19,    24,
      30,    48,    51,    54,    57,    60,    30,    47,    54,    57,
      54,    37,    54,    54,    54,    54,    54,    14,    15,    16,
      17,    18,    22,    23,    24,    25,    26,    27,    28,    37,
      34,    36,    30,    44,    60,    59,    32,    46,    46,    37,
      32,    31,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    60,    60,    54,    54,    55,    31,    34,
       6,    33,    58,    47,    49,    52,    30,    36,    37,    56,
      31,     7,    60,    60,    33,     9,    10,    50,    33,    53,
      60,    55,    54,    35,    60,    45,    30,    54,    46,    34,
      31,    37,    54,    35,    44,    46,    60,    60,    31,    46,
      34,    34,    46,    60,    60,    46
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    38,    39,    40,    40,    41,    42,    42,    43,    44,
      44,    45,    45,    46,    47,    47,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    49,    49,    50,    51,    52,
      52,    53,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    55,    55,    56,    56,    57,    57,    58,    58,    59,
      59,    60
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,     2,     2,     0,     2,     5,     7,     4,     4,
       0,     5,     0,     3,     2,     0,     4,     6,     3,     4,
       6,     2,     3,     2,     1,     2,     0,     3,     5,     2,
       0,     4,     1,     1,     1,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     3,     6,
       4,     2,     0,     3,     0,     1,     3,     6,     8,     2,
       0,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "$end", "error", "$undefined", "STRING_LIT", "INT_LIT", "CLASS", "DEF",
  "EXTENDS", "IF", "ELIF", "ELSE", "WHILE", "RETURN", "TYPECASE",
  "ATLEAST", "ATMOST", "EQUALS", "AND", "OR", "NOT", "IDENT", "NEG", "'<'",
  "'>'", "'-'", "'+'", "'*'", "'/'", "'.'", "UMINUS", "'('", "')'", "'{'",
  "'}'", "':'", "','", "'='", "';'", "$accept", "program", "classes",
  "class", "class_sig", "class_body", "formal_args", "formal_args_extra",
  "stmt_block", "stmts", "stmt", "elif_nts", "elif_nt", "typecase",
  "type_alts", "type_alt", "r_expr", "actual_args", "actual_args_extra",
  "l_expr", "method", "methods", "ident", YY_NULLPTR
  };


  const unsigned char
  parser::yyrline_[] =
  {
       0,    82,    82,    86,    87,    91,    95,    96,   100,   104,
     105,   109,   110,   114,   118,   119,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   135,   136,   140,   144,   148,
     149,   153,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   179,   180,   184,   185,   189,   190,   194,   195,   199,
     200,   204
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  parser::token_number_type
  parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      30,    31,    26,    25,    35,    24,    28,    27,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    34,    37,
      22,    36,    23,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,    33,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    29
    };
    const unsigned int user_token_number_max_ = 277;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // yy
#line 1394 "quack.tab.cxx" // lalr1.cc:1167
#line 207 "/mnt/c/Users/andre/Desktop/College/461 - Compiler Construction/Quack-Compiler/project4/src/quack.yxx" // lalr1.cc:1168


// -~-~-~- -~-~-~- -~-~-~-~

#include "Messages.h"

void yy::parser::error(const location_type& loc, const std::string& msg)
{
    report::error_at(loc, msg);
}
