#include "register_types.h"

#include "core/engine.h"

#include "goost.h"
#include "core/register_core_types.h"
#include "scene/register_scene_types.h"
#include "editor/register_editor_types.h"

#ifdef GOOST_CORE_ENABLED
static GoostEngine *_goost = nullptr;
#endif

void register_goost_types() {
#ifdef GOOST_CORE_ENABLED
	_goost = memnew(GoostEngine);
	ClassDB::register_class<GoostEngine>();
	Engine::get_singleton()->add_singleton(
			Engine::Singleton("GoostEngine", GoostEngine::get_singleton()));
	goost::register_core_types();
#endif
#ifdef GOOST_SCENE_ENABLED
	goost::register_scene_types();
#endif
#if defined(TOOLS_ENABLED) && defined(GOOST_EDITOR_ENABLED)
	goost::register_editor_types();
#endif
}

void unregister_goost_types() {
#ifdef GOOST_CORE_ENABLED
	if (_goost) { 
		memdelete(_goost);
	}
	goost::unregister_core_types();
#endif
#ifdef GOOST_SCENE_ENABLED
	goost::unregister_scene_types();
#endif
}
