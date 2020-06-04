#ifndef GDSCRIPT_TRANSPILER_H
#define GDSCRIPT_TRANSPILER_H

#include "core/reference.h"

#include "modules/gdscript/gdscript.h"
#include "modules/gdscript/gdscript_parser.h"

#include "gdscript_transpiler_utils.h"


class GDScriptTranspilerLanguage;

class GDScriptTranspiler {
	static Vector<GDScriptTranspilerLanguage *> transpiler;

public:
	static Variant transpile(const Ref<GDScript> &p_script, const String &p_language = "C++");
	static void get_supported_languages(List<String> *p_languages);
	static GDScriptTranspilerLanguage *recognize(const String &p_language);

	static void add_transpiler(GDScriptTranspilerLanguage *p_transpiler);
	static void remove_transpiler(GDScriptTranspilerLanguage *p_transpiler);

	static const Vector<GDScriptTranspilerLanguage *> &get_transpilers() { return transpiler; }

	static void cleanup();
};

class GDScriptTranspilerLanguage {
protected:
	Map<String, GDScriptTranspilerUtils::CodeBuilder> code;
	String script_path;
public:
	virtual String get_name() const = 0;
	virtual bool handles(const String &p_language) const;
	virtual Variant transpile(const Ref<GDScript> &p_script) = 0;
};

#endif // GDSCRIPT_TRANSPILER_H
