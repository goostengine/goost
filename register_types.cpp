#include "register_types.h"

#include "core/register_core_types.h"
#include "scene/register_scene_types.h"

void register_goost_types() {
#ifdef GOOST_CORE_ENABLED
	goost::register_core_types();
#endif
#ifdef GOOST_SCENE_ENABLED
	goost::register_scene_types();
#endif
}

void unregister_goost_types() {
#ifdef GOOST_CORE_ENABLED
	goost::unregister_core_types();
#endif
#ifdef GOOST_SCENE_ENABLED
	goost::unregister_scene_types();
#endif
}
