#include "core/object.h"

#include "register_scene_types.h"
#include "2d/physics/shape_cast_2d.h"

namespace goost {

void register_scene_types() {
	ClassDB::register_class<ShapeCast2D>();
}

void unregister_scene_types() {
	// Nothing to do.
}

} // namespace goost
