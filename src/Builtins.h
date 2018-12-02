/*
 * The Quack runtime needs a base class Obj and a handful 
 * of built-in classes  (Int, String, Nothing, Boolean)
 *
 * 2018 note (FIXME): The class structure needs a field that 
 * points to the supertype.  
 */
#ifndef Builtins_h
#define Builtins_h

/* Naming conventions:  
 * class_X means a reference to the class structure for class X, 
 * i.e., pointer to the struct that contains the method table. 
 * 
 * obj_X means the corresponding object structure reference, 
 * i.e., a pointer to the struct where instance variables live.
 * 
 * For each, we'll define a struct tag with *_struct and
 * and then define a reference to that struct with a typedef, 
 * e.g., class_Obj will be typedefed to struct class_Obj_struct *. 
 * Each class will have a single instantiation as the_class_X. 
 * the_class_X is a pointer to a statically allocated structure. 
 * 
 * All programmer-visible fields of objects are references to 
 * other objects. Boolean, Int, and String have "hidden" fields 
 * that contain their unboxed primitives. 
 * Classes are essentially vectors of function pointers, but for 
 * convenience and checking we declare them as (pointers to) 
 * structs containing function pointers with particular signatures. 
 * The receiver object ('this' in Quack) is an implicit argument 
 * in Quack but an explicit argument in the runtime. 
 */ 

/* The following object types are "known" from Obj, in the 
 * sense that there are Obj methods that return these types. 
 */
struct class_Nothing_struct;
typedef struct class_Nothing_struct* class_Nothing; 

typedef struct obj_Nothing_struct {
    class_Nothing clazz;
} * obj_Nothing;

struct obj_String_struct;
typedef struct obj_String_struct* obj_String;

struct obj_Boolean_struct;
typedef struct obj_Boolean_struct* obj_Boolean;

/* ==============
 * Obj 
 * Fields: None
 * Methods: 
 *    Constructor  (called after allocation) 
 *    STRING 
 *    PRINT
 *    EQUALS
 * 
 * ==============
 */

struct class_Obj_struct;
typedef struct class_Obj_struct* class_Obj; 

typedef struct obj_Obj_struct {
  struct class_Obj_struct *clazz;
} * obj_Obj;

struct class_Obj_struct {
  void *super;
  /* Method table */
  obj_Obj (*constructor) ( void );
  obj_String (*STR) (obj_Obj);
  obj_Nothing (*PRINT) (obj_Obj);
  obj_Boolean (*EQUALS) (obj_Obj, obj_Obj);
}; 

extern struct class_Obj_struct the_class_Obj_struct;
extern class_Obj const the_class_Obj; /* Initialized in Builtins.c */

/* ================
 * String
 * Fields: 
 *    One hidden field, currently holding char*
 *    (change later to a rope for garbage collection)
 * Methods: 
 *    Those of Obj, plus ordering, concatenation,
 *    overloaded comparisons
 * ==================
 */

struct class_String_struct;
typedef struct class_String_struct* class_String;

typedef struct obj_String_struct {
  class_String clazz;
  char *text; 
} * obj_String;

struct class_String_struct {
  /* Method table: Inherited or overridden */
  class_Obj super;
  obj_String (*constructor) ( void );
  obj_String (*STR) (obj_String);
  obj_Nothing (*PRINT) (obj_String);
  obj_Boolean (*EQUALS) (obj_String, obj_Obj);
  /* Method table: Introduced in String */
  obj_Boolean (*LESSER) (obj_String, obj_String);
  obj_Boolean (*GREATER) (obj_String, obj_String);
  obj_Boolean (*ATLEAST) (obj_String, obj_String);
  obj_Boolean (*ATMOST) (obj_String, obj_String);
  obj_String (*PLUS) (obj_String, obj_String); 
};

extern class_String the_class_String;

/* this creates an obj_String from a char pointer */
extern obj_String str_literal(char *s);

/* ================
 * Boolean
 * Fields: 
 *    One hidden field, an int (0 for False, -1 for True)
 * Methods: 
 *    
 * =================
 */

struct class_Boolean_struct;
typedef struct class_Boolean_struct* class_Boolean; 

typedef struct obj_Boolean_struct {
  class_Boolean  clazz;
  int value; 
} * obj_Boolean;

struct class_Boolean_struct {
  class_Obj super;
  /* Method table: Inherited or overridden */
  obj_Boolean (*constructor) ( void );
  obj_String (*STR) (obj_Boolean);
  obj_Nothing (*PRINT) (obj_Obj);               /* Inherit */
  obj_Boolean (*EQUALS) (obj_Obj, obj_Obj); /* Inherit */ 
};

extern class_Boolean the_class_Boolean; 

/* There are only two instances of Boolean, 
 * lit_true and lit_false
 * (i.e., the values of the literals true and false)
 */
extern obj_Boolean lit_false;
extern obj_Boolean lit_true;

