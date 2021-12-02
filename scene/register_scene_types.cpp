#include "register_scene_types.h"

#include "2d/debug_2d.h"
#include "audio/register_audio_types.h"
#include "physics/register_physics_types.h"

#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"

#include "core/goost_engine.h"

#include "goost/classes_enabled.gen.h"

namespace goost {

static Debug2D *_debug_2d = nullptr;
static bool _debug_2d_added = false;

static void _debug_2d_add_to_scene_tree() {
	// This is quite hacky, but couldn't find another way.
	// `SceneTree` is not accessible during `register_module_types()` in Godot.
	// This is meant to be to replicate autoload behavior.
	if (_debug_2d_added) {
		return;
	}
	auto tree = SceneTree::get_singleton();
	if (!tree) {
		return;
	}
	Debug2D::get_singleton()->set_name("Debug2D");
	tree->get_root()->add_child(Debug2D::get_singleton());
	_debug_2d_added = true;
}

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

	_debug_2d = memnew(Debug2D);
	ClassDB::register_class<Debug2D>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("Debug2D", Debug2D::get_singleton()));
	SceneTree::add_idle_callback(&_debug_2d_add_to_scene_tree);

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
	// There's no need to free `Debug2D` instance manually because it's added to
	// the `SceneTree`, but lets play safe and prevent memory leak in any case.
	if (_debug_2d && ObjectDB::instance_validate(_debug_2d)) {
		memdelete(_debug_2d);
	}
#ifdef GOOST_AUDIO_ENABLED
	unregister_audio_types();
#endif
#ifdef GOOST_PHYSICS_ENABLED
	unregister_physics_types();
#endif
}

} // namespace goost
