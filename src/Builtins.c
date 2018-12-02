/* 
 * The built-in classes of Quack 
 * (incomplete implementation) 
 * 
 */
#include <stdio.h>   
#include <stdlib.h>  /* Malloc lives here; might replace with gc.h    */ 
#include <string.h>  /* For strcpy; might replace with cords.h from gc */ 

#include "Builtins.h"


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

const class_Obj the_class_Obj;

/* Constructor */
obj_Obj new_Obj(  ) {
  obj_Obj new_thing = (obj_Obj) malloc(sizeof(struct obj_Obj_struct));
  new_thing->clazz = the_class_Obj;
  return new_thing; 
}

/* Obj:STR */
obj_String Obj_method_STR(obj_Obj this) {
  long addr = (long) this;
  char *rep;
  asprintf(&rep, "<Object at %ld>", addr);
  obj_String str = str_literal(rep); 
  return str;
}

/* Obj:PRINT */
obj_Nothing Obj_method_PRINT(obj_Obj this) {
  obj_String str = this->clazz->STR(this);
  fprintf(stdout, "%s", str->text);
  return this;
}

/* Obj:EQUALS (Note we may want to replace this) */
obj_Boolean Obj_method_EQUALS(obj_Obj this, obj_Obj other) {
  if (this == other) {
    return lit_true;
  } else {
    return lit_false;
  }
}
  

/* The Obj Class (a singleton) */
struct  class_Obj_struct  the_class_Obj_struct = {
  new_Obj,
  Obj_method_STR, 
  Obj_method_PRINT, 
  Obj_method_EQUALS
};

const class_Obj the_class_Obj = &the_class_Obj_struct; 

 
/* ================
 * String
 * Fields: 
 *    One hidden field, currently holding char*
 *    (change later to a rope for garbage collection)
 * Methods: 
 *    Those of Obj, plus ordering, concatenation 
 *    (Incomplete for now.)
 * ==================
 */

/* Constructor */
obj_String new_String(  ) {
  obj_String new_thing = (obj_String) malloc(sizeof(struct obj_String_struct));
  new_thing->clazz = the_class_String;
  return new_thing; 
}

/* String:STRING */
obj_String String_method_STR(obj_String this) {
  return this;
}

/* String:PRINT */
obj_String String_method_PRINT(obj_String this) {
  fprintf(stdout, "%s", this->text);
  return this;
}
  
/* String:EQUALS (Note we may want to replace this */
obj_Boolean String_method_EQUALS(obj_String this, obj_Obj other) {
  obj_String other_str = (obj_String) other;
  /* But is it really? */
  if (other_str->clazz != the_class_String) {
    return lit_false;
  }
  if (strcmp(this->text,other_str->text) == 0) {
    return lit_true;
  } else {
    return lit_false;
  }
}

/* String:LESSER */
obj_Boolean String_method_LESSER(obj_String this, obj_String other) {
    if (strcmp(this->text, other->text) < 0) {
        return lit_true;
    } else {
        return lit_false;
    }
}

/* String:GREATER */
obj_Boolean String_method_GREATER(obj_String this, obj_String other) {
    if (strcmp(this->text, other->text) > 0) {
        return lit_true;
    } else {
        return lit_false;
    }
}

/* String:ATLEAST */
obj_Boolean String_method_ATLEAST(obj_String this, obj_String other) {
    if (strcmp(this->text, other->text) >= 0) {
        return lit_true;
    } else {
        return lit_false;
    }
}

/* String:ATMOST */
obj_Boolean String_method_ATMOST(obj_String this, obj_String other) {
    if (strcmp(this->text, other->text) <= 0) {
        return lit_true;
    } else {
        return lit_false;
    }
}

/* String:PLUS */
obj_String String_method_PLUS(obj_String this, obj_String other) {
  char *returnedStr = (char *) malloc(1 + strlen(this->text) + strlen(other->text));
  strcpy(returnedStr, this->text);
  strcat(returnedStr, other->text);
  
  obj_String returned = new_String();
  returned->text = returnedStr;
    
  return returned;
}

/* The String Class (a singleton) */
struct  class_String_struct  the_class_String_struct = {
  (class_Obj) &the_class_Obj_struct,
  new_String,
  String_method_STR, 
  Obj_method_PRINT, 
  String_method_EQUALS,
  String_method_LESSER,
  String_method_GREATER,
  String_method_ATLEAST,
  String_method_ATMOST,
  String_method_PLUS
};

class_String the_class_String = &the_class_String_struct; 

/* 
 * Internal use function for creating String objects
 * from char*.  Use this to create string literals. 
 */
obj_String str_literal(char *s) {
  char *rep;
  obj_String str = new_String(); 
  str->text = s;
  return str;
}

/* ================
 * Boolean
 * Fields: 
 *    One hidden field, an int (0 for False, -1 for True)
 * Methods: 
 *    Those of Obj 
 * =================
 */
/* Constructor */
obj_Boolean new_Boolean(  ) {
  return lit_false;
}

/* Boolean:STR */
obj_String Boolean_method_STR(obj_Boolean this) {
  if (this == lit_true) {
    return str_literal("true");
  } else if (this == lit_false) {
    return str_literal("false");
  } else {
    return str_literal("!!!BOGUS BOOLEAN");
  }
}

