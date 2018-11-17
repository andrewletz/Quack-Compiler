#include "typechecker.h"

// initializes our map classes and Qclass statements so that
// we can use them later for the actual type checking.
// all information is gathered from the AST passed in with root.
void Typechecker::initialize() { 
	std::vector<AST::Node *> classes = root->get(CLASSES)->getAll(CLASS);
	for (AST::Node* n : classes) {
		std::cout << n->get(IDENT, CLASS_NAME)->name << std::endl;
	}
}

bool Typechecker::classHierarchyCheck() { return true; }
