#include "register_core_types.h"

#include "core/engine.h"
#include "scene/main/scene_tree.h"

#include "string_names.h"

#include "image/register_image_types.h"
#include "math/register_math_types.h"
#include "script/register_script_types.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_node.h"
#include "editor/editor_resource_preview.h"
#include "types/editor/data_container_preview.h"
#endif

#include "goost/classes_enabled.gen.h"

namespace goost {

static GoostEngine *_goost = nullptr;

#if defined(TOOLS_ENABLED) && defined(GOOST_DataContainer)
static void _data_container_preview_init();
#endif

void register_core_types() {
	StringNames::create();

	ClassDB::register_class<CommandLineOption>();
	ClassDB::register_class<CommandLineHelpFormat>();
	ClassDB::register_class<CommandLineParser>();

#ifdef GOOST_GoostEngine
	_goost = memnew(GoostEngine);
	ClassDB::register_class<GoostEngine>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("GoostEngine", GoostEngine::get_singleton()));
	SceneTree::add_idle_callback(&GoostEngine::flush_calls);
#endif
	ClassDB::register_class<InvokeState>();

	ClassDB::register_class<ListNode>();
	ClassDB::register_class<LinkedList>();

	ClassDB::register_class<Graph>();
	ClassDB::register_class<GraphVertex>();
	ClassDB::register_class<GraphEdge>();
	ClassDB::register_class<GraphIterator>();

	ClassDB::register_class<Map2D>();
	ClassDB::register_class<DataContainer>();

#ifndef DISABLE_DEPRECATED
	ClassDB::add_compatibility_class("VariantMap", "Map2D");
	ClassDB::add_compatibility_class("VariantResource", "DataContainer");
#endif
#if defined(TOOLS_ENABLED) && defined(GOOST_DataContainer)
	EditorNode::add_init_callback(_data_container_preview_init);
#endif

#ifdef GOOST_IMAGE_ENABLED
	register_image_types();
#endif
#ifdef GOOST_MATH_ENABLED
	register_math_types();
#endif
#ifdef GOOST_SCRIPT_ENABLED
	register_script_types();
#endif
}

void unregister_core_types() {
	if (_goost) {
		memdelete(_goost);
	}
#ifdef GOOST_IMAGE_ENABLED
	unregister_image_types();
#endif
#ifdef GOOST_MATH_ENABLED
	unregister_math_types();
#endif
#ifdef GOOST_SCRIPT_ENABLED
	unregister_script_types();
#endif
	StringNames::free();
}

#if defined(TOOLS_ENABLED) && defined(GOOST_DataContainer)
void _data_container_preview_init() {
	Ref<DataContainerPreviewGenerator> data_container_preview;
	data_container_preview.instance();
	EditorResourcePreview::get_singleton()->add_preview_generator(data_container_preview);
}
#endif

} // namespace goost
