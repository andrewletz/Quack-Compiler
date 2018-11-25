#ifndef STUBS_H
#define STUBS_H

#include <string>

const std::string STUBS =         "\n        /*\n         * Stubs for built-in classes of Quack. We do not generate code\n         * for these.  These are used only for static semantics checking.\n         * Their real code is hand-written C code.\n         */\n\n         class Obj() {\n            def PRINT() { }\n            def STR() : String { }\n            def EQUALS(other: Obj): Boolean {} // Default is object identity\n         }\n\n         class Nothing() { }\n\n         class String() {\n            def PLUS(other: String): String { }      // +\n            def EQUALS(other: Obj): Boolean { }   // ==\n            def ATMOST(other: String): Boolean { }   // <=\n            def LESS(other: String): Boolean { }     // <\n            def ATLEAST(other: String): Boolean { }  // >=\n            def MORE(other: String): Boolean { }     // >\n         }\n\n         class Boolean() { }\n\n         class Int() {\n            def PLUS(right: Int): Int {}   // this + right\n            def EQUALS(other: Obj): Boolean { }   // ==\n            def TIMES(right: Int): Int {}  // this * right\n            def MINUS(right: Int): Int {}  // this - right\n            def DIVIDE(right: Int): Int {}    // this / right\n            def ATMOST(other: Int): Boolean { }     // <=\n            def LESS(other: Int): Boolean { }       // <\n            def ATLEAST(other: Int): Boolean { }    // >=\n            def MORE(other: Int): Boolean { }       // >\n         }\n        ";

#endif //STUBS_H
