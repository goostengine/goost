#include "register_physics_types.h"
#include "goost/register_types.h"
#include "goost/classes_enabled.gen.h"

#include "2d/shape_cast_2d.h"

namespace goost {

void register_physics_types() {
	GOOST_REGISTER_ShapeCast2D;
}

void unregister_physics_types() {
	// Nothing to do.
}

} // namespace goost
