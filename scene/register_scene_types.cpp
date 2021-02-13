#include "register_scene_types.h"
#include "goost/register_types.h"
#include "goost/classes_enabled.gen.h"

#include "physics/register_physics_types.h"

#include "2d/editor/poly_node_2d_editor_plugin.h"
#include "2d/editor/visual_shape_2d_editor_plugin.h"
#include "2d/poly_collision_shape_2d.h"
#include "2d/poly_generators_2d.h"
#include "2d/poly_shape_2d.h"
#include "2d/visual_shape_2d.h"
#include "resources/gradient_texture_2d.h"

namespace goost {

void register_scene_types() {
#if defined(GOOST_CORE_ENABLED) && defined(GOOST_PolyNode2D)
	GOOST_REGISTER_PolyCircle2D;
	GOOST_REGISTER_PolyRectangle2D;
	GOOST_REGISTER_PolyShape2D;
	GOOST_REGISTER_PolyCollisionShape2D;
#endif
	GOOST_REGISTER_VisualShape2D;
	GOOST_REGISTER_GradientTexture2D;

#if defined(TOOLS_ENABLED) && defined(GOOST_EDITOR_ENABLED)
#if defined(GOOST_CORE_ENABLED) && defined(GOOST_PolyNode2D)
	EditorPlugins::add_by_type<PolyNode2DEditorPlugin>();
#endif
#if defined(GOOST_VisualShape2D)
	EditorPlugins::add_by_type<VisualShape2DEditorPlugin>();
#endif
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
