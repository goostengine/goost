#include "core/object.h"

#include "physics/register_physics_types.h"
#include "register_scene_types.h"

#include "2d/editor/poly_node_2d_editor_plugin.h"
#include "2d/editor/visual_shape_2d_editor_plugin.h"
#include "2d/poly_collision_shape_2d.h"
#include "2d/poly_shape_2d.h"
#include "2d/poly_generators_2d.h"
#include "2d/visual_shape_2d.h"

namespace goost {

void register_scene_types() {
#ifdef GOOST_CORE_ENABLED
	// Depend on `PolyNode2D`.
	ClassDB::register_class<PolyCircle2D>();
	ClassDB::register_class<PolyRectangle2D>();
	ClassDB::register_class<PolyShape2D>();
	ClassDB::register_class<PolyCollisionShape2D>();
#endif
	ClassDB::register_class<VisualShape2D>();

#if defined(TOOLS_ENABLED) && defined(GOOST_EDITOR_ENABLED)
#ifdef GOOST_CORE_ENABLED
	EditorPlugins::add_by_type<PolyNode2DEditorPlugin>(); // Depends on `PolyNode2D`.
#endif
	EditorPlugins::add_by_type<VisualShape2DEditorPlugin>();
#endif
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
