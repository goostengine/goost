#include "register_script_types.h"

#include "goost/register_types.h"
#include "goost/classes_enabled.gen.h"

#include "mixin_script/mixin_script.h"

#include "core/script_language.h"

namespace goost {

static MixinScriptLanguage *script_mixin_script = NULL;

void register_script_types() {
	script_mixin_script = memnew(MixinScriptLanguage);
	ScriptServer::register_language(script_mixin_script);
	ClassDB::register_class<MixinScript>();
	ClassDB::register_class<Mixin>();
}

void unregister_script_types() {
	if (script_mixin_script) {
		ScriptServer::unregister_language(script_mixin_script);
		memdelete(script_mixin_script);
	}
}

} // namespace goost
