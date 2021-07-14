#include "register_physics_types.h"

#include "goost/classes_enabled.gen.h"
#include "goost/goost.h"

namespace goost {

void register_physics_types() {
	goost::register_class<ShapeCast2D>();

#if defined(GOOST_GEOMETRY_ENABLED) && defined(GOOST_PolyNode2D) && defined(GOOST_PolyShape2D)
	goost::register_class<PolyCollisionShape2D>();
#endif
}

void unregister_physics_types() {
	// Nothing to do.
}

} // namespace goost
