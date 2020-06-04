#include "core/string_builder.h"

#include "gdscript_transpiler_utils.h"
#include "gdscript_transpiler_cxx.h"

String GDScriptTranspilerCpp::get_name() const {
    return "C++";
}

Variant GDScriptTranspilerCpp::transpile(const Ref<GDScript> &p_script) {
    ERR_FAIL_COND_V_MSG(p_script.is_null(), Dictionary(), "Invalid script.")
    
    GDScriptParser parser;
    script_path = p_script->get_path();
    parser.parse(p_script->get_source_code(), script_path.get_base_dir(), false, script_path, false, nullptr, false);
    
    const GDScriptParser::Node *gd_tree = parser.get_parse_tree();
    ERR_FAIL_COND_V(gd_tree->type != GDScriptParser::Node::TYPE_CLASS, Variant());
    
	const Node *cpp_tree = translate_node(gd_tree);
    
    code["header"] = GDScriptTranspilerUtils::CodeBuilder();
    code["source"] = GDScriptTranspilerUtils::CodeBuilder();
	transpile_node(cpp_tree);
    
    Dictionary ret;
    for (Map<String, GDScriptTranspilerUtils::CodeBuilder>::Element *E = code.front(); E; E = E->next()) {
        ret[E->key()] = E->get().get_code();
    }
    clear();
    
    return ret;
}

template <class T>
T *GDScriptTranspilerCpp::new_node() {
	T *node = memnew(T);

	node->next = list;
	list = node;

	if (!head) {
		head = node;
    }
	return node;
}

String GDScriptTranspilerCpp::to_string(const GDScriptParser::DataType &p_datatype) {
    String type = p_datatype.to_string();
    if (!p_datatype.has_type) {
        type = "Variant";
    } else if (p_datatype.builtin_type == Variant::NIL) {
        type = "void";
    }
    return type;
}

void GDScriptTranspilerCpp::clear() {
    while (list) {
		Node *l = list;
		list = list->next;
		memdelete(l);
	}
	head = nullptr;
	list = nullptr;
    
    code.clear();
    parsed_expression.clear();
}

