#include "register_scene_types.h"

#include "physics/register_physics_types.h"
#include "audio/register_audio_types.h"

#include "goost/classes_enabled.gen.h"

namespace goost {

void register_scene_types() {
#if defined(GOOST_GEOMETRY_ENABLED) && defined(GOOST_PolyNode2D)
	ClassDB::register_class<PolyCircle2D>();
	ClassDB::register_class<PolyCapsule2D>();
	ClassDB::register_class<PolyRectangle2D>();
	ClassDB::register_class<PolyPath2D>();
	ClassDB::register_class<PolyShape2D>();
#endif
	ClassDB::register_class<Stopwatch>();
	ClassDB::register_class<VisualShape2D>();
	ClassDB::register_class<GradientTexture2D>();
	ClassDB::register_class<LightTexture>();

#ifdef GOOST_AUDIO_ENABLED
	register_audio_types();
#endif

#ifdef GOOST_GUI_ENABLED
	ClassDB::register_class<GridRect>();
#endif

#if defined(TOOLS_ENABLED) && defined(GOOST_EDITOR_ENABLED)
#if defined(GOOST_GEOMETRY_ENABLED) && defined(GOOST_PolyNode2D)
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
#ifdef GOOST_AUDIO_ENABLED
	unregister_audio_types();
#endif
#ifdef GOOST_PHYSICS_ENABLED
	unregister_physics_types();
#endif
}

} // namespace goost
