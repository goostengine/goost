#include "core/object.h"

#include "2d/shape_cast_2d.h"
#include "register_physics_types.h"

namespace goost {

void register_physics_types() {
	ClassDB::register_class<ShapeCast2D>();
}

void unregister_physics_types() {
	// Nothing to do.
}

} // namespace goost