GDScriptTranspilerCpp::Node *GDScriptTranspilerCpp::translate_node(const GDScriptParser::Node *p_node) {
    if (!p_node) {
        return nullptr;
    }
    switch (p_node->type) {
		case GDScriptParser::Node::TYPE_CLASS: {
            auto class_node = static_cast<const GDScriptParser::ClassNode *>(p_node);
            return translate_class(class_node);
        } break;
		case GDScriptParser::Node::TYPE_FUNCTION: {
            auto func_node = static_cast<const GDScriptParser::FunctionNode *>(p_node);
            return translate_function(func_node);
        } break;
		case GDScriptParser::Node::TYPE_BUILT_IN_FUNCTION: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");    
        } break;
		case GDScriptParser::Node::TYPE_BLOCK: {
            auto block_node = static_cast<const GDScriptParser::BlockNode *>(p_node);
            return translate_block(block_node);
        } break;
		case GDScriptParser::Node::TYPE_IDENTIFIER: {
            auto gd_id = static_cast<const GDScriptParser::IdentifierNode *>(p_node);
            auto id = new_node<IdentifierNode>();
            
            id->name = String(gd_id->name);
            id->datatype = to_string(gd_id->datatype);
            
            return id;
        } break;
		case GDScriptParser::Node::TYPE_TYPE: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");
        } break;
		case GDScriptParser::Node::TYPE_CONSTANT: {
            auto gd_cnode = static_cast<const GDScriptParser::ConstantNode *>(p_node);
            auto cpp_cnode = new_node<ConstantNode>();
            cpp_cnode->value = gd_cnode->value;
            cpp_cnode->datatype = to_string(gd_cnode->datatype);
            cpp_cnode->is_enum = gd_cnode->datatype.builtin_type == Variant::DICTIONARY;
            return cpp_cnode;
        } break;
		case GDScriptParser::Node::TYPE_ARRAY: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");
        } break;
		case GDScriptParser::Node::TYPE_DICTIONARY: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");
        } break;
		case GDScriptParser::Node::TYPE_SELF: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");
        } break;
		case GDScriptParser::Node::TYPE_OPERATOR: {
            auto op_node = static_cast<const GDScriptParser::OperatorNode *>(p_node);
            auto cpp_op_node = new_node<OperatorNode>();
            cpp_op_node->op = OperatorNode::Operator(op_node->op);
            
            Vector<Node *> arguments;
            for (int i = 0; i < op_node->arguments.size(); i++) {
                arguments.push_back(translate_node(op_node->arguments[i]));
            }
            cpp_op_node->arguments = arguments;
            cpp_op_node->datatype = to_string(op_node->datatype);
            
            return cpp_op_node;
        } break;
		case GDScriptParser::Node::TYPE_CONTROL_FLOW: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");
        } break;
		case GDScriptParser::Node::TYPE_LOCAL_VAR: {
            auto gd_lvar = static_cast<const GDScriptParser::LocalVarNode *>(p_node);
            auto lvar = new_node<LocalVarNode>();
            
            lvar->name = String(gd_lvar->name);
            lvar->assign = translate_node(gd_lvar->assign);
            lvar->assign_op = static_cast<OperatorNode *>(translate_node(gd_lvar->assign_op));
            lvar->datatype = to_string(gd_lvar->datatype);
            
            return lvar;
        } break;
		case GDScriptParser::Node::TYPE_CAST: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");
        } break;
		case GDScriptParser::Node::TYPE_ASSERT: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");
        } break;
		case GDScriptParser::Node::TYPE_BREAKPOINT: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");
        } break;
		case GDScriptParser::Node::TYPE_NEWLINE: {
            return nullptr; // skip
        } break;
        default: {
            ERR_FAIL_V_MSG(nullptr, "Type not implemented yet.");
        }
    }
}

GDScriptTranspilerCpp::ClassNode *GDScriptTranspilerCpp::translate_class(const GDScriptParser::ClassNode *p_class) {
    const GDScriptParser::ClassNode *gd_class = p_class;
    auto cpp_class = new_node<ClassNode>();
    
    // Class
    if (gd_class->classname_used) {
        cpp_class->class_name = gd_class->name;
    } else {
        cpp_class->class_name = GDScriptTranspilerUtils::filepath_to_pascal_case(script_path);
    }
    cpp_class->inherits = "Reference"; // by default
    if (gd_class->extends_used) {
        if (gd_class->extends_file != String()) {
            cpp_class->inherits = GDScriptTranspilerUtils::filepath_to_pascal_case(String(gd_class->extends_file));
        } else if (!gd_class->extends_class.empty()) {
            cpp_class->inherits = gd_class->extends_class[0];
        }
    }
    // Members
    for (int i = 0; i < gd_class->variables.size(); ++i) {
        const GDScriptParser::ClassNode::Member &gd_member = gd_class->variables[i];
        ClassNode::Member cpp_member;
        cpp_member.info = gd_member._export;
        cpp_member.identifier = gd_member.identifier;
        cpp_member.setter = gd_member.setter;
        cpp_member.getter = gd_member.getter;
        
        if (cpp_member.identifier.begins_with("_")) {
            // A convention used to separate private from public members in GDScript.
            cpp_member.access = AccessSpecifier::ACCESS_PRIVATE;
        }
        cpp_member.type = to_string(gd_member.data_type);
        if (!gd_member.data_type.has_type) {
            if (cpp_member.info.hint == PROPERTY_HINT_ENUM) {
                cpp_member.type = "int";
            }
        }
        cpp_member.expression = translate_node(gd_member.expression);
        
        cpp_class->variables.push_back(cpp_member);
    }
    // Constants
    for (auto E = gd_class->constant_expressions.front(); E; E = E->next()) {
        ClassNode::Constant cpp_const;
        cpp_const.expression = translate_node(E->get().expression);
        cpp_const.datatype = to_string(E->get().type);
        cpp_const.is_enum = E->get().type.builtin_type == Variant::DICTIONARY;
        cpp_class->constant_expressions.insert(E->key(), cpp_const);
	}
    // Functions
    for (int i = 0; i < gd_class->functions.size(); ++i) {
        const GDScriptParser::FunctionNode *gd_func = gd_class->functions[i];
        auto cpp_func = static_cast<FunctionNode *>(translate_node(gd_func));
        cpp_class->functions.push_back(cpp_func);
    }
    
    return cpp_class;
}