/* ==============
 * Nothing (really just a singleton Obj)
 * Fields: None
 * Methods: 
 *    Constructor  (called after allocation) 
 *    STRING 
 *    PRINT
 *    EQUALS
 * 
 * ==============
 */

/* Although there is little reason to call methods on the none
 * object, the only instance of Nothing, we could call PRINT on 
 * it or compare it to something.  It's STR method should return 
 * "<nothing>". 
 */ 
struct class_Nothing_struct {
  class_Obj super;
  /* Method table */
  obj_Nothing (*constructor) ( void );
  obj_String (*STR) (obj_Nothing);
  obj_Nothing (*PRINT) (obj_Obj);               /* Inherited */
  obj_Boolean (*EQUALS) (obj_Obj, obj_Obj); /* Inherited */
}; 

extern class_Nothing the_class_Nothing;

/* There is a single instance of Nothing, 
 * called nothing
 */
extern obj_Nothing none;

/* ================
 * Int
 * Fields: 
 *    One hidden field, an int
 * Methods: 
 *    STRING  (override)
 *    PRINT   (inherit)
 *    EQUALS  (override)
 *    and introducing
 *    LESSER
 *    PLUS    
 *    (add more later) 
 * =================
 */

struct class_Int_struct;
typedef struct class_Int_struct* class_Int; 

typedef struct obj_Int_struct {
  class_Int  clazz;
  int value; 
} * obj_Int;

struct class_Int_struct {
  class_Obj super;
  /* Method table: Inherited or overridden */
  obj_Int (*constructor) ( void );
  obj_String (*STR) (obj_Int);  /* Overridden */
  obj_Nothing (*PRINT) (obj_Obj);      /* Inherited */
  obj_Boolean (*EQUALS) (obj_Int, obj_Obj); /* Overridden */
  obj_Boolean (*LESSER) (obj_Int, obj_Int);   /* Introduced */
  obj_Boolean (*GREATER) (obj_Int, obj_Int);        /* Introduced */
  obj_Boolean (*ATLEAST) (obj_Int, obj_Int);        /* Introduced */
  obj_Boolean (*ATMOST) (obj_Int, obj_Int);        /* Introduced */
  obj_Int (*PLUS) (obj_Int, obj_Int);       /* Introduced */
  obj_Int (*MINUS) (obj_Int, obj_Int);        /* Introduced */
  obj_Int (*TIMES) (obj_Int, obj_Int);        /* Introduced */
  obj_Int (*DIVIDE) (obj_Int, obj_Int);        /* Introduced */
  obj_Int (*NEGATE) (obj_Int);        /* Introduced */
};

extern class_Int the_class_Int; 

/* Integer literals constructor, 
 * used by compiler and not otherwise available in 
 * Quack programs. 
 */
extern obj_Int int_literal(int n);


/* ===============================
 * Make all the methods we might 
 * inherit visible to user code 
 *================================
 */

/* ==== Obj ==== */
obj_Obj new_Obj();
obj_String Obj_method_STR(obj_Obj this);
obj_Nothing Obj_method_PRINT(obj_Obj this); 
obj_Boolean Obj_method_EQUALS(obj_Obj this, obj_Obj other);

/* ==== String ==== */
obj_String String_method_STR(obj_String this);
obj_String String_method_PRINT(obj_String this); 
obj_Boolean String_method_EQUALS(obj_String this, obj_Obj other);
obj_Boolean String_method_LESSER(obj_String this, obj_String other); 
obj_Boolean String_method_GREATER(obj_String this, obj_String other); 
obj_Boolean String_method_ATLEAST(obj_String this, obj_String other); 
obj_Boolean String_method_ATMOST(obj_String this, obj_String other); 
obj_String String_method_PLUS(obj_String this, obj_String other); 

/* ==== Boolean ==== */
obj_Boolean new_Boolean();
obj_String Boolean_method_STR(obj_Boolean this);

/* ==== Nothing ==== */
obj_Nothing new_Nothing();
obj_String Nothing_method_STR(obj_Nothing this);

/* ==== Int ==== */
obj_Int new_Int();
obj_String Int_method_STR(obj_Int this); 
obj_Boolean Int_method_EQUALS(obj_Int this, obj_Obj other);
obj_Boolean Int_method_LESSER(obj_Int this, obj_Int other);
obj_Boolean Int_method_GREATER(obj_Int this, obj_Int other);
obj_Boolean Int_method_ATLEAST(obj_Int this, obj_Int other);
obj_Boolean Int_method_ATMOST(obj_Int this, obj_Int other);
obj_Int Int_method_PLUS(obj_Int this, obj_Int other);
obj_Int Int_method_MINUS(obj_Int this, obj_Int other);
obj_Int Int_method_TIMES(obj_Int this, obj_Int other);
obj_Int Int_method_DIVIDE(obj_Int this, obj_Int other);
obj_Int Int_method_NEGATE(obj_Int this);

#endif
