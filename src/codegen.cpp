#include "codegen.h"
#include "cstring"

bool CodeGenerator::generate() {
	primitives.push_back("String");
	primitives.push_back("Boolean");
	primitives.push_back("Int");
	primitives.push_back("Obj");
	primitives.push_back("Nothing");

	std::ofstream fout(filename);

	// include the built-in functions and classes provided by Professor Young
	fout << "#include \"src/Builtins.h\"" << indent;

    bool classesGenerated = this->generateClasses(fout);
    if (!classesGenerated) {
    	report::error("code for classes could not be generated!", CODEGENERATION);
        report::dynamicBail();
    } else if (report::ok()) {
        report::gnote("code for classes successfully generated.", CODEGENERATION);
    }

	bool mainGenerated = this->generateMain(fout);
    if (!mainGenerated) {
    	report::error("code for main method could not be generated!", CODEGENERATION);
        report::dynamicBail();
    } else if (report::ok()) {
        report::gnote("code for main method successfully generated.", CODEGENERATION);
    }

	return true;
}

bool CodeGenerator::checkPrimitive(std::string name) {
	if (std::find(primitives.begin(), primitives.end(), name) != primitives.end()) {
		return true;
	} 
	return false;
}

bool CodeGenerator::generateClasses(std::ostream &output) {
	// print out the typedef'd structs for every class
	generateTypedefs(output);

	// output the forward declarations of "the_class_CLASSNAME_struct"s
	generateForwardDecls(output);

	// idk what this is but it's in michal young's output so here it is
	generateExterns(output);

	// output the struct obj_CLASSNAME_struct with fields and clazz pointer,
	// output the struct class_CLASSNAME_struct with methods
	generateStructs(output);

	// output the methods for each class
	generateMethods(output);

	// output the singleton classes for each class
	generateSingletons(output);

	// final whitespace!
	output << indent;

	return true;
}

void CodeGenerator::generateTypedefs(std::ostream &output) {
	output << "// -~-~-~-~- Typedefs Begin -~-~-~-~-" << indent;
	for (auto qclass : this->classes) {
		auto currentClass = qclass.second;
		std::string name = currentClass->name;
		Qmethod *constructor = currentClass->constructor;
		if (checkPrimitive(name)) {
			continue;
		} 

		// output the definitions for this class
		output << "// Class " << name << "'s typedefs" << std::endl;
		output << "struct class_" << name << "_struct;" << std::endl;

		output << "typedef struct class_" << name << 
		"_struct* class_"<< name << ";" << std::endl;

		output << "struct obj_" << name << "_struct;" << std::endl;

		output << "typedef struct obj_" << name << 
		"_struct* obj_"<< name << ";" << std::endl;

		output << std::endl;
	}
	output << "// -~-~-~-~- Typedefs End -~-~-~-~-" << indent;
}

void CodeGenerator::generateForwardDecls(std::ostream &output) {
	output << "// -~-~-~-~- Forward Declarations Begin -~-~-~-~-" << std::endl;
	for (auto qclass : this->classes) {
		auto currentClass = qclass.second;
		std::string name = currentClass->name;
		Qmethod *constructor = currentClass->constructor;
		if (checkPrimitive(name)) {
			continue;
		} 
		output << "\n// Class " << name << "'s forward declarations" << std::endl;
		output << "struct class_" << name << "_struct the_class_" << name << "_struct;" << std::endl;
		output << "obj_" << name << " new_" << name << "(";
		i = 0;
		for (auto constructArg : constructor->argtype) {
			if (constructArg.second == name) {
				continue;
			}
			if (i == 0) {
				output << "obj_" << constructArg.second << " " << constructArg.first;
				++i;
			} else {
				output << ", obj_" << constructArg.second << " " << constructArg.first;
			}
		}
		output << ");" << std::endl;
		for (auto method : currentClass->methods) {
			std::string returnType = method->type["return"];
			std::string methodName = method->name;

			output << "obj_" << returnType << " " << name << "_method_" << methodName << "(";
			i = 0;

			if (method->argtype.size() == 1) {
				output << "obj_" << name << " this";
			} else {
				output << "obj_" << name << " this, ";
			}
			for (auto arg : method->argtype) {
				if (arg.first == "Nothing" || arg.first == "return") {
					output << "";
					continue;
				}
				if (i == 0) {
					output << "obj_" << arg.second << " " << arg.first;
					++i;
				} else {
					output << ", obj_" << arg.second << " " << arg.first;
				}
			}
			output << ");" << std::endl;
		}
	}
	output << "\n// -~-~-~-~- Forward Declarations End -~-~-~-~-" << indent;
}

