#include "register_scene_types.h"

#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"

#include "editor/editor_node.h"

#include "audio/register_audio_types.h"
#include "physics/register_physics_types.h"

#include "2d/debug_2d.h"
#include "2d/spawner_2d.h"

#include "goost/classes_enabled.gen.h"

namespace goost {

#if defined(GOOST_GEOMETRY_ENABLED) && defined(GOOST_Debug2D)

static Debug2D *_debug_2d = nullptr;
static bool _debug_2d_added = false;

static void _debug_2d_add_to_scene_tree() {
	// This is quite hacky, but couldn't find another way.
	// `SceneTree` is not accessible during `register_module_types()` in Godot.
	// This is meant to be to replicate autoload behavior.
	if (_debug_2d_added) {
		return;
	}
	Debug2D::get_singleton()->set_name("Debug2D");

	if (Engine::get_singleton()->is_editor_hint()) {
#ifdef TOOLS_ENABLED
		EditorNode::get_singleton()->get_scene_root()->add_child(Debug2D::get_singleton());
#endif
	} else {
		if (!SceneTree::get_singleton()) {
			return;
		}
		SceneTree::get_singleton()->get_root()->add_child(Debug2D::get_singleton());
	}
	Debug2D::get_singleton()->set_enabled(GLOBAL_GET("debug/draw/2d/enabled"));

	_debug_2d_added = true;
}
#endif

void register_scene_types() {
#if defined(GOOST_GEOMETRY_ENABLED) && defined(GOOST_PolyNode2D)
	ClassDB::register_class<PolyCircle2D>();
	ClassDB::register_class<PolyCapsule2D>();
	ClassDB::register_class<PolyRectangle2D>();
	ClassDB::register_class<PolyPath2D>();
	ClassDB::register_class<PolyShape2D>();
#endif
	ClassDB::register_class<Stopwatch>();
	ClassDB::register_class<Spawner2D>();
	ClassDB::register_class<VisualShape2D>();
	ClassDB::register_class<GradientTexture2D>();
	ClassDB::register_class<LightTexture>();

#if defined(GOOST_GEOMETRY_ENABLED) && defined(GOOST_Debug2D)
	// Define project settings before registering classes.
	GLOBAL_DEF("debug/draw/2d/enabled", true);
	GLOBAL_DEF("debug/draw/2d/color", Color(0.0, 0.6, 0.7, 1));
	GLOBAL_DEF("debug/draw/2d/filled", true);
	GLOBAL_DEF("debug/draw/2d/line_width", 1.0);
	ProjectSettings::get_singleton()->set_custom_property_info("debug/draw/2d/line_width", PropertyInfo(Variant::REAL, "debug/draw/2d/line_width", PROPERTY_HINT_RANGE, "0.1,5.0,0.1,or_greater"));
	GLOBAL_DEF("debug/draw/2d/antialiased", false);

	ClassDB::register_virtual_class<Debug2D>();
	ClassDB::register_virtual_class<DebugCapture>();

	_debug_2d = memnew(Debug2D);
	Engine::get_singleton()->add_singleton(Engine::Singleton("Debug2D", Debug2D::get_singleton()));
	SceneTree::add_idle_callback(&_debug_2d_add_to_scene_tree);

#endif
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
#if defined(GOOST_GEOMETRY_ENABLED) && defined(GOOST_Debug2D)
	// There's no need to free `Debug2D` instance manually because it's added to
	// the `SceneTree`, but lets play safe and prevent memory leak in any case.
	if (_debug_2d && ObjectDB::instance_validate(_debug_2d)) {
		memdelete(_debug_2d);
	}
#endif
#ifdef GOOST_AUDIO_ENABLED
	unregister_audio_types();
#endif
#ifdef GOOST_PHYSICS_ENABLED
	unregister_physics_types();
#endif
}

} // namespace goost
