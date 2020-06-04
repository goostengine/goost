#ifndef GDSCRIPT_TRANSPILER_BIND_H
#define GDSCRIPT_TRANSPILER_BIND_H

#include "core/object.h"
#include "gdscript_transpiler.h"

class _GDScriptTranspiler : public Object {
	GDCLASS(_GDScriptTranspiler, Object);
	
private:
	static _GDScriptTranspiler *singleton;

protected:
	static void _bind_methods();
	
public:
	static _GDScriptTranspiler *get_singleton() { return singleton; }
	
public:
	Variant transpile(const Ref<GDScript> &p_script, const String &p_language = "C++");
	Array get_supported_languages() const;
	
	_GDScriptTranspiler();
};

#endif // GDSCRIPT_TRANSPILER_H
