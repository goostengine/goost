#include "register_script_types.h"

#include "goost/register_types.h"
#include "goost/classes_enabled.gen.h"

#include "mixin_script/mixin_script.h"
#include "mixin_script/editor/mixin_script_editor.h"
#include "mixin_script/editor/mixin_script_editor_plugin.h"

#include "core/script_language.h"
#include "editor/editor_node.h"
#include "editor/plugins/script_editor_plugin.h"

namespace goost {

static MixinScriptLanguage *script_mixin_script = NULL;

#ifdef GOOST_MixinScript
static ScriptEditorBase *create_editor(const RES &p_resource) {
	if (Object::cast_to<MixinScript>(*p_resource)) {
		return memnew(MixinScriptEditor);
	}
	return nullptr;
}

static void mixin_script_register_editor_callback() {
	ScriptEditor::register_create_script_editor_function(create_editor);
}
#endif

void register_script_types() {
#ifdef GOOST_MixinScript
	script_mixin_script = memnew(MixinScriptLanguage);
	ScriptServer::register_language(script_mixin_script);
	ClassDB::register_class<MixinScript>();
	ClassDB::register_class<Mixin>();

#ifdef TOOLS_ENABLED
	EditorNode::add_plugin_init_callback(mixin_script_register_editor_callback);
	EditorPlugins::add_by_type<MixinScriptEditorPlugin>();
#endif
#endif // GOOST_MixinScript
}

void unregister_script_types() {
#ifdef GOOST_MixinScript
	if (script_mixin_script) {
		ScriptServer::unregister_language(script_mixin_script);
		memdelete(script_mixin_script);
	}
#endif
}

} // namespace goost
