#include "gdscript_transpiler_bind.h"
#include "gdscript_transpiler.h"

_GDScriptTranspiler *_GDScriptTranspiler::singleton = nullptr;

Variant _GDScriptTranspiler::transpile(const Ref<GDScript> &p_script, const String &p_language) {
    return GDScriptTranspiler::transpile(p_script, p_language);
}

Array _GDScriptTranspiler::get_supported_languages() const {
    List<String> languages;
    GDScriptTranspiler::get_supported_languages(&languages);
    
    Array ret;
    for (List<String>::Element *E = languages.front(); E; E = E->next()) {
        ret.push_back(E->get());
    }
    return ret;
}

void _GDScriptTranspiler::_bind_methods() {
    ClassDB::bind_method(D_METHOD("transpile", "gdscript", "to_language"), &_GDScriptTranspiler::transpile, DEFVAL("C++"));
    ClassDB::bind_method(D_METHOD("get_supported_languages"), &_GDScriptTranspiler::get_supported_languages);
}

_GDScriptTranspiler::_GDScriptTranspiler() {
	singleton = this;
}
