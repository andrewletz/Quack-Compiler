bool Typechecker::initCheckStmt(Qmethod &method, AST::Node *stmt, bool isConstructor) {
	std::cout << "--- new stmt ---" << std::endl;
	std::vector<AST::Node *> leaves;
	getLeaves(method, stmt, leaves, isConstructor);
	for (AST::Node* leaf : leaves) {
		if (leaf->nameinit) {
			std::cout << leaf->name << std::endl;
		} else if (leaf->valueinit) {
			std::cout << leaf->value << std::endl;
		}
	}
	// ----------------------------------------------------


	bool return_flag = true;
	Type nodeType = stmt->type;

	if (nodeType == ASSIGN) {

		// checking if we are assigning to an ident of form this.x
		AST::Node *left = stmt->get(DOT, L_EXPR);
		if (left != NULL) {
			AST::Node *load = left->get(LOAD);
			if (load != NULL) {
				if (load->get(IDENT)->name == "this") { 
					if (isConstructor) { // we have found a this.x = ... statement, push back to class's instancevars
						OUT << "defining this." << left->get(IDENT)->name << END;
				 		method.clazz->instanceVars.push_back(left->get(IDENT)->name);
					} else {
						RED << stageString(INITBEFOREUSE) << "attempt to assign instance variable \"" << left->get(IDENT)->name <<
							"\" in \"" << method.name << "\" in class \""
							<< method.clazz->name << "\"" << END;
						report::trackError(INITBEFOREUSE);
						return_flag = false;
					}
				}
			} 
		}


	}

	return return_flag;
}

bool Typechecker::initCheckQmethod(Qmethod &method, bool isConstructor) {
	bool ret_flag = true;
	if (method.stmts.empty()) return ret_flag;
	// if you encounter an assign of form this.x, add it to Qclass's instanceVars
	// then check if that instance var shares the same name as any given method in the same class
	// if so, report an error

	// check if l_expr is equal to a literal, if so report an error
	// -> Attempt to assign to a literal in method "__"
	for (AST::Node* stmt : method.stmts) {
		ret_flag = initCheckStmt(method, stmt, isConstructor);
		
	}
	
	return ret_flag;
}