/* The Boolean Class (a singleton) */
struct  class_Boolean_struct  the_class_Boolean_struct = {
  (class_Obj) &the_class_Obj_struct,
  new_Boolean,
  Boolean_method_STR, 
  Obj_method_PRINT, 
  Obj_method_EQUALS
};

class_Boolean the_class_Boolean = &the_class_Boolean_struct; 
  
/* 
 * These are the only two objects of type Boolean that 
 * should ever exist. The constructor just picks one of 
 * them. 
 */ 
struct obj_Boolean_struct lit_false_struct =
  { &the_class_Boolean_struct, 0 };
obj_Boolean lit_false = &lit_false_struct;
struct obj_Boolean_struct lit_true_struct =
  { &the_class_Boolean_struct, 1 };
obj_Boolean lit_true = &lit_true_struct;

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
/*  Constructor */
obj_Nothing new_Nothing(  ) {
  return none; 
}

/* Nothing:STR */
obj_String Nothing_method_STR(obj_Nothing this) {
    return str_literal("<nothing>");
}

/* The Nothing Class (a singleton) */
struct  class_Nothing_struct  the_class_Nothing_struct = {
  (class_Obj) &the_class_Obj_struct,
  new_Nothing,
  Nothing_method_STR, 
  Obj_method_PRINT, 
  Obj_method_EQUALS
};

class_Nothing the_class_Nothing = &the_class_Nothing_struct; 
  
/* 
 * This is the only instance of class Nothing that 
 * should ever exist
 */ 
struct obj_Nothing_struct nothing_struct = { &the_class_Nothing_struct };
obj_Nothing none = &nothing_struct; 

/* ================
 * Int
 * Fields: 
 *    One hidden field, an int
 * Methods: 
 *    Those of Obj
 *    PLUS
 *    LESSER
 *    (add more later) 
 * =================
 */

/* Constructor */
obj_Int new_Int(  ) {
  obj_Int new_thing = (obj_Int) malloc(sizeof(struct obj_Int_struct));
  new_thing->clazz = the_class_Int;
  new_thing->value = 0;          
  return new_thing; 
}

/* Int:STR */
obj_String Int_method_STR(obj_Int this) {
  char *rep;
  asprintf(&rep, "%d", this->value);
  return str_literal(rep); 
}

/* Inherit Obj:PRINT */

/* Int:EQUALS */
obj_Boolean Int_method_EQUALS(obj_Int this, obj_Obj other) {
  obj_Int other_int = (obj_Int) other; 
  /* But is it? */
  if (other_int->clazz != this->clazz) {
    return lit_false;
  }
  if (this->value != other_int->value) {
    return lit_false;
  }
  return lit_true;
}

/* Int:LESSER */ 
obj_Boolean Int_method_LESSER(obj_Int this, obj_Int other) {
  if (this->value < other->value) {
    return lit_true;
  }
  return lit_false;
}

/* Int:GREATER */
obj_Boolean Int_method_GREATER(obj_Int this, obj_Int other) {
    if (this->value > other->value) {
        return lit_true;
    }
    return lit_false;
}

/* Int:ATLEAST */
obj_Boolean Int_method_ATLEAST(obj_Int this, obj_Int other) {
    if (this->value >= other->value) {
        return lit_true;
    }
    return lit_false;
}

/* Int:ATMOST */
obj_Boolean Int_method_ATMOST(obj_Int this, obj_Int other) {
     if (this->value <= other->value) {
        return lit_true;
    }
    return lit_false;
}

/* PLUS (new method) */
obj_Int Int_method_PLUS(obj_Int this, obj_Int other) {
  return int_literal(this->value + other->value);
}

/* Int:MINUS */
obj_Int Int_method_MINUS(obj_Int this, obj_Int other) {
    return int_literal(this->value - other->value);
}

/* Int:TIMES */
obj_Int Int_method_TIMES(obj_Int this, obj_Int other) {
    return int_literal(this->value * other->value);
}

/* Int:DIVIDE */
obj_Int Int_method_DIVIDE(obj_Int this, obj_Int other) {
    return int_literal(this->value / other->value);
}

/* Int:NEGATE */
obj_Int Int_method_NEGATE(obj_Int this) {
    return int_literal(-(this->value));
}

/* The Int Class (a singleton) */
struct  class_Int_struct  the_class_Int_struct = {
  (class_Obj) &the_class_Obj_struct,
  new_Int,
  Int_method_STR, 
  Obj_method_PRINT, 
  Int_method_EQUALS,
  Int_method_LESSER,
  Int_method_GREATER,
  Int_method_ATLEAST,
  Int_method_ATMOST,
  Int_method_PLUS,
  Int_method_MINUS,
  Int_method_TIMES,
  Int_method_DIVIDE,
  Int_method_NEGATE
};

class_Int the_class_Int = &the_class_Int_struct; 
  
/* Integer literals constructor, 
 * used by compiler and not otherwise available in 
 * Quack programs. 
 */
obj_Int int_literal(int n) {
  obj_Int boxed = new_Int();
  boxed->value = n;
  return boxed;
}
