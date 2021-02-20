#include "register_physics_types.h"
#include "goost/register_types.h"
#include "goost/classes_enabled.gen.h"

namespace goost {

void register_physics_types() {
	goost::register_class<ShapeCast2D>();
}

void unregister_physics_types() {
	// Nothing to do.
}

} // namespace goost
