#include "typechecker.h"

// forward declare some helper methods so that we don't clutter the start of the file
bool isSubclassOrEqual(std::string class1, std::string class2);
bool doesClassExist(std::string classname);
bool isBuiltin(std::string classname);

Qmethod Typechecker::createQmethod(AST::Node *method, Qclass *containerClass, bool isConstructor) {
	Qmethod newMethod;
	newMethod.node = method;
	newMethod.clazz = containerClass;
	newMethod.name = method->get(IDENT, METHOD_NAME)->name;

	// check for methods sharing a name with their class
	if (!isConstructor) { // the name of a constructor is stored as the name of a class in the AST
		if (newMethod.name == containerClass->name) {
			RED << stageString(CLASSHIERARCHY) << "method \"" << newMethod.name << 
						"\" shares name with containing class!" << END;
	            	report::bail(CLASSHIERARCHY);
		}
	}

	// check for duplicate methods
	for (Qmethod otherMethod : containerClass->methods) {
		if (otherMethod.name == newMethod.name) {
			RED << stageString(CLASSHIERARCHY) << "method \"" << newMethod.name << 
						"\" in class \"" << containerClass->name << "\" is defined more than once!" << END;
	            	report::bail(CLASSHIERARCHY);
		}
	}

	newMethod.type["return"] = method->get(IDENT, RETURN_TYPE)->name;

	AST::Node *formalsContainer = method->get(FORMAL_ARGS);
	if(!formalsContainer->order.empty()) { // empty formals check before iterating over it
		std::vector<AST::Node *> formals = formalsContainer->getAll(CLASS_ARG);
		for (AST::Node *arg : formals) {
			newMethod.init.push_back(arg->get(IDENT, VAR_IDENT)->name);
			newMethod.type[arg->get(IDENT, VAR_IDENT)->name] = arg->get(IDENT, TYPE_IDENT)->name;
			// OUT << "	Formal: " << arg->get(IDENT, VAR_IDENT)->name
			// 		<< ", " << arg->get(IDENT, TYPE_IDENT)->name << END;
		}
	}

	AST::Node *stmts = method->get(BLOCK, STATEMENTS); // from the newMethod, grab all statements
	if (!stmts->order.empty()) { // empty newMethod check before iterating over it
		for (AST::Node * stmt : method->get(BLOCK, STATEMENTS)->rawChildren) {
			newMethod.stmts.push_back(stmt); // add each of these to the Qmethod in order
		}
	}

	return newMethod;
}


Qclass Typechecker::createQclass(AST::Node *clazz) {
	Qclass newClass;
	newClass.node = clazz;
	newClass.name = clazz->get(IDENT, CLASS_NAME)->name;
	newClass.super = clazz->get(IDENT, SUPER_NAME)->name;

	// make sure the class isn't extending any of the "final" Quack built-in classes
	if (newClass.super == "Nothing" || newClass.super == "String" ||
		newClass.super == "Boolean" || newClass.super == "Int" ) {
		RED << stageString(CLASSHIERARCHY) << "class \"" << newClass.name << 
					"\" extends built-in type \"" << newClass.super << "\"!" << END;
            	report::bail(CLASSHIERARCHY);
	}
	
	AST::Node *astConstructor = clazz->get(METHOD, CONSTRUCTOR);
	Qmethod classConstructor = createQmethod(astConstructor, &newClass, true);
	newClass.constructor = classConstructor;

	AST::Node *methodsContainer = clazz->get(METHODS);
	if(!methodsContainer->order.empty()) { // empty methods check before iterating over it
		std::vector<AST::Node *> methods = methodsContainer->getAll(METHOD);
		for (AST::Node *method : methods) {
			newClass.methods.push_back(createQmethod(method, &newClass, false));
		}
	}

	return newClass;
}

void Typechecker::initializeClasses(AST::Node *astRoot) {
	bool haveClasses = !astRoot->get(CLASSES)->order.empty();
	bool haveStatements = !astRoot->get(BLOCK, STATEMENTS)->order.empty();
	// if we have an empty program, no type checking
	if ( !haveClasses && !haveStatements) { return; }

	if ( haveClasses ) { // only initialize our classes if we have some
		std::vector<AST::Node *> classes = astRoot->get(CLASSES)->getAll(CLASS);
		for (AST::Node *n : classes) {
			Qclass clazz = createQclass(n);

			// check for class redeclaration here
			if (this->classes.find(clazz.name) != this->classes.end()) {
				RED << stageString(CLASSHIERARCHY) << "class \"" << clazz.name << 
					"\" has already been defined!" << END;
            	report::bail(CLASSHIERARCHY);
			}

			this->classes[clazz.name] = clazz;
			printQclass(clazz);
		}
	}
}

