#include "typechecker.h"

std::vector<std::string> intersection(std::vector<std::string> vec1, std::vector<std::string> vec2) {
    std::vector<std::string> ret_vec;

    std::sort(vec1.begin(), vec1.end());
    std::sort(vec2.begin(), vec2.end());

    std::set_intersection(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::inserter(ret_vec, ret_vec.begin()));

    return ret_vec;
}

// std::set_difference doesn't work properly... so I made my own simple version
std::vector<std::string> difference(std::vector<std::string> vec1, std::vector<std::string> vec2) {
	if (vec1.empty()) return vec2;
	if (vec2.empty()) return vec1;
    std::vector<std::string> ret_vec;

    for (std::string v1 : vec1) {
    	if ((std::find(vec2.begin(), vec2.end(), v1) == vec2.end())) ret_vec.push_back(v1);
    }

    for (std::string v2 : vec2) {
    	if ((std::find(vec1.begin(), vec1.end(), v2) == vec1.end())) ret_vec.push_back(v2);
    }

    return ret_vec;
}

Qmethod* Typechecker::createQmethod(AST::Node *method, Qclass *containerClass, bool isConstructor) {
	Qmethod *newMethod = new Qmethod();
	newMethod->node = method;
	newMethod->clazz = containerClass;
	newMethod->name = method->get(IDENT, METHOD_NAME)->name;

	// check for methods sharing a name with their class
	if (!isConstructor) { // the name of a constructor is stored as the name of a class in the AST
		if (newMethod->name == containerClass->name) {
			RED << stageString(CLASSHIERARCHY) << "method " << newMethod->name << 
						"() shares name with containing class" << END;
	            	report::bail(CLASSHIERARCHY);
		}
	}

	// check for duplicate methods
	for (Qmethod* otherMethod : containerClass->methods) {
		if (otherMethod->name == newMethod->name) {
			RED << stageString(CLASSHIERARCHY) << "method " << newMethod->name << 
						"() in class \"" << containerClass->name << "\" is defined more than once" << END;
	            	report::bail(CLASSHIERARCHY);
		}
	}

	newMethod->type["return"] = method->get(IDENT, RETURN_TYPE)->name;
	newMethod->argtype["return"] = method->get(IDENT, RETURN_TYPE)->name;

	AST::Node *formalsContainer = method->get(FORMAL_ARGS);
	if(!formalsContainer->order.empty()) { // empty formals check before iterating over it
		std::vector<AST::Node *> formals = formalsContainer->getAll(CLASS_ARG);
		for (AST::Node *arg : formals) {
			std::string arg_name = arg->get(IDENT, VAR_IDENT)->name;
			newMethod->init.push_back(arg_name);
			newMethod->args.push_back(arg_name);

			std::string arg_type = arg->get(IDENT, TYPE_IDENT)->name;
			newMethod->type[arg_name] = arg_type;
			newMethod->argtype[arg_name] = arg_type;
		}
	}

	AST::Node *stmts = method->get(BLOCK, STATEMENTS); // from the newMethod, grab all statements
	if (!stmts->order.empty()) { // empty newMethod check before iterating over it
		for (AST::Node * stmt : method->get(BLOCK, STATEMENTS)->rawChildren) {
			newMethod->stmts.push_back(stmt); // add each of these to the Qmethod in order
		}
	}

	return newMethod;
}