void CodeGenerator::generateStructs(std::ostream &output) {
	output << "// -~-~-~-~- Structs Begin -~-~-~-~-" << indent;
	for (auto qclass : this->classes) {
		auto currentClass = qclass.second;
		std::string name = currentClass->name;
		Qmethod *constructor = currentClass->constructor;
		if (checkPrimitive(name)) {
			continue;
		}
		// if the super class hasn't been printed yet (and isn't a builtin), then print it
		if (std::find(printedClasses.begin(), printedClasses.end(), currentClass->super) == printedClasses.end()) {
			if (!checkPrimitive(currentClass->super)) {
				generateStruct(output, this->classes[currentClass->super]);
				generateConstructor(output, this->classes[currentClass->super]);
				printedClasses.push_back(currentClass->super);
			}
		}
		// if the class hasn't been printed yet, then print it
		if (std::find(printedClasses.begin(), printedClasses.end(), currentClass->name) == printedClasses.end()) {
			generateStruct(output, currentClass);
			generateConstructor(output, currentClass);
			printedClasses.push_back(name);
		}
	}
	output << "// -~-~-~-~- Structs End -~-~-~-~-" << indent;
}

void CodeGenerator::generateStruct(std::ostream &output, Qclass *whichClass) {
	auto currentClass = whichClass;
	std::string name = currentClass->name;
	Qmethod *constructor = currentClass->constructor;

	// output the struct obj_CLASSNAME_struct with fields and clazz pointer
	output << "typedef struct obj_" << name << "_struct {" << std::endl;
	output << "\tclass_" << name << " clazz;" << std::endl;

	// vector of fields we've already printed
	std::vector<std::string> printedFields;

	// print the fields that were "inherited" (declared in super class that we must declare)
	for (auto superField : this->fieldGenerationOrder[currentClass->super]) {
		for (auto field : currentClass->instanceVars) {
			std::string fieldType = currentClass->instanceVarType[field];
			if (field == superField) {
				if (std::find(printedFields.begin(), printedFields.end(), field) == printedFields.end()) {
					output << "\tobj_" << fieldType << " " << field << ";" << std::endl;
					printedFields.push_back(field);
					fieldGenerationOrder[name].push_back(field);
				}
			}
		}
	}

	// print the remaining fields
	for (auto field : currentClass->instanceVars) {
		std::string fieldType = currentClass->instanceVarType[field];
		if (field != "this") {
			if (std::find(printedFields.begin(), printedFields.end(), field) == printedFields.end()) {
				output << "\tobj_" << fieldType << " " << field << ";" << std::endl;
				printedFields.push_back(field);
				fieldGenerationOrder[name].push_back(field);
			}
		}
	}
	output << "} * obj_" << name << ";" << indent;

	// output the "struct class_CLASSNAME_struct"
	output << "struct class_" << name << "_struct {" << std::endl;
	output << "\tclass_Obj super_;" << std::endl;
	output << "\t// Method Table - constructor comes first" << std::endl;

	// print the constructor, which is a special method not inside "methods" vector
	output << "\tobj_" << name << " (*constructor) (";

	int i = 0;
	for (auto constructArg : constructor->argtype) {
		if (constructArg.second == name) {
			continue;
		}
		if (i == 0) {
			output << "obj_" << constructArg.second;
			++i;
		} else {
			output << ", obj_" << constructArg.second;
		}
	}
	output << ");" << std::endl;

	// print out all methods, this includes inherited & overriden ones
	if (checkPrimitive(currentClass->super)) {
		methodOrderer(output, this->classes[currentClass->super]->methods, currentClass);
	} else {
		methodOrderer(output, this->methodGenerationOrder[currentClass->super], currentClass);
	}
}

