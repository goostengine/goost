#ifndef GDSCRIPT_TRANSPILER_UTILS_H
#define GDSCRIPT_TRANSPILER_UTILS_H

#include "core/string_builder.h"
#include "core/ustring.h"
#include "core/variant.h"

namespace GDScriptTranspilerUtils {

String snake_to_pascal_case(const String &p_identifier, bool p_input_is_upper = false);
String snake_to_camel_case(const String &p_identifier, bool p_input_is_upper = false);
String filepath_to_pascal_case(const String &p_identifier, bool p_input_is_upper = false);

class CodeBuilder {
	StringBuilder code;
	unsigned int indent_level;
	String indent_sequence;
	bool newline;

public:
	Dictionary map;

public:
	String get_indent_string() const;
	void set_indent_sequence(const String &p_indent_sequence);
	String get_indent_sequence() const { return indent_sequence; }
	void indent(unsigned int p_steps = 1) { indent_level += p_steps; }
	void dedent(unsigned int p_steps = 1) { indent_level -= p_steps; }

	void set_newline_enabled(bool p_enabled) { newline = p_enabled; }

	void set_substitute_map(const Dictionary &p_map) { map = p_map; }
	Dictionary get_substitute_map() { return map; }

	void operator+=(const String &p_string);
	String get_code() const;

	CodeBuilder();
};

} // namespace GDScriptTranspilerUtils

#endif // GDSCRIPT_TRANSPILER_UTILS_H
