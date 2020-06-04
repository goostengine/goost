#ifndef GDSCRIPT_TRANSPILER_CPP_H
#define GDSCRIPT_TRANSPILER_CPP_H

#include "gdscript_transpiler.h"

class GDScriptTranspilerCpp : public GDScriptTranspilerLanguage {
public:
	enum AccessSpecifier {
		ACCESS_PUBLIC,
		ACCESS_PROTECTED,
		ACCESS_PRIVATE,
	};
	struct Node {
		enum Type {
			TYPE_CLASS,
			TYPE_FUNCTION,
			TYPE_BUILT_IN_FUNCTION,
			TYPE_BLOCK,
			TYPE_IDENTIFIER,
			TYPE_TYPE,
			TYPE_CONSTANT,
			TYPE_ARRAY,
			TYPE_DICTIONARY,
			TYPE_SELF,
			TYPE_OPERATOR,
			TYPE_CONTROL_FLOW,
			TYPE_LOCAL_VAR,
			TYPE_CAST,
			TYPE_ASSERT,
			TYPE_BREAKPOINT,
			TYPE_NEWLINE,
		};
		Node *next;
		Type type;
		AccessSpecifier access;
		
		Node() {
			next = nullptr;
			access = ACCESS_PUBLIC; // everything is public in GDScript
		}
	};
	
	struct FunctionNode;
	struct BlockNode;
	struct ConstantNode;
	struct LocalVarNode;
	struct OperatorNode;
	
	struct ClassNode : public Node {
		String class_name;
		String inherits;
		
		struct Member {
			PropertyInfo info;
			String identifier;
			// DataType data_type;
			String setter;
			String getter;
			String type;
			Node *expression;
			AccessSpecifier access;
			// OperatorNode *initial_assignment;
			// MultiplayerAPI::RPCMode rpc_mode;
			// int usages;
			Member() {
				expression = nullptr;
				access = ACCESS_PUBLIC; // everything is public in GDScript
			}
		};
		struct Constant {
			Node *expression;
			String datatype;
			bool is_enum;
			
			Constant() {
				is_enum = false;
				expression = nullptr;
			}
		};
		Vector<Member> variables;
		Map<StringName, Constant> constant_expressions;
		Vector<FunctionNode *> functions;
		
		ClassNode() {
			type = TYPE_CLASS;
		}
	};
	
	struct FunctionNode : public Node {
		bool is_static;
		// MultiplayerAPI::RPCMode rpc_mode;
		bool has_yield;
		String name;
		String return_type;
		Vector<String> arguments;
		Vector<String> argument_types;
		Vector<Node *> default_values;
		BlockNode *body;

		FunctionNode() {
			type = TYPE_FUNCTION;
			body = nullptr;
			is_static = false;
			// rpc_mode = MultiplayerAPI::RPC_MODE_DISABLED;
			has_yield = false;
		}
	};
	
	struct BlockNode : public Node {
		ClassNode *parent_class;
		BlockNode *parent_block;
		List<Node *> statements;
		Map<StringName, LocalVarNode *> variables;
		bool has_return;

		BlockNode() {
			type = TYPE_BLOCK;
			parent_block = nullptr;
			parent_class = nullptr;
			has_return = false;
		}
	};
	
	struct IdentifierNode : public Node {
		String name;
		String datatype;
		IdentifierNode() {
			type = TYPE_IDENTIFIER;
		}
	};
	
	struct LocalVarNode : public Node {
		String name;
		Node *assign;
		OperatorNode *assign_op;
		// int assignments;
		// int usages;
		String datatype;
		
		LocalVarNode() {
			type = TYPE_LOCAL_VAR;
			assign = nullptr;
			assign_op = nullptr;
			// assignments = 0;
			// usages = 0;
		}
	};
	struct OperatorNode : public Node {
		enum Operator {
			//call/constructor operator
			OP_CALL,
			OP_PARENT_CALL,
			OP_YIELD,
			OP_IS,
			OP_IS_BUILTIN,
			//indexing operator
			OP_INDEX,
			OP_INDEX_NAMED,
			//unary operators
			OP_NEG,
			OP_POS,
			OP_NOT,
			OP_BIT_INVERT,
			//binary operators (in precedence order)
			OP_IN,
			OP_EQUAL,
			OP_NOT_EQUAL,
			OP_LESS,
			OP_LESS_EQUAL,
			OP_GREATER,
			OP_GREATER_EQUAL,
			OP_AND,
			OP_OR,
			OP_ADD,
			OP_SUB,
			OP_MUL,
			OP_DIV,
			OP_MOD,
			OP_SHIFT_LEFT,
			OP_SHIFT_RIGHT,
			OP_INIT_ASSIGN,
			OP_ASSIGN,
			OP_ASSIGN_ADD,
			OP_ASSIGN_SUB,
			OP_ASSIGN_MUL,
			OP_ASSIGN_DIV,
			OP_ASSIGN_MOD,
			OP_ASSIGN_SHIFT_LEFT,
			OP_ASSIGN_SHIFT_RIGHT,
			OP_ASSIGN_BIT_AND,
			OP_ASSIGN_BIT_OR,
			OP_ASSIGN_BIT_XOR,
			OP_BIT_AND,
			OP_BIT_OR,
			OP_BIT_XOR,
			//ternary operators
			OP_TERNARY_IF,
			OP_TERNARY_ELSE,
		};
		Operator op;

		Vector<Node *> arguments;
		String datatype;
		OperatorNode() { type = TYPE_OPERATOR; }
	};
	
	struct ConstantNode : public Node {
		Variant value;
		String datatype;
		bool is_enum;
		
		ConstantNode() {
			type = TYPE_CONSTANT;
			is_enum = false;
		}
	};
private:
	Node *head;
	Node *list;
	template <typename T> 
	T *new_node();
	
	String parsed_expression;
	
	String to_string(const GDScriptParser::DataType &p_type);

protected:
	Node *translate_node(const GDScriptParser::Node *p_node); // expression
	ClassNode *translate_class(const GDScriptParser::ClassNode *p_class);
	FunctionNode *translate_function(const GDScriptParser::FunctionNode *p_function);
	BlockNode *translate_block(const GDScriptParser::BlockNode *p_block);
	
	void transpile_node(const Node *p_node);
	void transpile_class(const ClassNode *p_class);
	// void transpile_expression(const Node *p_expression);
	// void transpile_block(const BlockNode *p_block);
	// void transpile_function(const FunctionNode *p_function);
	
	void clear();
	
public:
	virtual String get_name() const;
	virtual Variant transpile(const Ref<GDScript> &p_script);
	
	GDScriptTranspilerCpp();
	~GDScriptTranspilerCpp();
};

#endif // GDSCRIPT_TRANSPILER_CPP_H
