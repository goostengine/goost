#include "register_scene_types.h"
#include "goost/register_types.h"
#include "physics/register_physics_types.h"
#include "goost/classes_enabled.gen.h"

namespace goost {

void register_scene_types() {
#if defined(GOOST_CORE_ENABLED) && defined(GOOST_PolyNode2D)
	goost::register_class<PolyCircle2D>();
	goost::register_class<PolyRectangle2D>();
	goost::register_class<PolyShape2D>();
	goost::register_class<PolyCollisionShape2D>();
#endif
	goost::register_class<VisualShape2D>();
	goost::register_class<GradientTexture2D>();

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
