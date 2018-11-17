#include "typechecker.h"

bool Typechecker::isVarInit(Qmethod method, std::string ident) {
	return std::find(method.init.begin(), 
		method.init.end(), ident) != method.init.end();
}

Qmethod Typechecker::createQmethod(AST::Node *method) {
	Qmethod newMethod;
	newMethod.node = method;
	newMethod.name = method->get(IDENT, METHOD_NAME)->name;
	newMethod.type["return"] = method->get(IDENT, RETURN_TYPE)->name;

	AST::Node *formalsContainer = method->get(FORMAL_ARGS);
	if(!formalsContainer->order.empty()) { // empty formals check before iterating over it
		std::vector<AST::Node *> formals = formalsContainer->getAll(CLASS_ARG);
		for (AST::Node *arg : formals) {
			newMethod.init.push_back(arg->get(IDENT, VAR_IDENT)->name);
			newMethod.type[arg->get(IDENT, VAR_IDENT)->name] = arg->get(IDENT, TYPE_IDENT)->name;
			// std::cout << "	Formal: " << arg->get(IDENT, VAR_IDENT)->name
			// 		<< ", " << arg->get(IDENT, TYPE_IDENT)->name << std::endl;
		}
	}

	AST::Node *stmts = method->get(BLOCK, STATEMENTS); // from the newMethod, grab all statements
	if (!stmts->order.empty()) { // empty newMethod check before iterating over it
		for (Type t : stmts->order) { // for each statement type
			std::vector<AST::Node *> stmtvec = stmts->getAll(t); // get all statements of that type
			for (AST::Node *s : stmtvec) { 
				newMethod.stmts.push_back(s); // add each of these to the Qmethod in order
			}
		}
	}

	return newMethod;
}


Qclass Typechecker::createQclass(AST::Node *clazz) {
	Qclass newClass;
	newClass.node = clazz;
	newClass.name = clazz->get(IDENT, CLASS_NAME)->name;
	newClass.super = clazz->get(IDENT, SUPER_NAME)->name;
	
	AST::Node *astConstructor = clazz->get(METHOD, CONSTRUCTOR);
	Qmethod classConstructor = createQmethod(astConstructor);
	newClass.constructor = classConstructor;

	AST::Node *methodsContainer = clazz->get(METHODS);
	if(!methodsContainer->order.empty()) { // empty methods check before iterating over it
		std::vector<AST::Node *> methods = methodsContainer->getAll(METHOD);
		for (AST::Node *method : methods) {
			newClass.methods.push_back(createQmethod(method));
		}
	}

	return newClass;
}

// initializes our map classes and Qclass statements so that
// we can use them later for the actual type checking.
// all information is gathered from the AST passed in with root.
void Typechecker::initialize() { 
	bool haveClasses = !root->get(CLASSES)->order.empty();
	bool haveStatements = !root->get(BLOCK, STATEMENTS)->order.empty();
	// if we have an empty program, no type checking
	if ( !haveClasses && !haveStatements) { return; }

	if ( haveClasses ) { // only initialize our classes if we have some
		std::vector<AST::Node *> classes = root->get(CLASSES)->getAll(CLASS);
		for (AST::Node *n : classes) {
			Qclass clazz = createQclass(n);
			this->classes[clazz.name] = clazz;
			printQclass(clazz);
		}
	}
}

bool Typechecker::classHierarchyCheck() { return true; }

void Typechecker::printQclass(Qclass clazz) {
	std::cout << "******************| class " << clazz.name << " |******************" << std::endl;
	std::cout << "Super: " << clazz.super << std::endl << std::endl;
	printQmethod(clazz.constructor);
	for (Qmethod m : clazz.methods) {
		printQmethod(m);
	}
}

void Typechecker::printQmethod(Qmethod method) {
	std::cout << "	~~--~~| method " << method.name << " |~~--~~ " << std::endl;
	std::cout << "	Return type: " << method.type["return"] << std::endl;
	std::cout << "	Initialized vars: " << std::endl;
	for (std::string s : method.init) {
		std::cout << "		" << s << std::endl;
	}
	std::cout << "	Known types: " << std::endl;
	for (std::map<std::string, std::string>::iterator it = method.type.begin(); it != method.type.end(); ++it) {
	 	std::cout << "		" << it->first << ", " << it->second << std::endl;
	}
	std::cout << "	Stmts: " << std::endl;
	for (AST::Node *stmt : method.stmts) { // print each stmt type inside Qmethod's stmts vector
			std::cout << "		" << typeString(stmt->type) << std::endl;
	}
	std::cout << std::endl;
}