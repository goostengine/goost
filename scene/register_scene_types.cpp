#include "core/object.h"

#include "physics/register_physics_types.h"
#include "register_scene_types.h"

#include "2d/editor/visual_shape_2d_editor_plugin.h"
#include "2d/editor/poly_node_2d_editor_plugin.h"
#include "2d/poly_shapes_2d.h"
#include "2d/visual_shape_2d.h"

namespace goost {

void register_scene_types() {
	ClassDB::register_class<PolyCircle2D>();
	ClassDB::register_class<VisualShape2D>();
#if defined(TOOLS_ENABLED) && defined(GOOST_EDITOR_ENABLED)
	EditorPlugins::add_by_type<PolyNode2DEditorPlugin>();
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
