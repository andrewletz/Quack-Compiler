#include "typechecker.h"

std::vector<std::string> intersection(std::vector<std::string> vec1, std::vector<std::string> vec2) {
    std::vector<std::string> ret_vec;

    std::sort(vec1.begin(), vec1.end());
    std::sort(vec2.begin(), vec2.end());

    std::set_intersection(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::inserter(ret_vec, ret_vec.begin()));

    return ret_vec;
}

std::vector<std::string> difference(std::vector<std::string> vec1, std::vector<std::string> vec2) {
	if (vec1.empty()) return vec2;
	if (vec2.empty()) return vec1;
    std::vector<std::string> ret_vec;

    std::sort(vec1.begin(), vec1.end());
    std::sort(vec2.begin(), vec2.end());

    std::set_difference(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::inserter(ret_vec, ret_vec.begin()));

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

	AST::Node *formalsContainer = method->get(FORMAL_ARGS);
	if(!formalsContainer->order.empty()) { // empty formals check before iterating over it
		std::vector<AST::Node *> formals = formalsContainer->getAll(CLASS_ARG);
		for (AST::Node *arg : formals) {
			newMethod->init.push_back(arg->get(IDENT, VAR_IDENT)->name);
			newMethod->args.push_back(arg->get(IDENT, VAR_IDENT)->name);
			newMethod->type[arg->get(IDENT, VAR_IDENT)->name] = arg->get(IDENT, TYPE_IDENT)->name;
			// OUT << "	Formal: " << arg->get(IDENT, VAR_IDENT)->name
			// 		<< ", " << arg->get(IDENT, TYPE_IDENT)->name << END;
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
			// when this code is ran, the only methods in init should be formal args
			for (std::string var : method->init) {
				std::string varType = method->type[var];
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
			for (Qmethod *parentMethod : this->classes[qclss.second->super]->methods) {
				if (method->name == parentMethod->name) { 
					// we must be overriding this method if names match exactly
					// check that types properly line up for overriden method

					// check return types between overriden and parent method
					std::string returnTypeChild = method->type["return"];
					std::string returnTypeParent = parentMethod->type["return"];

					if (!this->isSubclassOrEqual(returnTypeChild, returnTypeParent)) {
						RED << stageString(TYPEINFERENCE) << "overriden method \"" << method->name <<
							"\" in \"" << qclss.second->name << "\" has return type \""
							<< returnTypeChild << "\" inconsistent with parent method" << END;
						report::trackError(TYPEINFERENCE);
						return_flag = false;
					}

					// check argument types between overriden and parent method
					// when this code is ran, the only methods in init should be formal args
					for (std::string var : method->init) {
						if (isVarInit(parentMethod, var)) {
							std::string varTypeChild = method->type[var];
							std::string varTypeParent = parentMethod->type[var];

							if (!isSubclassOrEqual(varTypeChild, varTypeParent)) {
								RED << stageString(TYPEINFERENCE) << "overriden method \"" << method->name <<
									"\" in \"" << qclss.second->name << "\" has argument \""
									<< var << "\" with incorrect type \"" << varTypeChild << "\"" 
									<< " (should be \"" << varTypeParent << "\")" << END;
								report::trackError(TYPEINFERENCE);
								return_flag = false;
							}

						} else {
							RED << stageString(CLASSHIERARCHY) << "overriden method \"" << method->name <<
								"\" in \"" << qclss.second->name << "\" has argument \""
								<< var << "\" not present in parent method" << END;
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

bool Typechecker::fieldsCompatibleCheck() {
	return true;
}

bool Typechecker::initCheckStmt(Qmethod *method, AST::Node *stmt, 
							std::vector<std::string> &var_init,
                            std::vector<std::string> &field_init,
                            bool isConstructor, bool isMainStatements) {
	bool ret_flag = true;
	Type nodeType = stmt->type;

	// control flow has the most complicated case, have to check all syntactic paths
	// init check while loop
	if (nodeType == WHILE) {
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
					if (isConstructor) { // we have found a this.x = ... statement, push back to class's instancevars
						// OUT << "defining this." << left->get(IDENT)->name << END;
						if (method->clazz->name == instanceVar) {
							RED << stageString(INITBEFOREUSE) << "instance variable \""
								<< instanceVar << "\" in class \""
								<< method->clazz->name << "\" shares name with containing class" << END;
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
							// method->clazz->instanceVars.push_back(instanceVar);
							// method->clazz->instanceVarType[instanceVar] = "UNKNOWN";
							field_init.push_back(instanceVar);
						}

						// if it has an explicit type
						// AST::Node *explicit_type = stmt->get(IDENT, TYPE_IDENT);
						// if (explicit_type != NULL) {
						// 	if (method->clazz->instanceVarType.find(instanceVar) == method->clazz->instanceVarType.end()
						// 		|| method->clazz->instanceVarType[instanceVar] == "UNKNOWN") {
						// 		method->clazz->instanceVarType[instanceVar] = explicit_type->name;
						// 	} else {
						// 		RED << stageString(INITBEFOREUSE) << "instance variable \""
						// 			<< instanceVar << "\" in class \""
						// 			<< method->clazz->name << "\" is assigned explicit type more than once" << END;
						// 		report::trackError(INITBEFOREUSE);
						// 		ret_flag = false;
						// 	}
						// }
				 		
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
					// method->init.push_back(left->name);
					// method->type[left->name] = "UNKNOWN";
					var_init.push_back(left->name);
				}
				// if it has an explicit type
				// AST::Node *explicit_type = stmt->get(IDENT, TYPE_IDENT);
				// if (explicit_type != NULL) {
				// 	if (method->type.find(left->name) == method->type.end()
				// 		|| method->type[left->name] == "UNKNOWN") {
				// 		method->type[left->name] = explicit_type->name;
				// 	} else {
				// 		RED << stageString(INITBEFOREUSE) << "variable \""
				// 			<< left->name << "\" in " << method->name <<  "() in class \""
				// 			<< method->clazz->name << "\" is assigned explicit type more than once" << END;
				// 		report::trackError(INITBEFOREUSE);
				// 		ret_flag = false;
				// 	}
				// }
				
			}
		}

	}
	// stmt of form "x.x"
	else if (nodeType == DOT) {
		AST::Node *load = stmt->get(LOAD);
		if (load != NULL) {
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
						<< instanceVar << "\" used in method \"" << method->name << "\" in class \""
						<< method->clazz->name << "\"" << END;
					report::trackError(INITBEFOREUSE);
					ret_flag = false;
				}
			}
		}
	}
	// either a variable of form "x" or a reference to "this"
	else if (nodeType == LOAD) {

		// if we find a "this" on its own, set it to an instance var
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
				if (ident != "false" && ident != "true") {
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

	for (AST::Node *child : stmt->rawChildren) {
		if (!initCheckStmt(method, child, var_init, field_init, isConstructor, isMainStatements)) ret_flag = false;
	}

	return ret_flag;
}

bool Typechecker::initCheckQmethod(Qmethod *method, bool isConstructor, bool isMainStatements) {
	bool ret_flag = true;
	if (method->stmts.empty()) return ret_flag;
	// if you encounter an assign of form this.x, add it to Qclass's instanceVars
	// then check if that instance var shares the same name as any given method in the same class
	// if so, report an error

	// check if l_expr is equal to a literal, if so report an error
	// -> Attempt to assign to a literal in method "__"
	
	// we pass this vector around so we know what we initialized after each statement
	std::vector<std::string> var_init;
	std::vector<std::string> field_init;

	for (AST::Node* stmt : method->stmts) {
		// do some preliminary checks before handing it off to the recursive method
		if (stmt->type == CONSTRUCTOR) {
			// if you found a constructor "Class(x, y, z);" on its own, that is an error (maybe should be a warning?)
			RED << stageString(INITBEFOREUSE) << "constructor not assigned to any variable in method \""
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
			method->type[var] = "UNKNOWN";
		}
		for (std::string field : field_init) {
			method->clazz->instanceVars.push_back(field);
			method->clazz->instanceVarType[field] = "UNKNOWN";
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

bool Typechecker::typeInferenceCheck() {
	return true;
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

    // for (auto clzz : this->classes) {
    // 	printQclass(clzz.second);
    // }

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

bool Typechecker::isInstanceVar(Qmethod *method, std::string ident) {
	return std::find(method->clazz->instanceVars.begin(), 
		method->clazz->instanceVars.end(), ident) != method->clazz->instanceVars.end();
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
	if (class1 == class2) {
		return true;
	}

	if (class1 == "Obj") {
		return false;
	}

	if (classes[class1]->super == "Obj" && class2 == "Obj") {
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
			Qclass *second = clss.second;
			if (second->name == tempSuper) {
				tempSuper = second->super;
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
		
	OUT << "Instance vars: " << END;
	for (std::string s : clazz->instanceVars) {
		OUT << "	Name: " << s << ", Type: " << clazz->instanceVarType[s] << END;
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
			OUT << " (arg)";
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