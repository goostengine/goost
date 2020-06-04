#include "register_types.h"

#include "core/register_core_types.h"
#include "scene/register_scene_types.h"

void register_goost_types() {
	goost::register_core_types();
	goost::register_scene_types();
}

void unregister_goost_types() {
	goost::unregister_core_types();
	goost::unregister_scene_types();
}
