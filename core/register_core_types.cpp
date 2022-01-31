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
#include "types/editor/variant_resource_preview.h"
#endif

#include "goost/classes_enabled.gen.h"

namespace goost {

static GoostEngine *_goost = nullptr;

#if defined(TOOLS_ENABLED) && defined(GOOST_VariantResource)
static void _variant_resource_preview_init();
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

	ClassDB::register_class<VariantMap>();
	ClassDB::register_class<VariantResource>();

#if defined(TOOLS_ENABLED) && defined(GOOST_VariantResource)
	EditorNode::add_init_callback(_variant_resource_preview_init);
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

#if defined(TOOLS_ENABLED) && defined(GOOST_VariantResource)
void _variant_resource_preview_init() {
	Ref<VariantResourcePreviewGenerator> variant_resource_preview;
	variant_resource_preview.instance();
	EditorResourcePreview::get_singleton()->add_preview_generator(variant_resource_preview);
}
#endif

} // namespace goost
