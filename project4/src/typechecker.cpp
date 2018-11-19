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

	// Initialize the class hierarchy table
	std::string class_name;
	std::string super_name;
	std::map<std::string, std::vector<std::string>>::iterator it;

	for (auto clss : this->classes) {
		class_name = clss.second.name;
		super_name = clss.second.super;

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
	for (auto qclss : class_hierarchy) {
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
			for (auto spr : class_hierarchy[current_super]) {
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

bool Typechecker::initializeBeforeUseCheck() {
	return false;
}

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