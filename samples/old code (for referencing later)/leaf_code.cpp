	// leaf node, push onto leaves return vector
	else if (nodeType == INTCONST || nodeType == STRCONST || nodeType == IDENT) {
		if (stmt->isLeaf) {
			// if (stmt->isInstanceVar) {
			// 	if (!isInstanceVar(method, stmt->name)) {
			// 		RED << stageString(INITBEFOREUSE) << "attempt to use uninitialized instance variable \"this."
			// 			<< stmt->name << "\" in " << method->name << "() in class \""
			// 			<< method->clazz->name << "\"" << END;
			// 		report::trackError(INITBEFOREUSE);
			// 		ret_flag = false;
			// 	}
			// } else {
			// 	if (!isVarInit(method, stmt->name)) {
			// 		RED << stageString(INITBEFOREUSE) << "attempt to use uninitialized variable \""
			// 			<< stmt->name << "\" in " << method->name << "() in class \""
			// 			<< method->clazz->name << "\"" << END;
			// 		report::trackError(INITBEFOREUSE);
			// 		ret_flag = false;
			// 	}
			// }
			ret_vec.push_back(stmt);
		}
		
		return ret_flag;
	}