void CodeGenerator::methodOrderer(std::ostream &output, std::vector<Qmethod *> whereToLook, Qclass *currentClass) {
	std::string name = currentClass->name;
	Qmethod *constructor = currentClass->constructor;

	// vector of methods we've already printed
	std::vector<std::string> printedMethods;
	// vector of names of our methods to check against super classes
	std::vector<std::string> methodNames;
	for (auto method : currentClass->methods) {
		methodNames.push_back(method->name);
	}

	// print the super methods
	for (auto superMethod : whereToLook) {
		// check if the super method is in this class' methods, if it's overriden
		if (std::find(methodNames.begin(), methodNames.end(), superMethod->name) != methodNames.end()) {
			// look for the method that has the same name as the super method
			for (auto method : currentClass->methods) {
				if (method->name == superMethod->name) {
					if (std::find(printedMethods.begin(), printedMethods.end(), method->name) == printedMethods.end()) {
						output << "\tobj_" << method->type["return"] << " (*" << method->name << ") (";
						output << "obj_" << name;
						for (auto arg : method->args) {
							std::string argtype = method->argtype[arg];
							if (method->argtype[arg] != "Nothing") {
									output << ", obj_" << argtype;
							} else {
								output << "obj_" << name;
							}
						}
						output << "); // overriden method" << std::endl;
						printedMethods.push_back(method->name);
						methodGenerationOrder[name].push_back(method);
					}
				}
			}
		} else {
			output << "\tobj_" << superMethod->type["return"] << " (*" << superMethod->name << ") (";
			output << "obj_" << currentClass->super;
			for (auto arg : superMethod->args) {
				std::string argtype = superMethod->argtype[arg];
				if (superMethod->argtype[arg] != "Nothing") {
						output << ", obj_" << argtype;
				} else {
					output << "obj_" << currentClass->super;
				}
			}
			output << "); // inherited from " << currentClass->super << std::endl;
			printedMethods.push_back(superMethod->name);
			currentClass->methods.push_back(superMethod);
			methodGenerationOrder[name].push_back(superMethod);
			classInherited[name].push_back(superMethod->name);
		}
	}

	// print the rest of the class's methods
	for (auto method : currentClass->methods) {
		if (std::find(printedMethods.begin(), printedMethods.end(), method->name) == printedMethods.end()) {
			output << "\tobj_" << method->type["return"] << " (*" << method->name << ") (";
			output << "obj_" << name;
			for (auto arg : method->args) {
				std::string argtype = method->argtype[arg];
				if (method->argtype[arg] != "Nothing") {
						output << ", obj_" << argtype;
				} else {
					output << "obj_" << name;
				}
			}
			output << ");" << std::endl;
			printedMethods.push_back(method->name);
			methodGenerationOrder[name].push_back(method);
		}
	}
	output << "};" << indent;
}

void CodeGenerator::generateConstructor(std::ostream &output, Qclass *currentClass) {
	std::string name = currentClass->name;
	Qmethod *constructor = currentClass->constructor;

	// time to print some methods!
	// begin with the constructor...
	output << "// " << name << "'s constructor method definition" << std::endl;
	output << "obj_" << name << " new_" << name << "(";
	i = 0;
	for (auto constructArg : constructor->argtype) {
		if (constructArg.second == name) {
			continue;
		}
		if (i == 0) {
			output << "obj_" << constructArg.second << " " << constructArg.first;
			++i;
		} else {
			output << ", obj_" << constructArg.second << " " << constructArg.first;
		}
	}
	output << ") {" << std::endl;
	output << "\tobj_" << name << " this = (obj_" << name <<
	") malloc(sizeof(struct obj_" << name << "_struct));" << std::endl;
	output << "\tthis->clazz" << " = " << "the_class_" << name << ";" << std::endl;
	for (auto inited : constructor->type) {
		if (inited.first == "return") {
			continue;
		}
		if (std::find(constructor->args.begin(), constructor->args.end(), inited.first) != constructor->args.end()) {
			continue;
		}
		output << "\tobj_" << inited.second << " " << inited.first << ";" << std::endl;
	}
	for (AST::Node *stmt : constructor->stmts) {
		generateStatement(output, stmt, constructor, name);
	}
	output << "\treturn this;" << std::endl << "}" << indent;
}

void CodeGenerator::generateExterns(std::ostream &output) {
	output << "// -~-~-~-~- Externs Begin -~-~-~-~-" << indent;
	for (auto qclass : this->classes) {
		auto currentClass = qclass.second;
		std::string name = currentClass->name;
		Qmethod *constructor = currentClass->constructor;
		if (checkPrimitive(name)) {
			continue;
		} 

		output << "extern class_" << name << " the_class_" << name << ";" << indent;
	}
	output << "// -~-~-~-~- Externs End -~-~-~-~-" << indent;
}

