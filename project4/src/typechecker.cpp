#include "typechecker.h"

bool Typechecker::isVarInit(Qmethod method, std::string ident) {
	return std::find(method.init.begin(), 
		method.init.end(), ident) != method.init.end();
}

void Typechecker::printQmethod(Qmethod method) {
	std::cout << "	~~--~~| " << method.name << " |~~--~~ " << std::endl;
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
	std::cout << "	~~--~~--~~--~~--~~--~~ " << std::endl;
}

// initializes our map classes and Qclass statements so that
// we can use them later for the actual type checking.
// all information is gathered from the AST passed in with root.
void Typechecker::initialize() { 
	std::vector<AST::Node *> classes = root->get(CLASSES)->getAll(CLASS);
	for (AST::Node *n : classes) {
		Qclass newClass;
		newClass.node = n;
		newClass.name = n->get(IDENT, CLASS_NAME)->name;
		newClass.super = n->get(IDENT, SUPER_NAME)->name;
		std::cout << "Class name: " << newClass.name << std::endl;
		std::cout << "super: " << newClass.super << std::endl;

		Qmethod constructor;
		AST::Node *astconstructor = n->get(METHOD, CONSTRUCTOR);
		constructor.name = astconstructor->get(IDENT, METHOD_NAME)->name;
		constructor.type["return"] = astconstructor->get(IDENT, RETURN_TYPE)->name;

		AST::Node *cformalscontainer = astconstructor->get(FORMAL_ARGS);
		if(!cformalscontainer->order.empty()) { // empty formals check before iterating over it
			std::vector<AST::Node *> cformals = cformalscontainer->getAll(CLASS_ARG);
			for (AST::Node *carg : cformals) {
				constructor.init.push_back(carg->get(IDENT, VAR_IDENT)->name);
				constructor.type[carg->get(IDENT, VAR_IDENT)->name] = carg->get(IDENT, TYPE_IDENT)->name;
				// std::cout << "	Formal: " << carg->get(IDENT, VAR_IDENT)->name
				// 		<< ", " << carg->get(IDENT, TYPE_IDENT)->name << std::endl;
			}
		}

		AST::Node *cstmts = astconstructor->get(BLOCK, STATEMENTS); // from the constructor, grab all statements
		if (!cstmts->order.empty()) { // empty constructor check before iterating over it
			for (Type t : cstmts->order) { // for each statement type
				std::vector<AST::Node *> cstmtvec = cstmts->getAll(t); // get all statements of that type
				for (AST::Node *s : cstmtvec) { 
					constructor.stmts.push_back(s); // add each of these to the Qmethod in order
				}
			}
		}

		printQmethod(constructor);
		std::cout << std::endl;
	}
}

bool Typechecker::classHierarchyCheck() { return true; }
