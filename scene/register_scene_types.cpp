#include "core/object.h"

#include "register_scene_types.h"
#include "physics/register_physics_types.h"

namespace goost {

void register_scene_types() {
#ifdef GOOST_PHYSICS_ENABLED
	register_physics_types();
#endif
}

void unregister_scene_types() {
#ifdef GOOST_PHYSICS_ENABLED
	unregister_physics_types();
#endif
}

} // namespace goost