void CodeGenerator::generateMethods(std::ostream &output) {
	output << "// -~-~-~-~- Methods Begin -~-~-~-~-" << indent;
	for (auto qclass : this->classes) {
		auto currentClass = qclass.second;
		std::string name = currentClass->name;
		Qmethod *constructor = currentClass->constructor;
		if (checkPrimitive(name)) {
			continue;
		}

		// time to print method definitions!
		// ... the rest of the methods, at least
		output << "// " << name << "'s other method definitions" << std::endl;
		for (auto method : currentClass->methods) {
			std::string returnType = method->type["return"];
			std::string methodName = method->name;

			auto it = std::find(classInherited[name].begin(), classInherited[name].end(), method->name);

			if (it != classInherited[name].end()) {
				continue;
			}

			output << "obj_" << returnType << " " << name << "_method_" << methodName << "(";
			i = 0;

			if (method->argtype.size() == 1) {
				output << "obj_" << name << " this";
			} else {
				output << "obj_" << name << " this, ";
			}
			for (auto arg : method->argtype) {
				if (arg.first == "Nothing" || arg.first == "return") {
					output << "";
					continue;
				}
				if (i == 0) {
					output << "obj_" << arg.second << " " << arg.first;
					++i;
				} else {
					output << ", obj_" << arg.second << " " << arg.first;
				}
			}
			output << ")" << " {" << std::endl;
			for (auto inited : method->type) {
				if (inited.first == "return") {
					continue;
				}
				if (std::find(method->args.begin(), method->args.end(), inited.first) != method->args.end()) {
					continue;
				}
				output << "\tobj_" << inited.second << " " << inited.first << ";" << std::endl;
			}
			for (AST::Node *stmt : method->stmts) {
				generateStatement(output, stmt, method, name);
			}
			if (method->type["return"] == "Nothing") {
				output << "\treturn (obj_Nothing) (none);" << std::endl;
			}
			output << "}" << indent;
		}
	}
	output << "// -~-~-~-~- Methods End -~-~-~-~-" << indent;
}

void CodeGenerator::generateSingletons(std::ostream &output) {
	output << "// -~-~-~-~- Singletons Begin -~-~-~-~-" << indent;
	for (auto qclass : this->classes) {
		auto currentClass = qclass.second;
		std::string name = currentClass->name;
		Qmethod *constructor = currentClass->constructor;
		if (checkPrimitive(name)) {
			continue;
		} 
		// printing out the singleton class 
		output << "// The " << name << " class (singleton version)" << std::endl;
		output << "struct class_" << name << "_struct the_class_" << name << "_struct = {" << std::endl;
		output << "\t(class_Obj) &the_class_" << currentClass->super << "_struct," << std::endl;

		// print the singleton's constructor
		output << "\tnew_" << name << ", // constructor" << std::endl;

		std::vector<std::string> alreadyPrinted;
		// print the rest of the singleton's methods
		for (auto method : this->methodGenerationOrder[name]) {
			if (checkPrimitive(currentClass->super)) {
				for (auto superMethod : this->classes[currentClass->super]->methods) {
					if (superMethod->name == method->name) {
						if (std::find(classInherited[name].begin(), classInherited[name].end(), method->name) != classInherited[name].end()) {
							output << "\t" << currentClass->super << "_method_" << method->name << ",";
							output << " // inherited from " << currentClass->super << std::endl;
							alreadyPrinted.push_back(method->name);
							this->classNameByMethod[method->name] = currentClass->super;
						}
					} else {
						if (std::find(classInherited[name].begin(), classInherited[name].end(), method->name) != classInherited[name].end()) {
							output << "";
						} else if (std::find(alreadyPrinted.begin(), alreadyPrinted.end(), method->name) == alreadyPrinted.end()) {
							output << "\t" << name << "_method_" << method->name << "," << std::endl;
							alreadyPrinted.push_back(method->name);
						}
					}
				}
			} else {
				for (auto superMethod : this->classes[currentClass->super]->methods) {
					if (superMethod->name == method->name) {
						if (std::find(classInherited[name].begin(), classInherited[name].end(), method->name) != classInherited[name].end()) {
							output << "\t" << this->classNameByMethod[method->name] << "_method_" << method->name << ",";
							output << " // inherited from " << this->classNameByMethod[method->name] << std::endl;
							alreadyPrinted.push_back(method->name);
						}
					} else {
						if (std::find(classInherited[name].begin(), classInherited[name].end(), method->name) != classInherited[name].end()) {
							output << "";
						} else if (std::find(alreadyPrinted.begin(), alreadyPrinted.end(), method->name) == alreadyPrinted.end()) {
							output << "\t" << name << "_method_" << method->name << "," << std::endl;
							alreadyPrinted.push_back(method->name);
						}
					}
				}
			}
		}
		output << "};" << indent;

		output << "class_" << name << " the_class_" << name << " = " << "&the_class_" << name << "_struct;" << std::endl;
	}
	output << indent;
	output << "// -~-~-~-~- Singletons End -~-~-~-~-" << indent;
}

