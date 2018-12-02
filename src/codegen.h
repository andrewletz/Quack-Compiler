#ifndef CODEGEN_H
#define CODEGEN_H

#include <fstream>
#include "ASTNode.h"
#include "Messages.h"
#include "typechecker.h"
#include <list>

class CodeGenerator {
	public:
        /* ============ */
        /* Data Members */
        /* ============ */

        // classes from typechecker, typechecker instance
		std::map<std::string, Qclass*> classes;
		Typechecker *tc;
		std::vector<std::string> primitives = {};
		std::vector<std::string> printedClasses;
		std::map<std::string, std::vector<Qmethod *>> methodGenerationOrder;
		std::map<std::string, std::vector<std::string>> fieldGenerationOrder;
		std::map<std::string, std::vector<std::string>> classInherited;
		std::map<std::string, std::string> classNameByMethod;

		// indent for codegen
        std::string indent = "\n\n";

        // filename for outputting
        std::string filename;

        // variable for counting temps, variable for printing arguments correctly
        int labelno = 0;
        int tempno = 0;
        int i = 0;

        /* ========================== */
        /* Constructors & Destructors */
        /* ========================== */

        CodeGenerator(Typechecker *tc, std::string fname) : classes(tc->classes), filename(fname), tc(tc) { };
        virtual ~CodeGenerator() { };

        /* ======= */
        /* Methods */
        /* ======= */

		/* ==== main code generation methods ==== */
		bool generate();
        bool generateClasses(std::ostream &output);
        bool generateMain(std::ostream &output);

        /* ==== helper functions for the main code generation ==== */
        bool checkPrimitive(std::string name);
        void generateTypedefs(std::ostream &output);
        // helper functions for generateClasses
        void generateForwardDecls(std::ostream &output);
		void generateStructs(std::ostream &output);
		void generateStruct(std::ostream &output, Qclass *whichClass); // helper function for generateStructs
		void generateConstructor(std::ostream &output, Qclass *whichClass); // helper function for generateStructs
		void methodOrderer(std::ostream &output, std::vector<Qmethod *> whereToLook, Qclass *currentClass);
		void generateExterns(std::ostream &output);
		void generateMethods(std::ostream &output);
		void generateSingletons(std::ostream &output);
		// helper functions for generateMain
        void generateMainCall(std::ostream &output, AST::Node *stmt);
        // helper function for generating statements
        std::string generateStatement(std::ostream &output, AST::Node *stmt, Qmethod *whichMethod, std::string whichClass="main");
};

#endif