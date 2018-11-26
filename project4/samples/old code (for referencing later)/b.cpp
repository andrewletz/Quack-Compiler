void Typechecker::getLeaves(AST::Node *stmt, std::vector<AST::Node *> &ret_vec) {
	Type nodeType = stmt->type;
	if (nodeType == DOT) {
		// for (AST::Node *s : stmt->rawChildren) {
		// 	OUT << typeString(s->type) << ", " << typeString(s->subType) << END;
		// }
		// OUT << END;
		if (stmt->get(LOAD) != NULL) {
			if (stmt->get(LOAD)->get(IDENT)->name == "this") {
				OUT << "found this" << END;
			}
		}
	} 

	if (nodeType == INTCONST || nodeType == STRCONST || nodeType == IDENT) {
		ret_vec.push_back(stmt);
	} else {
		for (AST::Node *child : stmt->rawChildren) {
			getLeaves(child, ret_vec);
		}
	}
}


for (AST::Node *s : stmt->rawChildren) {
	OUT << typeString(s->type) << ", " << typeString(s->subType) << END;
}
OUT << END;

load_, l_expr_
ident_, r_expr_