bool CodeGenerator::generateMain(std::ostream &output) {
	output << "// -~-~-~-~- Main Method - it's the end! -~-~-~-~-" << std::endl;

	output << "int main(int argc, char *argv[]) {" << std::endl;

	Qclass *mainClass = this->tc->main;

	if (mainClass) {
		Qmethod *mainConstruct = mainClass->constructor;
		std::vector<AST::Node *> mainStatements = mainConstruct->stmts;

		for (auto inited : mainConstruct->type) {
				if (inited.first == "return") {
					continue;
				}
				if (std::find(mainConstruct->args.begin(), mainConstruct->args.end(), inited.first) != mainConstruct->args.end()) {
					continue;
				}
				output << "\tobj_" << inited.second << " " << inited.first << ";" << std::endl;
		}

		if (!mainStatements.empty()) {
			for (AST::Node *stmt : mainStatements) {
				generateStatement(output, stmt, mainClass->constructor);
			}
		}
	}

	output << "\n\treturn 0;\n}" << std::endl;

	return true;
}

std::string CodeGenerator::generateStatement(std::ostream &output, AST::Node *stmt, Qmethod *whichMethod, std::string whichClass) {
	Type nodeType = stmt->type;
	//std::cerr << "the nodeType is : " << typeString(nodeType) << std::endl;
	std::string name = whichClass;
	Qclass *currentClass;
	if (whichClass != "main") {
		Qclass *currentClass = classes[whichClass];
		name = currentClass->name;
	}

	if (nodeType == TYPECASE) {
		AST::Node *var = stmt->get(LOAD);
		std::string typeSwitch = generateStatement(output, var, whichMethod, name);
		bool z = false;
		std::string switchType = this->tc->typeInferStmt(whichMethod, var, z, z);
		
		std::string tempClass = "(class_" + switchType + ") " + typeSwitch + "->clazz";

		std::string temp = "tempClass" + std::to_string(this->tempno);
		output << "\tclass_" << switchType << " " << temp << " = " << tempClass << ";" << std::endl;

		AST::Node *type_alts_container = stmt->get(TYPE_ALTERNATIVES);
		std::vector<AST::Node *> type_alts = type_alts_container->getAll(TYPE_ALTERNATIVE);

		output << "\twhile (" << temp << ") {" << std::endl;

		for (AST::Node *type_alt : type_alts) {
			AST::Node *ident = type_alt->getBySubtype(VAR_IDENT);
			AST::Node *ident_type = type_alt->getBySubtype(TYPE_IDENT);
			AST::Node *type_stmts = type_alt->get(BLOCK, STATEMENTS);
			output << "\tif (" << temp << " == (class_" << switchType << ") the_class_" << ident_type->name << ") {" << std::endl;
			output << "\t" << ident->name << " = " << "(obj_" << ident_type->name << ") " << typeSwitch << ";" << std::endl;
			for (AST::Node *type_stmt : type_stmts->rawChildren) {
				generateStatement(output, type_stmt, whichMethod, name);
			}
			output << "\tbreak;" << std::endl << "\t}" << std::endl;
		}
		output << "\telse {" << std::endl << "\t" << temp << " = " << temp << "->super;" << std::endl << "\t}" << std::endl;
		output << "\t}" << std::endl;
		return "";
	}

	if (nodeType == WHILE) {
		AST::Node *cond = stmt->get(COND)->rawChildren[0];

		std::string testcondString = "test_cond" + std::to_string(this->tempno);
		std::string loopagainString = "loop_again" + std::to_string(this->tempno);
		std::string endwhileString = "end_while" + std::to_string(this->tempno);
		std::string halfwayString = "halfway" + std::to_string(this->tempno);

		output << "\tgoto " << testcondString << ";" << std::endl;
		output << "\t" << loopagainString << ": ; // Null statement" << std::endl; 

		++this->tempno;
		AST::Node *while_stmts = stmt->get(BLOCK, STATEMENTS);
		for (AST::Node *while_stmt : while_stmts->rawChildren) {
			generateStatement(output, while_stmt, whichMethod, name);
		}

		output << "\t" << testcondString << ": ; // Null statement" << std::endl;
		output << "\tgoto " << halfwayString << ";" << std::endl;
		output << "\t" << halfwayString << ": ; // Null statement" << std::endl;

		if (cond != NULL) {
			std::string condStatement = generateStatement(output, cond, whichMethod, name);
			output << "\tif (lit_true == " << condStatement << ") { goto " << loopagainString << "; }" << std::endl;
			output << "\tgoto " << endwhileString << ";" << std::endl;
		}
		output << "\t" << endwhileString << ": ; // Null statement" << std::endl;

		return "";
	}

	if (nodeType == IF) {
		AST::Node *cond = stmt->get(COND)->rawChildren[0];
		if (cond != NULL) {
			std::string condName = generateStatement(output, cond, whichMethod, name);
			output << "\tif (lit_true == " << condName << ") { goto if" << this->tempno << "; }" << std::endl;
		}
		std::string ifString = "if" + std::to_string(this->tempno);
		std::string elseString = "else" + std::to_string(this->tempno);
		std::string endifString = "endif" + std::to_string(this->tempno);

		output << "\tgoto " << elseString << ";" << std::endl;
		output << "\t// if statement true part!" << std::endl;

		output << "\t" << ifString << ": ; // Null statement" << std::endl;

		++this->tempno;
		AST::Node *true_stmts = stmt->get(BLOCK, TRUE_STATEMENTS);
		for (AST::Node *true_stmt : true_stmts->rawChildren) {
			std::string generatedTrue = generateStatement(output, true_stmt, whichMethod, name);
		}
		output << "\tgoto " << endifString << ";" << std::endl;
	
		output << "\t// if statement false part!" << std::endl;

		output << "\t" << elseString << ": ; // Null statement" << std::endl;

		AST::Node *false_stmts = stmt->get(BLOCK, FALSE_STATEMENTS);
		for (AST::Node *false_stmt : false_stmts->rawChildren) {
			std::string generatedFalse = generateStatement(output, false_stmt, whichMethod, name);
		}
		output << "\tgoto " << endifString << ";" << std::endl;

		output << "\t" << endifString << ": ; // Null statement" << std::endl;

		return "";
	}

	if (nodeType == CONSTRUCTOR) {
		AST::Node *class_name_node = stmt->get(IDENT);
		std::string retVal;
		if (class_name_node != NULL) {
			std::string class_name = class_name_node->name;
			retVal =  "new_" + class_name + "(";
			Qclass *qclass = this->classes[class_name];
			Qmethod *constructorMethod = qclass->constructor;
			int numArgsConstructor = constructorMethod->args.size();

			// make sure all our args line up
			AST::Node *actual_args_container = stmt->get(ACTUAL_ARGS);
			if (actual_args_container != NULL) {
				std::vector<AST::Node *> actual_args = actual_args_container->getAll(METHOD_ARG);
				if (!actual_args.empty()) {
					std::vector<std::string> argNames;
					std::vector<std::string> argTypes;
					for (AST::Node *arg : actual_args) {
						AST::Node *subLexpr = arg->getBySubtype(METHOD_ARG);
						std::string argName = generateStatement(output, subLexpr, whichMethod, name);
						argNames.push_back(argName);
					}

					int numArgsCallToConstructor = argNames.size();
					for (int i = 0; i < numArgsConstructor; i++) {
						std::string constructorArg = constructorMethod->args[i]; // get arg name from class we are constructing
						std::string constructorArgType = constructorMethod->argtype[constructorArg]; // get the type of that arg
						argTypes.push_back(constructorArgType);
						std::string callToConstructorType = argNames[i]; // type of arg i in our call to the constructor
					}

					for (int i = 0; i < numArgsConstructor; ++i) {
						if (i == 0) {
							retVal += ("(obj_" + argTypes[i] + ") ");
							retVal += argNames[i];
						}
						else {
							retVal += (", (obj_" + argTypes[i] + ") ");
							retVal += argNames[i];
						}
					}

					retVal += ")";
				} else { // we have a constructor call with 0 args
					if (numArgsConstructor == 0) { // if the class also has 0 args, great, just return that class name
						retVal += ")";
					}
				}
				output << "\tobj_" << class_name << " tempVar" << this->tempno << " = " << retVal << ";" << std::endl;
				std::string returned = "tempVar" + std::to_string(this->tempno);
				++this->tempno;
				return returned;
			}
		}
	}

	if (nodeType == CALL) { // a call always has 3 children
		// if (stmt->skip) return lhsType; // we dont want to error check again
		AST::Node *lhs = stmt->rawChildren[0]; // left hand side can be any type of node
		std::string lhsStmt = generateStatement(output, lhs, whichMethod, name);
		bool z = false;
		std::string returnType = this->tc->typeInferStmt(whichMethod, stmt, z, z);
		std::string lhsType = this->tc->typeInferStmt(whichMethod, lhs, z, z);
		std::string methodName = stmt->rawChildren[1]->name; // center node is always the ident corresponding to method name

		Qclass *qclass;
		Qclass *qclass_temp;
		Qmethod *calledMethod;

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

		if (methodName == "NOT") {
				output << "\tobj_Boolean tempBool" << this->tempno << " = !" << lhsStmt << ";" << std::endl;
				std::string retVal = "tempBool" + std::to_string(this->tempno);
				++this->tempno;
				return retVal;
		}
		if (methodName == "AND" || methodName == "OR") {
			AST::Node *actual_args_container = stmt->get(ACTUAL_ARGS);
			if (actual_args_container != NULL) {
				std::vector<AST::Node *> actual_args = actual_args_container->getAll(METHOD_ARG);
				if (actual_args.size() == 1) {
					AST::Node *real_arg = actual_args.front()->getBySubtype(METHOD_ARG);
					if (methodName == "AND") {
						// begin and
						output << "\t// and statement beginning!" << std::endl;

						// returned boolean
						output << "\tobj_Boolean tempBool" << this->labelno << " = lit_false;" << std::endl;

						// first side
						output << "\tif (lit_true == " << lhsStmt << ") { goto and_HALFWAY" << this->labelno << "; }" << std::endl;
						output << "\tgoto and_END" << this->labelno << ";" << std::endl;
						output << "\tand_HALFWAY" << this->labelno << ": ; // Null statement" << std::endl;
						
						// second side
						std::string argStmt = generateStatement(output, real_arg, whichMethod, name);
						output << "\tif (lit_true == " << argStmt << ") { goto and_TRUE" << this->labelno << "; }" << std::endl;
						output << "\tgoto and_END" << this->labelno << ";" << std::endl;

						// get the true version
						output << "\tand_TRUE" << this->labelno << ": ; // Null statement" << std::endl;
						output << "\ttempBool" << this->labelno << " = lit_true;" << std::endl;
						// the end, wasn't true
						output << "\tand_END" << this->labelno << ": ; // Null statement" << std::endl;

						// end and
						output << "\t// and statement done!" << std::endl;

						std::string retVal = "tempBool" + std::to_string(this->labelno);
						++this->labelno;
						return retVal;
					} else {
						// begin or
						output << "\t// or statement beginning!" << std::endl;

						// returned boolean
						output << "\tobj_Boolean tempBool" << this->labelno << " = lit_false;" << std::endl;

						// first side
						output << "\tif (lit_true == " << lhsStmt << ") { goto or_TRUE" << this->labelno << "; }" << std::endl;
						output << "\tgoto or_END" << this->labelno << ";" << std::endl;
						output << "\tor_HALFWAY" << this->labelno << ": ; // Null statement" << std::endl;
						
						// second side
						std::string argStmt = generateStatement(output, real_arg, whichMethod, name);
						output << "\tif (lit_true == " << argStmt << ") { goto or_TRUE" << this->labelno << "; }" << std::endl;
						output << "\tgoto or_END" << this->labelno << ";" << std::endl;

						// get the true version
						output << "\tor_TRUE" << this->labelno << ": ; // Null statement" << std::endl;
						output << "\ttempBool" << this->labelno << " = lit_true;" << std::endl;
						// the end, wasn't true
						output << "\tor_END" << this->labelno << ": ; // Null statement" << std::endl;

						// end and
						output << "\t// and statement done!" << std::endl;

						std::string retVal = "tempBool" + std::to_string(this->labelno);
						++this->labelno;
						return retVal;
					}
				}
			}
		}

		// std::cerr << whichMethod->argtype["other"] << std::endl;
		std::vector<std::string> argNames;

		// make sure all our args line up
		AST::Node *actual_args_container = stmt->get(ACTUAL_ARGS);
		if (actual_args_container != NULL) {
			std::vector<AST::Node *> actual_args = actual_args_container->getAll(METHOD_ARG);
			if (!actual_args.empty()) {
				for (AST::Node *arg : actual_args) {
					AST::Node *real_arg = arg->getBySubtype(METHOD_ARG); // the actual arg node is inside of the "METHOD_ARG" node
					std::string argName = generateStatement(output, real_arg, whichMethod, name);
					argNames.push_back(argName);
				}
			} 
		}

		std::string retVal;

		output << "\tobj_" << returnType << " tempResult" << this->tempno << " = " << lhsStmt <<
		"->clazz->" << methodName << "(";
		retVal = "tempResult" + std::to_string(this->tempno);
		++this->tempno;


		int i = 0;
		if (argNames.size() == 0) {
			output << "(obj_" << calledMethod->clazz->name << ") "<< lhsStmt;
		} else {
			output << "(obj_" << calledMethod->clazz->name << ") "<< lhsStmt << ", ";
		}
		for (auto arg : argNames) {
			if (i == 0) {
				output << "(obj_" << calledMethod->argtype[calledMethod->args[i]] << ") " << arg;
				++i;
			} else {
				output << ", (obj_" << calledMethod->argtype[calledMethod->args[i]] << ") " << arg;
				++i;
			}
		}

		output << ");" << std::endl;
		return retVal;
	}

	if (nodeType == ASSIGN) {
		AST::Node *r_expr = stmt->getBySubtype(R_EXPR);
		// assign of form "this.x = ..."
		AST::Node *left = stmt->get(DOT, L_EXPR);
		if (left != NULL) {
			std::string rhs = generateStatement(output, r_expr, whichMethod, name);
			AST::Node *load = left->get(LOAD);
			if (load != NULL) {
				if (load->get(IDENT)->name == "this") { // we have found a this.x = ... statement
					std::string instanceVar = left->get(IDENT)->name;

					output << "\tthis->" << instanceVar << " = " << rhs << ";" << std::endl;

					return "";
				}
			} 
		}

		// assign of form "x = ..." and "x : Clss = ..."
		left = stmt->get(IDENT, LOC);
		if (left != NULL) {
			std::string rhs = generateStatement(output, r_expr, whichMethod, name);

			std::string castType = whichMethod->type[left->name];

			output << "\t" << left->name << " = ";

			output << "(obj_" << castType << ") (" << rhs << ");" << std::endl;

			return "";
		}
	}

	if (nodeType == DOT) {
		AST::Node *load = stmt->get(LOAD);
		if (load != NULL) {
			// we have a "this.x" somewhere in a method, make appropriate checks
			std::string lhs = generateStatement(output, stmt->rawChildren[0], whichMethod, name);
			if (lhs == "this") {
				std::string instanceVar = stmt->get(IDENT)->name;
				std::string completeDot = (lhs + "->" + instanceVar);
				return completeDot;
			} else { // we have an "x.y" somewhere in a method
				std::string instanceVar = stmt->getBySubtype(R_EXPR)->name;
				std::string completeDot = (lhs + "->" + instanceVar);
				return completeDot;
			}
		} else { // if the lhs of the DOT isn't a load, we have to infer its type generically
			std::string lhsTemp = generateStatement(output, stmt->rawChildren[0], whichMethod, name);
			std::string instanceVar = stmt->getBySubtype(R_EXPR)->name;

			std::string completeDot = (lhsTemp + "->" + instanceVar);
			return completeDot;
		}
	}

	if (nodeType == RETURN) {
		AST::Node *load = stmt->getBySubtype(R_EXPR);
		if (load != NULL) {
			std::string returned = generateStatement(output, load, whichMethod, name);
			for (auto tbd : whichMethod->type) {
				if (tbd.first == "return") {
					std::string returnedTypeCast = "(obj_" + tbd.second + ")";
					output << "\treturn " << returnedTypeCast << " (" << returned << ");" << std::endl;
					return "";
				}
			}
		} else {
			output << "\treturn " << "(obj_Obj) none;" << std::endl;
			return "";
		}
	}

	if (nodeType == LOAD) {
		if (stmt->get(IDENT) != NULL) { 
			std::string ident = stmt->get(IDENT)->name;
			if (ident == "this") {
				return "this";
			} else if (ident == "true" || ident == "false") { 
				if (ident == "true") return "lit_true";
				else return "lit_false";
			} else {
				return ident;
			}
		} else {
			// if it doesn't go straight to an ident, grab whatever it's loading (most likely a dot)
			return generateStatement(output, stmt->rawChildren[0], whichMethod, name);
		}
	}

	if (nodeType == INTCONST) {
		output << "\tobj_Int tempInt" << this->tempno <<  " = int_literal(" << stmt->value << ");" << std::endl;
		++this->tempno;
		return ("tempInt" + std::to_string(this->tempno - 1));
	}

	if (nodeType == STRCONST) {
		output << "\tobj_String tempStr" << this->tempno <<  " = str_literal(\"" << stmt->name << "\");" << std::endl;
		++this->tempno;
		return ("tempStr" + std::to_string(this->tempno - 1));
	}

	if (nodeType == IDENT) {
		if (stmt->name == "true" || stmt->name == "false") {
			output << "\tobj_Boolean tempBool" << this->tempno <<  " = " << "lit_" << stmt->name << "_struct" << std::endl;
			++this->tempno;
			return ("tempBool" + std::to_string(this->tempno - 1));
		} else {
			std::cerr << "got to ident that isn't a bool?" << std::endl;
		}
	}
}