GDScriptTranspilerCpp::FunctionNode *GDScriptTranspilerCpp::translate_function(const GDScriptParser::FunctionNode *p_function) {
    auto gd_func = p_function;
    auto cpp_func = new_node<FunctionNode>();
    
    cpp_func->name = gd_func->name;
    if (cpp_func->name.begins_with("_")) {
        // A convention used to separate private from public methods in GDScript.
        cpp_func->access = AccessSpecifier::ACCESS_PRIVATE;
    }
    cpp_func->return_type = to_string(gd_func->return_type);
    
    Vector<String> arguments;
    Vector<String> argument_types;
    // We can assume that arguments and types have the same size.
    for (int j = 0; j < gd_func->arguments.size(); ++j) {
        String arg = gd_func->arguments[j];
        arguments.push_back(arg);
        
        const GDScriptParser::DataType &gd_arg_type = gd_func->argument_types[j];
        String type = to_string(gd_arg_type);
        if (gd_arg_type.kind == GDScriptParser::DataType::BUILTIN) {
            if (cpp_func->name.begins_with("set")) {
                type = vformat("const %s &", type);
            }
        }
        argument_types.push_back(type);
    }
    cpp_func->arguments = arguments;
    cpp_func->argument_types = argument_types;
    
    Vector<Node*> default_values;
    for (int j = 0; j < gd_func->default_values.size(); ++j) {
        Node *expression = translate_node(gd_func->default_values[j]);
        default_values.push_back(expression);
    }
    cpp_func->body = static_cast<BlockNode *>(translate_node(gd_func->body));
    
    return cpp_func;
}

GDScriptTranspilerCpp::BlockNode *GDScriptTranspilerCpp::translate_block(const GDScriptParser::BlockNode *p_block) {
    auto cpp_block = new_node<BlockNode>();
    
    List<Node *> statements;
    for (auto E = p_block->statements.front(); E; E = E->next()) {
        auto s = translate_node(E->get());
        statements.push_back(s);
    }
    cpp_block->statements = statements;
    
    Map<StringName, LocalVarNode *> variables;
    for (auto E = p_block->variables.front(); E; E = E->next()) {
        auto cpp_lvar = new_node<LocalVarNode>();
        cpp_lvar->name = String(E->get()->name);
        cpp_lvar->assign = translate_node(E->get()->assign);
        cpp_lvar->assign_op = static_cast<OperatorNode *>(translate_node(E->get()->assign_op));
        cpp_lvar->datatype = to_string(E->get()->datatype);
        variables.insert(cpp_lvar->name, cpp_lvar);
    }
    cpp_block->variables = variables;
    
    cpp_block->has_return = p_block->has_return;
    
    return cpp_block;
}

