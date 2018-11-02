#include "ASTNode.h"
#include <iterator>
#include <iostream>
#include <typeinfo>

void printVec(std::vector<AST::ASTNode *> vec) {
	for (std::vector<AST::ASTNode *>::const_iterator i = vec.begin(); i != vec.end(); ++i) {
    	std::cout << *i << ' ';
	}
}

void printMap(std::map<Type, std::vector<AST::ASTNode*> > myMap) {
	for(std::map<Type, std::vector<AST::ASTNode*> >::const_iterator it = myMap.begin(); it != myMap.end(); ++it) {
	    std::cout << it->first << std::endl;
	    //printVec(it->second);
	}
}

int main() {
	AST::ASTNode* root = new AST::ASTNode(CLASSES);
	root->name = "Root";
	std::cout << root->name << std::endl;
	AST::ASTNode* var = new AST::ASTNode(IDENT, "variable");
	std::cout << var->type << " : " << var->name << std::endl;
	root->insert(var);
	std::cout << root->children.at(IDENT) << std::endl;
	// std::cout << typeid(root->get(IDENT)).name() << std::endl;
	// std::cout << root->get(IDENT)->name << std::endl;
	// AST::ASTNode* getRoot = root->get(IDENT);
	// std::cout << getRoot->name << std::endl;

	//root->insert(new AST::ASTNode(IDENT, "variable2"));
	std::vector<AST::ASTNode *> vec = root->getSeq(IDENT);
	// std::cout << vec[0] << std::endl;
	// printVec(vec);
}