Qclass* Typechecker::createQclass(AST::Node *clazz) {
	Qclass *newClass = new Qclass();
	newClass->node = clazz;
	newClass->name = clazz->get(IDENT, CLASS_NAME)->name;
	newClass->super = clazz->get(IDENT, SUPER_NAME)->name;
	newClass->instanceVars.push_back("this");
	newClass->instanceVarType["this"] = newClass->name;

	// check to make sure the class isn't extending any of the "final" Quack built-in classes
	if (newClass->super == "Nothing" || newClass->super == "String" ||
		newClass->super == "Boolean" || newClass->super == "Int" ) {
		RED << stageString(CLASSHIERARCHY) << "class \"" << newClass->name << 
					"\" extends built-in type \"" << newClass->super << "\"" << END;
            	report::bail(CLASSHIERARCHY);
	}
	
	AST::Node *astConstructor = clazz->get(METHOD, CONSTRUCTOR);
	Qmethod *classConstructor = createQmethod(astConstructor, newClass, true);
	newClass->constructor = classConstructor;

	AST::Node *methodsContainer = clazz->get(METHODS);
	if(!methodsContainer->order.empty()) { // empty methods check before iterating over it
		std::vector<AST::Node *> methods = methodsContainer->getAll(METHOD);
		for (AST::Node *method : methods) {
			newClass->methods.push_back(createQmethod(method, newClass, false));
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
			Qclass *clazz = createQclass(n);

			// check for class redeclaration here
			if (this->classes.find(clazz->name) != this->classes.end()) {
				RED << stageString(CLASSHIERARCHY) << "class \"" << clazz->name << 
					"\" has already been defined" << END;
            	report::bail(CLASSHIERARCHY);
			}

			this->classes[clazz->name] = clazz;
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

	AST::Node *main_stmts = this->root->get(BLOCK, STATEMENTS);
	// build fake main if we have statements
	if (!main_stmts->order.empty()) {
		Qclass *mainWrapper = new Qclass();
		mainWrapper->node = main_stmts;
		mainWrapper->name = "$MAIN";
		mainWrapper->super = "$MAIN";

		Qmethod *mainConstructor = new Qmethod();
		mainConstructor->clazz = mainWrapper;
		mainConstructor->name = "$MAIN";
		for (AST::Node * stmt : main_stmts->rawChildren) {
			mainConstructor->stmts.push_back(stmt); // add each of these to the Qmethod in order
		}

		mainWrapper->constructor = mainConstructor;

		this->main = mainWrapper;
	} else {
		this->main = NULL;
	}

	// Initialize the class hierarchy table
	std::string class_name;
	std::string super_name;
	std::map<std::string, std::vector<std::string>>::iterator it;

	for (auto clss : this->classes) {
		class_name = clss.second->name;
		super_name = clss.second->super;

		// check for no such super errors here (not great style, but we're already iterating so might aswell)
		if (!doesClassExist(super_name)) {
			RED << stageString(CLASSHIERARCHY) << "class \"" << class_name << 
				"\" extends undeclared superclass \"" << super_name << "\"" << END;
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

bool Typechecker::methodSignaturesTypecheck() {
	bool return_flag = true;
	for (auto qclss : this->classes) {
		for(Qmethod *method : qclss.second->methods) {
			std::string returnType = method->type["return"];

			// check return type exists
			if (!doesClassExist(returnType)) {
				RED << stageString(TYPEINFERENCE) << "method " << method->name <<
					"() in \"" << qclss.second->name << "\" has non-existant return type \""
					<< returnType << "\"" << END;
				report::trackError(TYPEINFERENCE);
				return_flag = false;
			}

			// check argument types exist
			for (std::string var : method->args) {
				std::string varType = method->argtype[var];
				if (!doesClassExist(varType)) {
					RED << stageString(TYPEINFERENCE) << "overriden method " << method->name <<
						"() in \"" << qclss.second->name << "\" has argument \""
						<< var << "\" with non-existant type \"" << varType << "\"" << END;
					report::trackError(TYPEINFERENCE);
					return_flag = false;
					continue;
				}
			}
		}
	}
	return return_flag;
}

bool Typechecker::methodsCompatibleCheck() {
	bool return_flag = true;
	for (auto qclss : this->classes) {
		for(Qmethod *method : qclss.second->methods) {
			if (doesClassExist(method->name)) {
				RED << stageString(CLASSHIERARCHY) << "method " << method->name <<
					"() in \"" << qclss.second->name << "\" shares name with class" << END;
				report::trackError(CLASSHIERARCHY);
				return_flag = false;
			}
			for (Qmethod *parentMethod : this->classes[qclss.second->super]->methods) {
				if (method->name == parentMethod->name) { 
					// we must be overriding this method if names match exactly
					// check that types properly line up for overriden method

					// check return types between overriden and parent method
					std::string returnTypeChild = method->type["return"];
					std::string returnTypeParent = parentMethod->type["return"];

					if (!this->isSubclassOrEqual(returnTypeChild, returnTypeParent)) {
						RED << stageString(TYPEINFERENCE) << "overriden method " << method->name <<
							"() in \"" << qclss.second->name << "\" has return type \""
							<< returnTypeChild << "\" inconsistent with parent method" << END;
						report::trackError(TYPEINFERENCE);
						return_flag = false;
					}

					// check that we have matching types
					int numArgsParent = parentMethod->args.size();
					int numArgsChild = method->args.size();
					for (int i = 0; i < numArgsParent; i++) {
						std::string parentArg = parentMethod->args[i];
						std::string parentArgType = parentMethod->argtype[parentArg];
						if (i >= numArgsChild) {
							RED << stageString(CLASSHIERARCHY) << "overriden method " << method->name <<
								"() in \"" << qclss.second->name << "\" is missing argument \""
								<< parentArg << "\"" << END;
							report::trackError(CLASSHIERARCHY);
							return_flag = false;
						} else {
							std::string childArg = method->args[i];
							std::string childArgType = method->argtype[childArg];
							if (!isSubclassOrEqual(childArgType, parentArgType)) {
								RED << stageString(TYPEINFERENCE) << "overriden method " << method->name <<
									"() in \"" << qclss.second->name << "\" has argument \""
									<< childArg << "\" with incorrect type \"" << childArgType << "\"" 
									<< " (should be \"" << parentArgType << "\")" << END;
								report::trackError(TYPEINFERENCE);
								return_flag = false;
							}
						}
					}

					// check we don't have too many args in child method
					if (numArgsChild > numArgsParent) {
						int difference = numArgsChild - numArgsParent;
						for (int i = numArgsParent; i < numArgsChild; i++) {
							RED << stageString(CLASSHIERARCHY) << "overriden method " << method->name <<
								"() in \"" << qclss.second->name << "\" has argument \""
								<< method->args[i] << "\" not present in parent method" << END;
							report::trackError(CLASSHIERARCHY);
							return_flag = false;
						}
					}

				}
			}
		}
	}
	return return_flag;
}

bool Typechecker::initCheckStmt(Qmethod *method, AST::Node *stmt, 
							std::vector<std::string> &var_init,
                            std::vector<std::string> &field_init,
                            bool isConstructor, bool isMainStatements) {
	bool ret_flag = true;
	Type nodeType = stmt->type;

	if (nodeType == TYPECASE) {
		// check that the cond is init first (no type checking yet, it can be anything)
		AST::Node *var = stmt->get(IDENT);
		if (var != NULL) {
			if (!initCheckStmt(method, var, var_init, field_init, isConstructor, isMainStatements)) ret_flag = false;
		}

		AST::Node *type_alts_container = stmt->get(TYPE_ALTERNATIVES);
		std::vector<AST::Node *> type_alts = type_alts_container->getAll(TYPE_ALTERNATIVE);

		std::vector<std::string> var_init_copy1 = var_init;
		std::vector<std::string> field_init_copy1 = field_init;

		for (AST::Node *type_alt : type_alts) {
			AST::Node *ident = type_alt->getBySubtype(VAR_IDENT);
			// if the newly introduced variable is already in init, throw an error
			if (isVarInit(method, ident->name)) {
				RED << stageString(INITBEFOREUSE) << "typecase cannot re-introduce variable \"" 
					<< ident->name << "\" in method " << method->name << "() in class \""
					<< method->clazz->name << "\"" << END;
					report::trackError(INITBEFOREUSE);
				ret_flag = false;
			} else { // else iterate through all the statements
				var_init_copy1.push_back(ident->name); // we need to push the newly introduced var onto the temporary vector
				AST::Node *type_stmts = type_alt->get(BLOCK, STATEMENTS);
				for (AST::Node *type_stmt : type_stmts->rawChildren) {
					if (!initCheckStmt(method, type_stmt, var_init_copy1, field_init_copy1, isConstructor, isMainStatements)) ret_flag = false;
				}
			}
		}

		if (isConstructor) {
			std::vector<std::string> uninitializedFields;
			uninitializedFields = difference(field_init, field_init_copy1);
			for (std::string f : uninitializedFields) {
				RED << stageString(INITBEFOREUSE) << "instance variable \"" << f 
					<< "\" not initialized on all syntactic paths in \"" << method->clazz->name << "\"" << END;
					report::trackError(INITBEFOREUSE);
					ret_flag = false;
				return ret_flag;
			}
		}

		// update final return vectors
		var_init = intersection(var_init, var_init_copy1);
		field_init = intersection(field_init, field_init_copy1);
		return ret_flag;
	} 
	else if (nodeType == CALL) {
		AST::Node *actual_args_container = stmt->get(ACTUAL_ARGS);
		if (actual_args_container != NULL) {
			std::vector<AST::Node *> actual_args = actual_args_container->getAll(METHOD_ARG);
			if (!actual_args.empty()) {
				for (AST::Node *arg : actual_args) {
					AST::Node *real_arg = arg->getBySubtype(METHOD_ARG); // the actual arg node is inside of the "METHOD_ARG" node
					if (real_arg->type == LOAD) {
						if (real_arg->get(IDENT) != NULL) {
							if (real_arg->get(IDENT)->name == "this") {
								if (isConstructor) {
									RED << stageString(INITBEFOREUSE) << "attempt to use \"this\" before full initialization in constructor of \"" 
										<< method->clazz->name << "\"" << END;
										report::trackError(INITBEFOREUSE);
									ret_flag = false;
									return ret_flag;
								}
							}
						}
					}
				}
			}
		}
	}
	// control flow has the most complicated case, have to check all syntactic paths
	// init check while loop
	else if (nodeType == WHILE) {
		// check that the cond is init first (no type checking yet, it can be anything)
		AST::Node *cond = stmt->get(COND);
		if (cond != NULL) {
			if (!initCheckStmt(method, cond, var_init, field_init, isConstructor, isMainStatements)) ret_flag = false;
		}

		AST::Node *while_stmts = stmt->get(BLOCK, STATEMENTS);
		std::vector<std::string> var_init_copy1 = var_init;
		std::vector<std::string> field_init_copy1 = field_init;
		for (AST::Node *n : while_stmts->rawChildren) {
			if (!initCheckStmt(method, n, var_init_copy1, field_init_copy1, isConstructor, isMainStatements)) ret_flag = false;
		}

		if (isConstructor) {
			std::vector<std::string> uninitializedFields;
			uninitializedFields = difference(field_init, field_init_copy1);
			for (std::string f : uninitializedFields) {
				RED << stageString(INITBEFOREUSE) << "instance variable \"" << f 
					<< "\" not initialized on all syntactic paths in \"" << method->clazz->name << "\"" << END;
					report::trackError(INITBEFOREUSE);
					ret_flag = false;
				return ret_flag;
			}
		}

		// update final return vectors
		var_init = intersection(var_init, var_init_copy1);
		field_init = intersection(field_init, field_init_copy1);
		return ret_flag;
	}
	// init check if statements
	else if (nodeType == IF) {
		// check that the cond is init first (no type checking yet, it can be anything)
		AST::Node *cond = stmt->get(COND);
		if (cond != NULL) {
			if (!initCheckStmt(method, cond, var_init, field_init, isConstructor, isMainStatements)) ret_flag = false;
		}

		AST::Node *true_stmts = stmt->get(BLOCK, TRUE_STATEMENTS);
		std::vector<std::string> var_init_copy1 = var_init;
		std::vector<std::string> field_init_copy1 = field_init;
		for (AST::Node *n : true_stmts->rawChildren) {
			if (!initCheckStmt(method, n, var_init_copy1, field_init_copy1, isConstructor, isMainStatements)) ret_flag = false;
		}

		AST::Node *false_stmts = stmt->get(BLOCK, FALSE_STATEMENTS);
		std::vector<std::string> var_init_copy2 = var_init;
		std::vector<std::string> field_init_copy2 = field_init;
		for (AST::Node *n : false_stmts->rawChildren) {
			if (!initCheckStmt(method, n, var_init_copy2, field_init_copy2, isConstructor, isMainStatements)) ret_flag = false;
		}

		// if we're in the constructor, we need to make sure all fields are initialized on all paths
		// if we aren't in the constructor, we don't have to assign on all paths (but will still throw
		// an error later on if we attempt to assign to an unknown field)
		if (isConstructor) {
			std::vector<std::string> uninitializedFields;
			uninitializedFields = difference(field_init_copy1, field_init_copy2);
			for (std::string f : uninitializedFields) {
				RED << stageString(INITBEFOREUSE) << "instance variable \"" << f 
					<< "\" not initialized on all syntactic paths in \"" << method->clazz->name << "\"" << END;
					report::trackError(INITBEFOREUSE);
					ret_flag = false;
				return ret_flag;
			}
		}

		// update our final return vectors
		var_init = intersection(var_init_copy1, var_init_copy2);
		field_init = intersection(field_init_copy1, field_init_copy2);
		return ret_flag;
	}
	// matches all types of assign nodes ("something = something")
	else if (nodeType == ASSIGN) {
		AST::Node *r_expr = stmt->getBySubtype(R_EXPR);
		if (r_expr != NULL) {
			AST::Node *ident = r_expr->get(IDENT);
			if (ident != NULL) {
				if (ident->name == "this" && isConstructor) {
					RED << stageString(INITBEFOREUSE) << "attempt to pass or assign a variable to \"this\" in constructor of class "
					<< method->clazz->name << END;
					report::trackError(INITBEFOREUSE);
					ret_flag = false;
					return ret_flag;
				}
			}
		}
		
		// assign of form "this.x = ..."
		AST::Node *left = stmt->get(DOT, L_EXPR);
		if (left != NULL) {
			AST::Node *load = left->get(LOAD);
			if (load != NULL) {
				if (load->get(IDENT)->name == "this") { 
					if (isMainStatements) {
						RED << stageString(INITBEFOREUSE) << "reference to \"this\" found in program's main statements" << END;
						report::trackError(INITBEFOREUSE);
						ret_flag = false;
						return ret_flag;
					}
					std::string instanceVar = left->get(IDENT)->name;

					if (instanceVar == "this") {
						RED << stageString(INITBEFOREUSE) << "invalid assignment of form this.this found in \"" 
							<< method->name << "() in class \""
							<< method->clazz->name << "\"" << END;
						report::trackError(INITBEFOREUSE);
						ret_flag = false;
					} 

					if (isConstructor) { // we have found a this.x = ... statement, push back to class's instancevars
						if (doesClassExist(instanceVar)) {
							RED << stageString(INITBEFOREUSE) << "instance variable \""
								<< instanceVar << "\" in class \""
								<< method->clazz->name << "\" shares name with class \"" << instanceVar << "\"" << END;
							report::trackError(INITBEFOREUSE);
							ret_flag = false;
						}
						for (Qmethod *clssmethod : method->clazz->methods) {
							if (clssmethod->name == instanceVar) {
								RED << stageString(INITBEFOREUSE) << "instance variable \""
									<< instanceVar << "\" in class \""
									<< method->clazz->name << "\" shares name with method" << END;
								report::trackError(INITBEFOREUSE);
								ret_flag = false;
							}
						}

						// if it isn't in the vector already, push it in
						if (!isInstanceVar(method, instanceVar) && (std::find(field_init.begin(), field_init.end(), instanceVar) == field_init.end()) ) {
							field_init.push_back(instanceVar);
						}
				 		
					} else {
						if (!isInstanceVar(method, instanceVar) && (std::find(field_init.begin(), field_init.end(), instanceVar) == field_init.end()) ) {
							RED << stageString(INITBEFOREUSE) << "attempt to assign to non-existant instance variable \""
								<< instanceVar << "\" in \"" << method->name << "\" in class \""
								<< method->clazz->name << "\"" << END;
							report::trackError(INITBEFOREUSE);
							ret_flag = false;
						}
					}
				}
			} 
		}

		// assign of form "x = ..." and "x : Clss = ..."
		left = stmt->get(IDENT, LOC);
		if (left != NULL) {
			// check if we are assigning to an illegal value (boolean identifier)
			bool is_invalid = false;
			if (left->name == "false" || left->name == "true" || left->name == "this") {
				RED << stageString(TYPEINFERENCE) << "attempt to assign to invalid value \""
					<< left->name << "\" in \"" << method->name << "\" in class \""
					<< method->clazz->name << "\"" << END;
				report::trackError(TYPEINFERENCE);
				ret_flag = false;
				is_invalid = true;
			}

			if (!is_invalid) {
				if (!isVarInit(method, left->name) && (std::find(var_init.begin(), var_init.end(), left->name) == var_init.end()) ) {
					var_init.push_back(left->name);
				}
			}
		}
	}
	// stmt of form "x.x"
	else if (nodeType == DOT) {
		AST::Node *load = stmt->get(LOAD);
		if (load != NULL) {
			if (load->get(IDENT) != NULL) {
				// we have a "this.x" somewhere in a method, make appropriate checks
				if (load->get(IDENT)->name == "this") {
					if (isMainStatements) {
							RED << stageString(INITBEFOREUSE) << "reference to \"this\" found in program's main statements" << END;
							report::trackError(INITBEFOREUSE);
							ret_flag = false;
							return ret_flag;
					}

					std::string instanceVar = stmt->get(IDENT)->name;
					if (!isInstanceVar(method, instanceVar) && (std::find(field_init.begin(), field_init.end(), instanceVar) == field_init.end()) ) {
						RED << stageString(INITBEFOREUSE) << "uninitialized instance variable \"this."
							<< instanceVar << "\" used in method " << method->name << "() in class \""
							<< method->clazz->name << "\"" << END;
						report::trackError(INITBEFOREUSE);
						ret_flag = false;
					}
				}
			}
		} 
	}
	// either a variable of form "x" or a reference to "this"
	else if (nodeType == LOAD) {
		// if we find a "this" on its own
		if (stmt->get(IDENT) != NULL) { 
			if (stmt->get(IDENT)->name == "this") {
				if (isMainStatements) {
					RED << stageString(INITBEFOREUSE) << "reference to \"this\" found in program's main statements" << END;
					report::trackError(INITBEFOREUSE);
					ret_flag = false;
					return ret_flag;
				}
			} else {
				// if its a boolean, we dont need to check if its init
				std::string ident = stmt->get(IDENT)->name;
				if (ident != "false" && ident != "true" && ident != "none") {
					if (!isVarInit(method, ident) && (std::find(var_init.begin(), var_init.end(), ident) == var_init.end()) )  {
						RED << stageString(INITBEFOREUSE) << "uninitialized variable \""
							<< stmt->get(IDENT)->name << "\" used in " << method->name << "() in class \""
							<< method->clazz->name << "\"" << END;
						report::trackError(INITBEFOREUSE);
						ret_flag = false;
					}
				}
			}
		}
	}
	// leaf node, we can return now
	else if (nodeType == INTCONST || nodeType == STRCONST || nodeType == IDENT) {
		return ret_flag;
	}
	else if (nodeType == RETURN) {
		if (isConstructor) {
			RED << stageString(INITBEFOREUSE) << "explicit return found in constructor of class \""
				<< method->clazz->name << "\"" << END;
			report::trackError(INITBEFOREUSE);
			ret_flag = false;
		}
	}

	for (AST::Node *child : stmt->rawChildren) {
		if (!initCheckStmt(method, child, var_init, field_init, isConstructor, isMainStatements)) ret_flag = false;
	}

	return ret_flag;
}

bool Typechecker::initCheckQmethod(Qmethod *method, bool isConstructor, bool isMainStatements) {
	bool ret_flag = true;
	if (method->stmts.empty()) return ret_flag;

	// we pass this vector around so we know what we initialized after each statement
	std::vector<std::string> var_init;
	std::vector<std::string> field_init;

	for (AST::Node* stmt : method->stmts) {
		// do some preliminary checks before handing it off to the recursive method
		if (stmt->type == CONSTRUCTOR) {
			// if you found a constructor "Class(x, y, z);" on its own, that is an error (maybe should be a warning?)
			RED << stageString(INITBEFOREUSE) << "constructor not assigned to any variable or called in method \""
				<< method->name << "\" in class \""
				<< method->clazz->name << "\"" << END;
			report::trackError(INITBEFOREUSE);
			ret_flag = false;
			continue;
		} else if (stmt->type == LOAD) {
			// if you have a statement that is just "x;" you should throw an error if it isn't initialized
			AST::Node *ident = stmt->get(IDENT, LOC);
			if (ident != NULL) {
				if (!isVarInit(method, ident->name)) {
					RED << stageString(INITBEFOREUSE) << "attempt to load uninitialized variable \""
					<< ident->name << "\" in method \"" << method->name << "\" in class \""
					<< method->clazz->name << "\"" << END;
					report::trackError(INITBEFOREUSE);
					ret_flag = false;
					continue;
				}
			}
		}

		if(!initCheckStmt(method, stmt, var_init, field_init, isConstructor, isMainStatements)) ret_flag = false;

		// for each thing in passed around vector, add to init/instancevars, then clear the vectors/maps
		for (std::string var : var_init) {
			method->init.push_back(var);
			method->type[var] = "$UNKNOWN";
		}
		for (std::string field : field_init) {
			method->clazz->instanceVars.push_back(field);
			method->clazz->instanceVarType[field] = "$UNKNOWN";
		}
		var_init.clear();
		field_init.clear();
	}

	return ret_flag;
}

bool Typechecker::initializeBeforeUseCheck() {
	bool ret_flag = true;

	// init before use check constructor, then init check methods
	// we don't need to do all constructors first here, but we do in type checking
	for (auto clss : this->classes) {
		if (isBuiltin(clss.second->name)) { 
			continue;
		}
		if (!initCheckQmethod(clss.second->constructor, true, false)) ret_flag = false;
		for (Qmethod *m : clss.second->methods) {
			if(!initCheckQmethod(m, false, false)) ret_flag = false;
		}
	}

	// init check the main statements
	if (this->main != NULL) {
		if (!initCheckQmethod(this->main->constructor, false, true)) ret_flag = false;
	}

	return ret_flag;
}

// Kind of a weird method. Returns a string for cases where we need to infer deeper (like
// on the r_expr of an assignment), but doesn't return for cases where we don't. Basically,
// it's an "optional return". That's why the return value isn't used in typeInferQmethod.
// The result of the type inference is set through "ret_flag", this time through a passed reference
// rather than return value (so we can reserve the return value for the previously mentioned reason).
// When a type is inferred for a variable, it is set as a side effect rather than returned.
std::string Typechecker::typeInferStmt(Qmethod *method, AST::Node *stmt, bool &changed, bool &ret_flag) {
	Type nodeType = stmt->type;

	if (nodeType == TYPECASE) {
		return "";
	}
	else if (nodeType == RETURN) {
		AST::Node *r_expr = stmt->rawChildren[0];
		std::string curr_type = method->type["return"];
		if (stmt->skip) return curr_type;
		std::string new_type = typeInferStmt(method, r_expr, changed, ret_flag);

		// return type must be equal to or sub type of explicitly declared return type
		if (!isSubclassOrEqual(new_type, curr_type)) {
			RED << stageString(TYPEINFERENCE) << "return type in method " << method->name << "() in class \"" 
				<< method->clazz->name << "\"" << " is \"" << new_type << "\" (must be subclass or equal to \"" << curr_type << "\")"
				<< END;
			report::trackError(TYPEINFERENCE);
			ret_flag = false;
			stmt->skip = true;
			return curr_type;
		}

		if (new_type != curr_type) {
			method->type["return"] = leastCommonAncestor(curr_type, new_type);
		}
	}
	else if (nodeType == IF) {
		AST::Node *cond = stmt->get(COND)->rawChildren[0];
		if (cond != NULL) {
			std::string condType = typeInferStmt(method, cond, changed, ret_flag);
			if (condType != "Boolean") {
				if (!cond->skip) {
					RED << stageString(TYPEINFERENCE) << "if statement condition not of type boolean in \"" 
						<< method->name << "() in \"" << method->clazz->name << "\"" << END;
					report::trackError(TYPEINFERENCE);
					ret_flag = false;
					cond->skip = true;
				}
			}
		}

		AST::Node *true_stmts = stmt->get(BLOCK, TRUE_STATEMENTS);
		for (AST::Node *true_stmt : true_stmts->rawChildren) {
			typeInferStmt(method, true_stmt, changed, ret_flag);
		}

		AST::Node *false_stmts = stmt->get(BLOCK, FALSE_STATEMENTS);
		for (AST::Node *false_stmt : false_stmts->rawChildren) {
			typeInferStmt(method, false_stmt, changed, ret_flag);
		}
	}
	else if (nodeType == WHILE) {
		AST::Node *cond = stmt->get(COND)->rawChildren[0];
		if (cond != NULL) {
			std::string condType = typeInferStmt(method, cond, changed, ret_flag);
			if (condType != "Boolean") {
				if (!cond->skip) {
					RED << stageString(TYPEINFERENCE) << "while statement condition not of type boolean in \"" 
						<< method->name << "() in \"" << method->clazz->name << "\"" << END;
					report::trackError(TYPEINFERENCE);
					ret_flag = false;
					cond->skip = true;
				}
			}
		}

		AST::Node *while_stmts = stmt->get(BLOCK, STATEMENTS);
		for (AST::Node *while_stmt : while_stmts->rawChildren) {
			typeInferStmt(method, while_stmt, changed, ret_flag);
		}
	}
	else if (nodeType == CALL) { // a call always has 3 children
		// if (stmt->skip) return lhsType; // we dont want to error check again
		AST::Node *lhs = stmt->rawChildren[0]; // left hand side can be any type of node
		std::string lhsType = typeInferStmt(method, lhs, changed, ret_flag);
		std::string methodName = stmt->rawChildren[1]->name; // center node is always the ident corresponding to method name

		Qclass *qclass;
		Qclass *qclass_temp;
		Qmethod *calledMethod;
		if (doesClassExist(lhsType)) {
			if (methodName == "NEGATE") {
				if (lhsType != "Int") {
					RED << stageString(TYPEINFERENCE) << "invalid use of \"negate\" operator on non-integer object \""
						<< lhsType << "\" in method \"" << method->name << "\" in class \""
						<< method->clazz->name << "\"" << END;
					report::trackError(TYPEINFERENCE);
					ret_flag = false;
					stmt->skip = true;
					return lhsType;
				} else {
					return "Int";
				}
			}
			if (methodName == "NOT") {
				if (lhsType != "Boolean") {
					RED << stageString(TYPEINFERENCE) << "invalid use of \"not\" operator on non-boolean object \""
						<< lhsType << "\" in method \"" << method->name << "\" in class \""
						<< method->clazz->name << "\"" << END;
					report::trackError(TYPEINFERENCE);
					ret_flag = false;
					stmt->skip = true;
					return lhsType;
				} else {
					return "Boolean";
				}
			}
			if (methodName == "AND" || methodName == "OR") {
				if (lhsType != "Boolean") {
					RED << stageString(TYPEINFERENCE) << "invalid use of \"" << methodName << "\" operator on non-boolean object \""
						<< lhsType << "\" in method \"" << method->name << "\" in class \""
						<< method->clazz->name << "\"" << END;
					report::trackError(TYPEINFERENCE);
					ret_flag = false;
					stmt->skip = true;
					return lhsType;
				} else {
					AST::Node *actual_args_container = stmt->get(ACTUAL_ARGS);
					if (actual_args_container != NULL) {
						std::vector<AST::Node *> actual_args = actual_args_container->getAll(METHOD_ARG);
						if (actual_args.size() == 1) {
							AST::Node *real_arg = actual_args.front()->getBySubtype(METHOD_ARG);
							std::string argType = typeInferStmt(method, real_arg, changed, ret_flag);
							if (argType != "Boolean") {
								RED << stageString(TYPEINFERENCE) << "invalid use of \"" << methodName << "\" operator on non-boolean object \""
									<< argType << "\" in method \"" << method->name << "\" in class \""
									<< method->clazz->name << "\"" << END;
								report::trackError(TYPEINFERENCE);
								ret_flag = false;
								stmt->skip = true;
								return lhsType;
							} else {
								return "Boolean";
							}
						} else {
							RED << stageString(TYPEINFERENCE) << "too many operands given to \"" << methodName << "\" operator \""
								<< lhsType << "\" in method \"" << method->name << "\" in class \""
								<< method->clazz->name << "\" " << END;
							report::trackError(TYPEINFERENCE);
							ret_flag = false;
							stmt->skip = true;
							return lhsType;
						}
					}
				}
			}


			bool foundMethod = false;
			qclass = this->classes[lhsType];
			qclass_temp = qclass;
			find: // keep jumping to this label if we haven't seen the method yet
				//OUT << "current class we're iterating over is " << qclass_temp->name << END;
				for (Qmethod *m : qclass_temp->methods) {
					//OUT << "	mname is " << m->name << END;
					if (m->name == methodName) {
						foundMethod = true;
						calledMethod = m;
						break;
					}
				}

			if (!foundMethod) { // if we haven't seen the method yet, iterate through the supers
				if (qclass_temp->name != "Obj") { // we're at the top of the tree, stop looking
					qclass_temp = this->classes[qclass_temp->super];
					goto find;
				}
			}

			//OUT << END;

			if (!foundMethod) {
				RED << stageString(TYPEINFERENCE) << "method " << methodName << "() not found in class \""
					<< lhsType << "\" in method \"" << method->name << "\" in class \""
					<< method->clazz->name << "\"" << END;
				report::trackError(TYPEINFERENCE);
				ret_flag = false;
				stmt->skip = true;
				return lhsType;
			}

		} else {
			RED << stageString(TYPEINFERENCE) << "invalid method " << methodName <<  "() called on unknown class \""
				<< lhsType << "\" used in method \"" << method->name << "\" in class \""
				<< method->clazz->name << "\"" << END;
			report::trackError(TYPEINFERENCE);
			ret_flag = false;
			stmt->skip = true;
			return lhsType;
		}

		std::string returnType = calledMethod->type["return"];
		// if we're here, we know the method exists. now we compare args
		int numArgsMethod = calledMethod->args.size();
		// make sure all our args line up
		AST::Node *actual_args_container = stmt->get(ACTUAL_ARGS);
		if (actual_args_container != NULL) {
			std::vector<AST::Node *> actual_args = actual_args_container->getAll(METHOD_ARG);
			if (!actual_args.empty()) {
				std::vector<std::string> argTypes;
				for (AST::Node *arg : actual_args) {
					AST::Node *real_arg = arg->getBySubtype(METHOD_ARG); // the actual arg node is inside of the "METHOD_ARG" node
					std::string argType = typeInferStmt(method, real_arg, changed, ret_flag);
					argTypes.push_back(argType);
				}
				
				int numArgsCallToMethod = argTypes.size();
				for (int i = 0; i < numArgsMethod; i++) {
					std::string methodArg = calledMethod->args[i]; // get arg name from class we are constructing
					std::string methodArgType = calledMethod->argtype[methodArg]; // get the type of that arg
					if (i >= numArgsCallToMethod) { // if we are missing args in our call to the constructor
						RED << stageString(TYPEINFERENCE) << "call to method "
							<< methodName << "() of class \"" << lhsType
							<< "\" in method " << method->name <<
							"() in \"" << method->clazz->name << "\" is missing argument \""
							<< methodArg << "\"" << END;
						report::trackError(TYPEINFERENCE);
						ret_flag = false;
						stmt->skip = true;
					} else {
						std::string callToMethodType = argTypes[i]; // type of arg i in our call to the constructor
						if (!isSubclassOrEqual(callToMethodType, methodArgType)) { // type check that arg
							RED << stageString(TYPEINFERENCE) << "argument " << i+1 << " of call to method "
								<< methodName << "() of class \"" << lhsType << "\" in method " << method->name << "() is of incorrect type \"" 
								<< callToMethodType << "\"" << " (must be subclass or equal to \"" << methodArgType << "\")"
								<< END;
							report::trackError(TYPEINFERENCE);
							ret_flag = false;
							stmt->skip = true;
						}
					}
				}

				// check we don't have too many args in our call to the method
				if (numArgsCallToMethod > numArgsMethod) {
					int difference = numArgsCallToMethod - numArgsMethod;
					for (int i = numArgsMethod; i < numArgsCallToMethod; i++) {
						RED << stageString(TYPEINFERENCE) << "invalid argument " << i+1 << " in call to method " 
							<< methodName << "() of class \"" << lhsType << "\" in method " << method->name <<
							"() in class \"" << method->clazz->name << "\" (too many args)" << END; 
						report::trackError(TYPEINFERENCE);
						ret_flag = false;
						stmt->skip = true;
					}
				}

			} else { // we have a method call with 0 args
				if (numArgsMethod == 0) { // if the class also has 0 args, great, just return that class name
					return returnType; 
				} else { // if it does have args however, print all the ones we're missing in our method call
					for (int i = 0; i < numArgsMethod; i++) {
						std::string methodArg = calledMethod->args[i]; // get arg name from method we are calling
						std::string methodArgType = calledMethod->argtype[methodArg]; // get the type of that arg
						RED << stageString(TYPEINFERENCE) << "call to method " << methodName
							<< "() of class \"" << lhsType << "\" in method " << method->name <<
							"() in \"" << method->clazz->name << "\" is missing argument \""
							<< methodArg << "\"" << END;
						report::trackError(TYPEINFERENCE);
						ret_flag = false;
						stmt->skip = true;
					}
				}
			}
		}
		return returnType;
		
	}
	else if (nodeType == CONSTRUCTOR) {
		AST::Node *class_name_node = stmt->get(IDENT);
		if (class_name_node != NULL) {
			std::string class_name = class_name_node->name;
			if (doesClassExist(class_name)) {
				if (stmt->skip) return class_name; // we dont want to error check again
				Qclass *qclass = this->classes[class_name];

				Qmethod *constructorMethod = qclass->constructor;
				int numArgsConstructor = constructorMethod->args.size();
				// make sure all our args line up
				AST::Node *actual_args_container = stmt->get(ACTUAL_ARGS);
				if (actual_args_container != NULL) {
					std::vector<AST::Node *> actual_args = actual_args_container->getAll(METHOD_ARG);
					if (!actual_args.empty()) {
						std::vector<std::string> argTypes;
						for (AST::Node *arg : actual_args) {
							AST::Node *subLexpr = arg->getBySubtype(METHOD_ARG);
							std::string argType = typeInferStmt(method, subLexpr, changed, ret_flag);
							argTypes.push_back(argType);
						}
						
						int numArgsCallToConstructor = argTypes.size();
						for (int i = 0; i < numArgsConstructor; i++) {
							std::string constructorArg = constructorMethod->args[i]; // get arg name from class we are constructing
							std::string constructorArgType = constructorMethod->argtype[constructorArg]; // get the type of that arg
							if (i >= numArgsCallToConstructor) { // if we are missing args in our call to the constructor
								RED << stageString(TYPEINFERENCE) << "call to constructor of class \"" << class_name
									<< "\" in method " << method->name <<
									"() in \"" << method->clazz->name << "\" is missing argument \""
									<< constructorArg << "\"" << END;
								report::trackError(TYPEINFERENCE);
								ret_flag = false;
								stmt->skip = true;
							} else {
								std::string callToConstructorType = argTypes[i]; // type of arg i in our call to the constructor
								if (!isSubclassOrEqual(callToConstructorType, constructorArgType)) { // type check that arg
									RED << stageString(TYPEINFERENCE) << "argument " << i+1 << " of call to constructor "
										<< class_name << "() in method " << method->name << "() is of incorrect type \"" 
										<< callToConstructorType << "\"" << " (must be subclass or equal to \"" << constructorArgType << "\")"
										<< END;
									report::trackError(TYPEINFERENCE);
									ret_flag = false;
									stmt->skip = true;
								}
							}
						}

						// check we don't have too many args in our call to the constructor
						if (numArgsCallToConstructor > numArgsConstructor) {
							int difference = numArgsCallToConstructor - numArgsConstructor;
							for (int i = numArgsConstructor; i < numArgsCallToConstructor; i++) {
								RED << stageString(TYPEINFERENCE) << "invalid argument " << i+1 << " in call to constructor "
									<< class_name << "() in method " << method->name << "() (too many args)" << END; 
								report::trackError(TYPEINFERENCE);
								ret_flag = false;
								stmt->skip = true;
							}
						}

					} else { // we have a constructor call with 0 args
						if (numArgsConstructor == 0) { // if the class also has 0 args, great, just return that class name
							return class_name; 
						} else { // if it does have args however, print all the ones we're missing in our constructor call
							for (int i = 0; i < numArgsConstructor; i++) {
								std::string constructorArg = constructorMethod->args[i]; // get arg name from class we are constructing
								std::string constructorArgType = constructorMethod->argtype[constructorArg]; // get the type of that arg
								RED << stageString(TYPEINFERENCE) << "call to constructor of class \"" << class_name
									<< "\" in method " << method->name <<
									"() in \"" << method->clazz->name << "\" is missing argument \""
									<< constructorArg << "\"" << END;
								report::trackError(TYPEINFERENCE);
								ret_flag = false;
								stmt->skip = true;
							}
						}
					}
				}
				return class_name;
			} else {
				if (!stmt->skip) {
					RED << stageString(TYPEINFERENCE) << "invalid constructor for unknown class \""
						<< class_name << "\" used in method \"" << method->name << "\" in class \""
						<< method->clazz->name << "\"" << END;
					report::trackError(TYPEINFERENCE);
					ret_flag = false;
					stmt->skip = true;
				}
			}
		}

	}
	else if (nodeType == ASSIGN) {
		AST::Node *r_expr = stmt->getBySubtype(R_EXPR);

		// assign of form "this.x = ..."
		AST::Node *left = stmt->get(DOT, L_EXPR);
		if (left != NULL) {
			AST::Node *load = left->get(LOAD);
			if (load != NULL) {
				if (load->get(IDENT)->name == "this") { // we have found a this.x = ... statement
					std::string instanceVar = left->get(IDENT)->name;
					if (stmt->skip) return method->clazz->instanceVarType[instanceVar];

					// if it has an explicit type
					AST::Node *explicit_type = stmt->get(IDENT, TYPE_IDENT);
					if (explicit_type != NULL) {
						if (!doesClassExist(explicit_type->name)) {
							RED << stageString(TYPEINFERENCE) << "attempt to assign explicit type with unknown class \""  
								<< explicit_type->name << "\" in method " << method->name << "() in class \"" 
								<< method->clazz->name << "\"" << END;
							report::trackError(TYPEINFERENCE);
							ret_flag = false;
							stmt->skip = true;
							return method->clazz->instanceVarType[instanceVar];
						}
						if (isSubclassOrEqual(method->clazz->instanceVarType[instanceVar], explicit_type->name)) {
							method->clazz->instanceVarType[instanceVar] = explicit_type->name;
						} else {
							RED << stageString(TYPEINFERENCE) << "instance variable \""
								<< instanceVar << "\" in class \""
								<< method->clazz->name << "\" is assigned invalid explicit type \"" << explicit_type->name << "\"" << END;
							report::trackError(TYPEINFERENCE);
							ret_flag = false;
							stmt->skip = true;
							return method->clazz->instanceVarType[instanceVar];
						}
						if (!isInstanceVarExplicit(method, instanceVar)) {
							method->clazz->explicitFields.push_back(instanceVar);
						}
					}

					//OUT << "Inferring type for: " << instanceVar << END;
					std::string curr_type = method->clazz->instanceVarType[instanceVar];
					//OUT << "	curr_type: " << curr_type << END;
					std::string assigned_type = typeInferStmt(method, r_expr, changed, ret_flag);
					//OUT << "	assigned_type: " << assigned_type << END << END;
					if (isInstanceVarExplicit(method, instanceVar)) {
						if (!isSubclassOrEqual(curr_type, assigned_type)) {
							RED << stageString(TYPEINFERENCE) << "instance variable \""
								<< instanceVar << "\" in class \""
								<< method->clazz->name << "\" is assigned invalid type \"" << assigned_type << "\"" << END;
							report::trackError(TYPEINFERENCE);
							ret_flag = false;
							stmt->skip = true;
							return curr_type;
						}
					}
					//OUT << "	assigned_type: " << assigned_type<< END;
					std::string new_type = leastCommonAncestor(curr_type, assigned_type);
					//OUT << "	new_type: " << new_type<< END;
					if (new_type != curr_type) {
						method->clazz->instanceVarType[instanceVar] = new_type;
						changed = true;
					}
					return new_type;
				}
			} else {
				// this would be a good warning spot, if the lhs of an assign is a dot and ISNT a this.x or x.y
			}
		}

		// assign of form "x = ..." and "x : Clss = ..."
		left = stmt->get(IDENT, LOC);
		if (left != NULL) {
			if (stmt->skip) return method->type[left->name];
			// if it has an explicit type
			AST::Node *explicit_type = stmt->get(IDENT, TYPE_IDENT);
			if (explicit_type != NULL) {
				if (!doesClassExist(explicit_type->name)) {
					RED << stageString(TYPEINFERENCE) << "attempt to assign explicit type with unknown class \""  
						<< explicit_type->name << "\" in method " << method->name << "() in class \"" 
						<< method->clazz->name << "\"" << END;
					report::trackError(TYPEINFERENCE);
					ret_flag = false;
					stmt->skip = true;
					return method->type[left->name];
				}
				// OUT << "Var Name: " << left->name << " Curr Type: " << method->type[left->name] << " Explicit Type: " << explicit_type->name << END;
				if (isSubclassOrEqual(method->type[left->name], explicit_type->name)) {
					method->type[left->name] = explicit_type->name;
				} else {
					RED << stageString(TYPEINFERENCE) << "variable \""
						<< left->name << "\" in " << method->name <<  "() in class \""
						<< method->clazz->name << "\" is assigned invalid explicit type \"" << explicit_type->name << "\"" << END;
					report::trackError(TYPEINFERENCE);
					ret_flag = false;
					stmt->skip = true;
					return method->type[left->name];
				}
				if (!isVarExplicit(method, left->name)) {
					method->explicitVars.push_back(left->name);
				}
			}

			// infer type if we don't have an explicit one
			std::string curr_type = method->type[left->name];
			//OUT << "curr_type: " << curr_type << END;
			std::string assigned_type = typeInferStmt(method, r_expr, changed, ret_flag);
			//OUT << "assigned_type: " << assigned_type << END;
			if (isVarExplicit(method, left->name)) {
				if (!isSubclassOrEqual(assigned_type, curr_type)) {
					RED << stageString(TYPEINFERENCE) << "variable \""
						<< left->name << "\" in " << method->name <<  "() in class \""
						<< method->clazz->name << "\" is assigned invalid type \"" << assigned_type << "\"" << END;
					report::trackError(TYPEINFERENCE);
					ret_flag = false;
					stmt->skip = true;
					return curr_type;
				}
			}

			std::string new_type = leastCommonAncestor(curr_type, assigned_type);
			//OUT << "new_type: " << new_type << END;
			if (new_type != curr_type) {
				method->type[left->name] = new_type;
				changed = true;
			}
			return new_type;
		}
	} else if (nodeType == DOT) {
		AST::Node *lhs = stmt->rawChildren[0];
		std::string lhsType = typeInferStmt(method, lhs, changed, ret_flag);
		if (!doesClassExist(lhsType)) return lhsType; // we should stop on the first invalid DOT in a series of DOTs

		Qclass *lhsClass = this->classes[lhsType];
		//OUT << "lhs node is " << typeString(lhs->type) << ", type is " << lhsType <<  END;

		AST::Node *rhs = stmt->rawChildren[1];
		if (rhs->type == IDENT) { // should always be an ident... but we can check anyways
			std::string rhsName = rhs->name; 
			//OUT << rhsName << END;
			if (!isInstanceVar(lhsClass->constructor, rhsName)) {
				RED << stageString(TYPEINFERENCE) << "attempt to access unknown instance variable \"" 
					<< rhsName << "\" from class type \"" << lhsType << "\" in " << method->name <<  "() in class \""
					<< method->clazz->name << "\"" << END;
					report::trackError(TYPEINFERENCE);
				ret_flag = false;
			} else {
				//OUT << "returning type " << lhsClass->instanceVarType[rhsName] << " from DOT" << END << END;
				return lhsClass->instanceVarType[rhsName]; // get the type of that instance var from the other class
			}
		}

	} else if (nodeType == LOAD) {
		if (stmt->get(IDENT) != NULL) { 
			std::string ident = stmt->get(IDENT)->name;
			if (ident == "this") {
				return method->clazz->name;
			} else if (ident == "true" || ident == "false") { 
				//OUT << "Returning Boolean..." << END;
				return "Boolean";
			} else if (ident == "none") {
				return "Nothing";
			} else {
				return method->type[ident];
			}
		} else {
			// if it doesn't go straight to an ident, grab whatever it's loading (most likely a dot)
			return typeInferStmt(method, stmt->rawChildren[0], changed, ret_flag);
		}

	} else if (nodeType == INTCONST) {
		return "Int";

	} else if (nodeType == STRCONST) {
		return "String";

	} else if (nodeType == IDENT) {
		if (stmt->name == "Nothing") return "Nothing";
		if (stmt->name == "true" || stmt->name == "false") return "Boolean";
	}

	return "$UNKNOWN";
}

bool Typechecker::typeInferQmethod(Qmethod *method, bool &changed) {
	bool ret_flag = true;
	if (method->stmts.empty()) return ret_flag;

	for (AST::Node* stmt : method->stmts) {
		typeInferStmt(method, stmt, changed, ret_flag);
	}

	return ret_flag;
}

bool Typechecker::typeInferenceCheck() {
	bool ret_flag = true;

	// check all constructors first so all possible instance vars have types
	bool changed;
	do {
		changed = false;
		for (auto clss : this->classes) {
			if (isBuiltin(clss.second->name)) { 
				continue;
			}
			if (!typeInferQmethod(clss.second->constructor, changed)) ret_flag = false;
		}
	} while (changed);

	// check all other methods and main
	do {
		changed = false;
		for (auto clss : this->classes) {
			if (isBuiltin(clss.second->name)) { 
				continue;
			}
			for (Qmethod *m : clss.second->methods) {
				if(!typeInferQmethod(m, changed)) ret_flag = false;
			}
		}

		// init check the main statements
		if (this->main != NULL) {
			if (!typeInferQmethod(this->main->constructor, changed)) ret_flag = false;
		}
	} while (changed);

	return ret_flag;
}

bool Typechecker::fieldsCompatibleCheck() {
	bool ret_flag = true;

	for (auto map_entry : this->classes) {
		std::string name = map_entry.first;
		Qclass *child = map_entry.second;
		Qclass *parent = this->classes[child->super];

		for (std::string instanceVar : parent->instanceVars) {
			if (instanceVar == "this") continue; // don't want to compare the actual classes
			if (std::find(child->instanceVars.begin(), child->instanceVars.end(), instanceVar) == child->instanceVars.end()) {
				RED << stageString(INITBEFOREUSE) << "child class \"" << child->name << "\" does not define field \"" 
					<< instanceVar << "\"" << END;
					report::trackError(INITBEFOREUSE);
				ret_flag = false;
			} else { // we know the child defined the field, now check its type
				std::string childType = child->instanceVarType[instanceVar];
				std::string parentType = parent->instanceVarType[instanceVar];
				if (childType != parentType) {
					RED << stageString(TYPEINFERENCE) << "child class \"" << child->name << "\" defines field \"" 
						<< instanceVar << "\" with incorrect type \"" << childType << "\" (should be \"" << parentType << "\")" << END;
						report::trackError(TYPEINFERENCE);
					ret_flag = false;
				}
			}
		}
	}

	return ret_flag;
}

bool Typechecker::checkProgram() {
	// Type checking: phase one
	// - check for circular dependency
	// - typecheck all method signatures
	// - check if class method definitions are compatible with parent's
	// (check if class has already been declared is done in initialize())
    // (check if class extends no such super is done in initialize())
    // (check if method shares same name as class is done in createQmethod())
    // (check for duplicate methods is done in createQmethod())

    // don't want to check things when generating AST
	if (report::getGenerateImage()) {
		return true;
	}

	bool classHierarchyValid = this->classHierarchyCheck();
    if (!classHierarchyValid) {
        report::error("class hierarchy check failed - circular dependency detected!", TYPECHECKER);
        report::bail(CLASSHIERARCHY);
    } else if (report::ok()) {
        report::gnote("circular dependency check passed.", TYPECHECKER);
    }

    bool methodsWelltyped = this->methodSignaturesTypecheck();
    if (!methodsWelltyped) {
    	report::error("method signature type check failed!", TYPECHECKER);
        // don't bail here so we can see more errors
    } else if (report::ok()) {
        report::gnote("method signature type check passed.", TYPECHECKER);
    }

    bool methodsCompatible = this->methodsCompatibleCheck();
    if (!methodsCompatible) {
    	report::error("method compatibility check failed!", TYPECHECKER);
        // don't bail here so we can see more errors
    } else if (report::ok()) {
        report::gnote("method compatibility check passed.", TYPECHECKER);
    }

    bool initBeforeUseCheckValid = this->initializeBeforeUseCheck();
    if (!initBeforeUseCheckValid) {
        report::error("initialization before use check failed!", TYPECHECKER);
        report::bail(INITBEFOREUSE);
    } else if (report::ok()) {
        report::gnote("initialization before use check passed.", TYPECHECKER);
    }

    bool typeInferenceCheckValid = this->typeInferenceCheck();
    if (!typeInferenceCheckValid) {
        report::error("type inference check failed!", TYPECHECKER);
        report::bail(TYPEINFERENCE);
    } else if (report::ok()) {
        report::gnote("type inference check passed.", TYPECHECKER);
    }

    bool fieldsCompatible = this->fieldsCompatibleCheck();
    if (!fieldsCompatible) {
        report::error("field compatibility check failed!", TYPECHECKER);
        report::bail(TYPEINFERENCE);
    } else if (report::ok()) {
        report::gnote("field compatibility check passed.", TYPECHECKER);
    }

    if (report::getVerbose()) {
	    for (auto clzz : this->classes) {
	    	printQclass(clzz.second);
	    }
	    if (this->main != NULL) printQclass(main);
	}

    // if we reach the end we know nothing has failed, return true to the driver
    return true;
}

bool Typechecker::isVarInit(Qmethod *method, std::string ident) {
	return std::find(method->init.begin(), 
		method->init.end(), ident) != method->init.end();
}

bool Typechecker::isVarArg(Qmethod *method, std::string ident) {
	return std::find(method->args.begin(), 
		method->args.end(), ident) != method->args.end();
}

bool Typechecker::isVarExplicit(Qmethod *method, std::string ident) {
	return std::find(method->explicitVars.begin(), 
		method->explicitVars.end(), ident) != method->explicitVars.end();
}

bool Typechecker::isInstanceVar(Qmethod *method, std::string ident) {
	return std::find(method->clazz->instanceVars.begin(), 
		method->clazz->instanceVars.end(), ident) != method->clazz->instanceVars.end();
}

bool Typechecker::isInstanceVarExplicit(Qmethod *method, std::string ident) {
	return std::find(method->clazz->explicitFields.begin(), 
		method->clazz->explicitFields.end(), ident) != method->clazz->explicitFields.end();
}

std::string Typechecker::getSuperClass(std::string class1) {
	return classes[class1]->super;
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
	if (class1 == "" || class1 == "$UNKNOWN") return true; // for explicit type check when the assigned var is in a conditional branch
									// and doesn't even have a type of "$UNKNOWN" yet
	if (class2 == "$UNKNOWN") return false;
	if (class1 == class2) return true;
	if (class2 == "Obj") return true;
	if (classes[class1]->super == "Obj" && class2 == "Obj") return true;

	std::string tempSuper = class1;
	while (tempSuper != class2 || tempSuper != "Obj") {
		if (tempSuper == "Obj") break;
		if (tempSuper == class2) return true;

		for (auto clss : classes) {
			Qclass *second = clss.second;
			if (second->name == tempSuper) {
				tempSuper = second->super;
			}
		}
	}
	return false;
}

// the old type in type inference must be passed in as the first argument
// this is so we can know when we see a variable for the first time, and
// assign it to the type of the r_expr directly instead of computing the LCA.
std::string Typechecker::leastCommonAncestor(std::string class1, std::string class2) {
	if (class1 == class2) return class1;
	if (class1 == "$UNKNOWN" || class1 == "") return class2;
	if (!doesClassExist(class1) || !doesClassExist(class2)) return "$UNKNOWN";

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
			class1Supers.push_back(classes[tempSuper1]->super);
			tempSuper1 = classes[tempSuper1]->super;
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
			class2Supers.push_back(classes[tempSuper2]->super);
			tempSuper2 = classes[tempSuper2]->super;
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

void Typechecker::printQclass(Qclass *clazz) {
	if (isBuiltin(clazz->name)) { 
		return;
	}
	OUT << "******************| class " << clazz->name << " |******************" << END;
	OUT << "Super: " << clazz->super << END << END;
		
	OUT << "	Instance vars: " << END;
	for (std::string s : clazz->instanceVars) {
		OUT << "	Name: " << s << ", Type: " << clazz->instanceVarType[s];
		if (isInstanceVarExplicit(clazz->constructor, s)) {
			OUT << " (explicit)";
		}
		OUT << END;
	}
	OUT << END;

	printQmethod(clazz->constructor);
	for (Qmethod *m : clazz->methods) {
		printQmethod(m);
	}
}

void Typechecker::printQmethod(Qmethod *method) {
	OUT << "	~~--~~| method " << method->name << " |~~--~~ " << END;
	OUT << "	Inside class: " << method->clazz->name << END;
	OUT << "	Return type: " << method->type["return"] << END;
	OUT << "	Initialized vars: " << END;
	for (std::string s : method->init) {
		OUT << "		" << s;
		if (isVarArg(method, s)) {
			OUT << " (arg) (explicit)";
		}
		if (isVarExplicit(method, s)) {
			OUT << " (explicit)";
		}
		OUT << END;
	}
	
	OUT << "	Known types: " << END;
	for (std::map<std::string, std::string>::iterator it = method->type.begin(); it != method->type.end(); ++it) {
	 	OUT << "		" << it->first << ", " << it->second << END;
	}
	OUT << "	Stmts: " << END;
	for (AST::Node *stmt : method->stmts) { // print each stmt type inside Qmethod's stmts vector
			OUT << "		" << typeString(stmt->type) << END;
	}
	OUT << END;
}