void GDScriptTranspilerCpp::transpile_node(const Node *p_node) {
    if (!p_node) {
        return;
    }
    GDScriptTranspilerUtils::CodeBuilder &hpp = code["header"];
    GDScriptTranspilerUtils::CodeBuilder &cpp = code["source"];
    
    switch (p_node->type) {
        case Node::TYPE_CLASS: {
            auto class_node = static_cast<const ClassNode *>(p_node);
            transpile_class(class_node);
        } break;
        case Node::TYPE_FUNCTION: {
            
        } break;
        case Node::TYPE_BUILT_IN_FUNCTION: {
            
        } break;
        case Node::TYPE_BLOCK: {
            
        } break;
        case Node::TYPE_IDENTIFIER: {
            
        } break;
        case Node::TYPE_TYPE: {
            
        } break;
        case Node::TYPE_CONSTANT: {
            auto cnode = static_cast<const ConstantNode *>(p_node);
            if (cnode->is_enum) {
                ERR_FAIL_COND(cnode->value.get_type() != Variant::DICTIONARY);
                
                const Dictionary &d = cnode->value;
                List<Variant> keys;
			    d.get_key_list(&keys);
                
                String enum_indent = hpp.get_indent_string();
                String const_indent = enum_indent + hpp.get_indent_sequence();
                
                String str("{\n");
                for (List<Variant>::Element *E = keys.front(); E; E = E->next()) {
                    str += const_indent;
                    str += vformat("%s = %s,\n", E->get(), d[E->get()]);
                }
                str += enum_indent;
                str += "};";
                parsed_expression = str;
            }
            else {
                switch (cnode->value.get_type()) {
                    case Variant::STRING: {
                        parsed_expression = "\"" + String(cnode->value) + "\"";
                    } break;
                    case Variant::BOOL: {
                        bool v = cnode->value;
                        parsed_expression = v ? "true" : "false";
                    } break;
                    case Variant::NIL: {
                        parsed_expression = "nullptr";
                    }
                    default: {
                        parsed_expression = cnode->value;
                    }
                }
            }
        } break;
        case Node::TYPE_ARRAY: {
            
        } break;
        case Node::TYPE_DICTIONARY: {
            
        } break;
        case Node::TYPE_SELF: {
            
        } break;
        case Node::TYPE_OPERATOR: {
            auto op_node = static_cast<const OperatorNode *>(p_node);
            switch (op_node->op) {
                case OperatorNode::OP_CALL: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_PARENT_CALL: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_YIELD: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_IS: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_IS_BUILTIN: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_INDEX: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_INDEX_NAMED: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_NEG: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_POS: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_NOT: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_BIT_INVERT: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_IN: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_EQUAL: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_NOT_EQUAL: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_LESS: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_LESS_EQUAL: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_GREATER: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_GREATER_EQUAL: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_AND: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_OR: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ADD: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_SUB: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_MUL: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_DIV: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_MOD: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_SHIFT_LEFT: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_SHIFT_RIGHT: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_INIT_ASSIGN: {
                    // transpile_node(op_node->arguments[0]);
                    // String a = parsed_expression;
                    // transpile_node(op_node->arguments[1]);
                    // String b = parsed_expression;
                    // parsed_expression = vformat("%s = %s", a, b);
                } break;
                case OperatorNode::OP_ASSIGN: {
                    transpile_node(op_node->arguments[0]);
                    String a = parsed_expression;
                    transpile_node(op_node->arguments[1]);
                    String b = parsed_expression;
                    if (!a.empty()) {
                        parsed_expression = vformat("%s = %s", a, b);
                    } else {
                        parsed_expression = vformat(" = %s", b);
                    }
                } break;
                case OperatorNode::OP_ASSIGN_ADD: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ASSIGN_SUB: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ASSIGN_MUL: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ASSIGN_DIV: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ASSIGN_MOD: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ASSIGN_SHIFT_LEFT: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ASSIGN_SHIFT_RIGHT: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ASSIGN_BIT_AND: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ASSIGN_BIT_OR: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_ASSIGN_BIT_XOR: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_BIT_AND: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_BIT_OR: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_BIT_XOR: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_TERNARY_IF: {
                    // parsed_expression = 
                } break;
                case OperatorNode::OP_TERNARY_ELSE: {
                    // parsed_expression = 
                } break;
            }
        } break;
        case Node::TYPE_CONTROL_FLOW: {
            
        } break;
        case Node::TYPE_LOCAL_VAR: {
            auto lvar = static_cast<const LocalVarNode *>(p_node);
            transpile_node(lvar->assign_op);
            cpp += vformat("%s %s%s;", lvar->datatype, lvar->name, parsed_expression);
        } break;
        case Node::TYPE_CAST: {
            
        } break;
        case Node::TYPE_ASSERT: {
            
        } break;
        case Node::TYPE_BREAKPOINT: {
            
        } break;
        case Node::TYPE_NEWLINE: {
            
        } break;
    }
}

