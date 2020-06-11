#include "register_types.h"
#include "core/engine.h"

#include "bind/gdscript_transpiler_bind.h"
#include "gdscript_transpiler.h"
#include "gdscript_transpiler_cxx.h"

static _GDScriptTranspiler *_gdscript_transpiler = nullptr;
static GDScriptTranspilerCpp *gd_transpiler_cpp = nullptr;

void register_gdscript_transpiler_types() {
#ifdef TOOLS_ENABLED
	gd_transpiler_cpp = memnew(GDScriptTranspilerCpp);
	GDScriptTranspiler::add_transpiler(gd_transpiler_cpp);

	_gdscript_transpiler = memnew(_GDScriptTranspiler);
	ClassDB::register_class<_GDScriptTranspiler>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("GDScriptTranspiler", _GDScriptTranspiler::get_singleton()));
#endif
}

void unregister_gdscript_transpiler_types() {
#ifdef TOOLS_ENABLED
	memdelete(_gdscript_transpiler);
	GDScriptTranspiler::cleanup();
#endif
}