// initializes map "classes" and Qclass statements so that
// we can use them later for the actual type checking.
// all information is gathered from the AST passed in with root.
// stub classes are created with passed in stubs
void Typechecker::initialize() { 
	this->initializeClasses(this->root);
	this->initializeClasses(this->stubs);

	// Initialize the class hierarchy table
	std::string class_name;
	std::string super_name;
	std::map<std::string, std::vector<std::string>>::iterator it;

	// put Obj on the hierarchy first
	// std::vector<std::string> builtins;
	// builtins.push_back("String");
	// builtins.push_back("Int");
	// builtins.push_back("Boolean");
	// this->class_hierarchy.insert(std::pair<std::string, std::vector<std::string>>("Obj", builtins));

	for (auto clss : this->classes) {
		class_name = clss.second.name;
		super_name = clss.second.super;

		// we can easily check for no such super errors here, might aswell
		if (!doesClassExist(super_name)) {
			RED << stageString(CLASSHIERARCHY) << "class \"" << class_name << 
				"\" extends undeclared superclass \"" << super_name << "\"!" << END;
        	report::bail(CLASSHIERARCHY);
		}

		it = this->class_hierarchy.find(super_name);
		if (it == this->class_hierarchy.end()) {
			std::vector<std::string> subs;
			this->class_hierarchy.insert(std::pair<std::string, std::vector<std::string>>(super_name, subs));
		}
		this->class_hierarchy[super_name].push_back(class_name);
	}

}

bool Typechecker::classHierarchyCheck() {
	std::stack<std::string> class_stack; // stack for the classes we've come across
	std::vector<std::string> seen_classes; // vector for the classes we've seen to check for cycles
	std::string current_super; // the current class we're working with / checking against

	// we have to check every super class and its dependencies in its class hierarchy. if we simply
	// start at a static class like "Obj", then we won't be able to get to classes that *don't* inherit
	// from Obj, such as the example found in circular_dependency.qk
	for (auto qclss : this->class_hierarchy) {
		if (isBuiltin(qclss.first)) continue;

		class_stack.push(qclss.first);

		while (!class_stack.empty()) {
			current_super = class_stack.top();
			class_stack.pop();

			// check the super class we're working with to see if it's in the vector of seen super classes,
			// if it is then we have found a circular dependency, if not then just mark that we've seen it
			std::vector<std::string>::iterator findCycle = std::find(seen_classes.begin(), seen_classes.end(), current_super);
			if (findCycle != seen_classes.end()) {
				return false;
			}
			seen_classes.push_back(current_super);

			// push all the dependencies of this super onto the stack so we can continue down this
			// hierarchy
			for (auto spr : this->class_hierarchy[current_super]) {
				class_stack.push(spr);
			}
		}

		// clean up the stack & the seen classes, check the next hierarchy
		while (!class_stack.empty()) {
			class_stack.pop();
		}
		seen_classes.clear();
	}
	return true;
}

bool Typechecker::methodsCompatibleCheck() {
	for (auto qclss : this->classes) {
		for(Qmethod method : qclss.second.methods) {
			for (Qmethod parentMethod : this->classes[qclss.second.super].methods) {
				if (method.name == parentMethod.name) { 
					// we must be overriding this method if names match exactly
					// check that types properly line up for overriden method
					std::string returnTypeChild = method.type["return"];
					std::string returnTypeParent = parentMethod.type["return"];
					if (!doesClassExist(returnTypeChild)
							|| !this->isSubclassOrEqual(returnTypeChild, returnTypeParent)) {

						RED << stageString(TYPEINFERENCE) << "overriden method \"" << method.name <<
							"\" in subclass \"" << qclss.second.name << "\" has improper return type \""
							<< returnTypeChild << "\"!" << END;
				    	report::bail(TYPEINFERENCE);
					}

					
					// OUT << "c: " << returnTypeChild << " p: " << returnTypeParent << END;
					// OUT << method.name << " overrides " << parentMethod.name << END;
				}
			}
		}
		//printQclass(qclss.second);
	}
	return true;
}

bool Typechecker::initCheckQmethod(bool isConstructor) {
	// if you encounter an assign of form this.x, add it to Qclass's instanceVars
	// then check if that instance var shares the same name as any given method in the same class
	// if so, report an error

	// check if l_expr is equal to a literal, if so report an error
	// -> Attempt to assign to a literal in method "__"
}

bool Typechecker::initializeBeforeUseCheck() {
	return true;
}

bool Typechecker::typeInferenceCheck() {
	return true;
}