void GDScriptTranspilerCpp::transpile_class(const ClassNode *p_class) {
    // Header
    GDScriptTranspilerUtils::CodeBuilder &hpp = code["header"];
    
    hpp.map["class_name"] = p_class->class_name;
    hpp.map["inherits"] = p_class->inherits;
    hpp.map["include_guard"] = vformat("%s_H", p_class->class_name.to_upper());
    
    hpp += "#ifndef {include_guard}";
    hpp += "#define {include_guard}";
    hpp += "\n";
    hpp += "class {class_name} : public {inherits} {";
    hpp.indent();
    hpp += "GDCLASS({class_name}, {inherits});";
    hpp.dedent();
    hpp += "\n";
    hpp += "public:";
    hpp.indent();
    
    for (auto E = p_class->constant_expressions.front(); E; E = E->next()) {
        const ClassNode::Constant &c = E->get();
        transpile_node(c.expression);
        String value = parsed_expression;
        if (c.is_enum) {
            hpp += vformat("enum %s %s", E->key(), value);
        } else {
            hpp += vformat("const %s %s = %s;", c.datatype, E->key(), value);
        }
    }
    
    for (int i = 0; i < p_class->variables.size(); ++i) {
        const ClassNode::Member &m = p_class->variables[i];
        hpp += vformat("%s %s;", m.type, m.identifier);
    }
    
    hpp += "\n";
    
    for (int i = 0; i < p_class->functions.size(); ++i) {
        const FunctionNode *func = p_class->functions[i];
        String signature;
        for (int j = 0; j < func->arguments.size(); ++j) {
            String type = func->argument_types[j];
            String arg = func->arguments[j];
            signature += vformat("%s %s", type, arg);
            if (j < func->arguments.size() - 1) {
                signature += ", ";
            }
        }
		hpp += vformat("%s %s(%s);", func->return_type, func->name, signature);
	}
    
    hpp.dedent();
    hpp += "};";
    hpp += "\n";
    hpp += "#endif // {include_guard}";
    
    // Source
    GDScriptTranspilerUtils::CodeBuilder &cpp = code["source"];
    cpp.map["class_name"] = p_class->class_name;
    
    String include_header = script_path.get_basename().get_file() + ".h";
    cpp += vformat("#include \"%s\"", include_header);
    cpp += "\n";
    
    for (int i = 0; i < p_class->functions.size(); ++i) {
        const FunctionNode *func = p_class->functions[i];
        String signature;
        for (int j = 0; j < func->arguments.size(); ++j) {
            String type = func->argument_types[j];
            String arg = func->arguments[j];
            signature += vformat("%s %s", type, arg);
            if (j < func->arguments.size() - 1) {
                signature += ", ";
            }
        }
		cpp += vformat("%s {class_name}::%s(%s) {", func->return_type, func->name, signature);
        cpp.indent();
        for (List<Node *>::Element *E = func->body->statements.front(); E; E = E->next()) {
            transpile_node(E->get());
        }
        cpp.dedent();
        cpp += "}\n";
	}
}

GDScriptTranspilerCpp::GDScriptTranspilerCpp() {
    head = nullptr;
	list = nullptr;
    clear();
}

GDScriptTranspilerCpp::~GDScriptTranspilerCpp() {
    clear();
}
