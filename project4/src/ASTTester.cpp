#include <iterator>
#include <iostream>
#include <typeinfo>
#include "ASTNode.h"

void printTest(std::vector<AST::Node *> vec) {

	std::vector<AST::Node *>::iterator it;

	for (it = vec.begin(); it != vec.end(); ++it) {
		printf("type : %s\n", TypeString[(*it)->type]);
		printf("name : %s\n", (*it)->name.c_str());
		printf("value : %d\n", (*it)->value);
		printf("\n");
	}
}

void printOrder(std::vector<Type> vec) {

	std::vector<Type>::iterator it;

	for (it = vec.begin(); it != vec.end(); ++it) {
		printf("type : %s\n", TypeString[*it]);
	}
}

int main() {
    AST::Node* root = new AST::Node(CLASSES);
	root->insert(new AST::Node(IDENT, "variable"));
	root->insert(new AST::Node(IDENT, "variable2"));
	root->insert(new AST::Node(INTCONST, 27));
	std::vector<AST::Node *> vec = root->getSeq(IDENT);
	std::vector<AST::Node *> vec2 = root->getSeq(INTCONST);

	AST::Node* nested = new AST::Node(BLOCK);
	nested->insert(new AST::Node(STRCONST, "nestedvariable"));
	root->insert(nested);

	AST::Node* getBlock = root->get(BLOCK);
	AST::Node* getBlockVar = getBlock->get(STRCONST);

	printTest(vec);
	printTest(vec2);

	AST::Node* got = root->get(INTCONST);
	std::cout << "INTCONST value from get is " << got->value << std::endl;

	std::cout << "STRCONST name from nested block STRCONST is " << getBlockVar->name << std::endl;

	std::cout << "\nOrder vector \n";
	printOrder(root->order);

	std::cout << isHeadType(PROGRAM) << std::endl;
	std::cout << isHeadType(OR) << std::endl;
	std::cout << "------~~~~JSON TIME~~~~------\n";

	AST::AST_print_context context;
	root->json(std::cout, context);
	std::cout << std::endl;
}