bool Typechecker::checkProgram() {
	// Type checking: phase one
	// - check for circular dependency
	// - check if class method definitions are compatible with parent's
	// (check if class has already been declared is done in initialize())
    // (check if class extends no such super is done in initialize())
    // (check if method shares same name as class is done in createQmethod())
    // (check for duplicate methods is done in createQmethod())

	bool classHierarchyValid = this->classHierarchyCheck();
    if (!classHierarchyValid) {
        report::error("class hierarchy check failed - circular dependency detected!", TYPECHECKER);
        report::bail(CLASSHIERARCHY);
    } else {
        report::gnote("class hierarchy check passed.", TYPECHECKER);
    }

    bool methodsCompatible = this->methodsCompatibleCheck();
    if (!methodsCompatible) {
        report::bail(CLASSHIERARCHY);
    } else {
        report::gnote("method compatibility check passed.", TYPECHECKER);
    }

    bool initBeforeUseCheckValid = this->initializeBeforeUseCheck();
    if (!initBeforeUseCheckValid) {
        report::error("initialization before use check failed: idk what to put here yet!", TYPECHECKER);
        report::bail(INITBEFOREUSE);
    } else {
        report::gnote("initialization before use check passed.", TYPECHECKER);
    }

    bool typeInferenceCheckValid = this->typeInferenceCheck();
    if (!typeInferenceCheckValid) {
        report::error("type inference check failed: idk what to put here yet!", TYPECHECKER);
        report::bail(TYPEINFERENCE);
    } else {
        report::gnote("type inference check passed.", TYPECHECKER);
    }

    // if we reach the end we know nothing has failed, return true to the driver
    return true;
}

bool Typechecker::isVarInit(Qmethod method, std::string ident) {
	return std::find(method.init.begin(), 
		method.init.end(), ident) != method.init.end();
}

std::string Typechecker::getSuperClass(std::string class1) {
	return classes[class1].super;
}

bool Typechecker::isBuiltin(std::string classname) {
	if (classname == "Obj" || classname == "String" || classname == "Nothing"
		|| classname == "Int" || classname == "Boolean") { 
		return true;
	} else {
		return false;
	} 
}

bool Typechecker::doesClassExist(std::string classname) {
	if (isBuiltin(classname)) { 
		return true;
	} else {
		return this->classes.find(classname) != this->classes.end();
	}
}

bool Typechecker::isSubclassOrEqual(std::string class1, std::string class2) {
	if (class1 == class2) {
		return true;
	}

	if (class1 == "Obj") {
		return false;
	}

	if (classes[class1].super == "Obj" && class2 == "Obj") {
		return true;
	}

	std::string tempSuper = class1;

	while (tempSuper != class2 || tempSuper != "Obj") {
		if (tempSuper == "Obj") {
			break;
		}
		if (tempSuper == class2) {
			return true;
		}
		for (auto clss : classes) {
			Qclass second = clss.second;
			if (second.name == tempSuper) {
				tempSuper = second.super;
			}
		}
	}
	return false;
}

std::string Typechecker::leastCommonAncestor(std::string class1, std::string class2) {
	if (class1 == class2) {
		return class1;
	}

	std::vector<std::string> class1Supers;

	if (class1 == "Obj") {
		class1Supers.push_back(class1);
	} else {
		class1Supers.push_back(class1);
		std::string tempSuper1 = class1;

		while (tempSuper1 != "Obj") {
			if (tempSuper1 == "Obj") {
				break;
			}
			class1Supers.push_back(classes[tempSuper1].super);
			tempSuper1 = classes[tempSuper1].super;
		}
		class1Supers.push_back("Obj");
	}

	std::vector<std::string> class2Supers;

	if (class2 == "Obj") {
		class2Supers.push_back(class2);
	} else {
		class2Supers.push_back(class2);
		std::string tempSuper2 = class2;

		while (tempSuper2 != "Obj") {
			if (tempSuper2 == "Obj") {
				break;
			}
			class2Supers.push_back(classes[tempSuper2].super);
			tempSuper2 = classes[tempSuper2].super;
		}
		class2Supers.push_back("Obj");
	}

	std::string retVal;

	for (auto super : class2Supers) {
		auto it = std::find(class1Supers.begin(), class1Supers.end(), super);
		if (it != class1Supers.end()) {
			retVal = *(it);
			break;
		} else {
			retVal = "Obj";
		}
	}

	return retVal;
}

void Typechecker::printQclass(Qclass clazz) {
	if (isBuiltin(clazz.name)) { 
		return;
	}
	OUT << "******************| class " << clazz.name << " |******************" << END;
	OUT << "Super: " << clazz.super << END << END;
	printQmethod(clazz.constructor);
	for (Qmethod m : clazz.methods) {
		printQmethod(m);
	}
}

void Typechecker::printQmethod(Qmethod method) {
	OUT << "	~~--~~| method " << method.name << " |~~--~~ " << END;
	OUT << "	Inside class: " << method.clazz->name << END;
	OUT << "	Return type: " << method.type["return"] << END;
	OUT << "	Initialized vars: " << END;
	for (std::string s : method.init) {
		OUT << "		" << s << END;
	}
	OUT << "	Known types: " << END;
	for (std::map<std::string, std::string>::iterator it = method.type.begin(); it != method.type.end(); ++it) {
	 	OUT << "		" << it->first << ", " << it->second << END;
	}
	OUT << "	Stmts: " << END;
	for (AST::Node *stmt : method.stmts) { // print each stmt type inside Qmethod's stmts vector
			OUT << "		" << typeString(stmt->type) << END;
	}
	OUT << END;
}