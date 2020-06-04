#include "gdscript_transpiler.h"
#include "core/string_builder.h"

Vector<GDScriptTranspilerLanguage *> GDScriptTranspiler::transpiler;

void GDScriptTranspiler::add_transpiler(GDScriptTranspilerLanguage *p_transpiler) {
	transpiler.push_back(p_transpiler);
}

void GDScriptTranspiler::remove_transpiler(GDScriptTranspilerLanguage *p_transpiler) {
	transpiler.erase(p_transpiler);
}

void GDScriptTranspiler::cleanup() {
	while (transpiler.size()) {
		remove_transpiler(transpiler[0]);
	}
}

Variant GDScriptTranspiler::transpile(const Ref<GDScript> &p_script, const String &p_language) {
    for (int i = 0; i < transpiler.size(); ++i) {
        if (transpiler[i]->handles(p_language)) {
            return transpiler[i]->transpile(p_script);
        }
    }
    ERR_FAIL_V_MSG(Variant(), "Unsupported language.");
}

void GDScriptTranspiler::get_supported_languages(List<String> *p_languages) {
    for (int i = 0; i < transpiler.size(); ++i) {
        p_languages->push_back(transpiler[i]->get_name());
    }
}

bool GDScriptTranspilerLanguage::handles(const String &p_language) const {
    return p_language.to_lower() == get_name().to_lower();
}

GDScriptTranspilerLanguage *GDScriptTranspiler::recognize(const String &p_language) {
    for (int i = 0; i < transpiler.size(); ++i) {
        if (transpiler[i]->handles(p_language)) {
            return transpiler[i];
        }
    }
    ERR_FAIL_V_MSG(nullptr, "Unrecognized language.");
}
