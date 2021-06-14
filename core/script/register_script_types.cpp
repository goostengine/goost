#include "register_script_types.h"

#include "goost/register_types.h"
#include "goost/classes_enabled.gen.h"

#include "multiscript/multiscript.h"

#include "core/script_language.h"

namespace goost {

static MultiScriptLanguage *script_multi_script = NULL;

void register_script_types() {
	script_multi_script = memnew(MultiScriptLanguage);
	ScriptServer::register_language(script_multi_script);
	ClassDB::register_class<MultiScript>();
	ClassDB::register_class<MultiScriptOwner>();
}

void unregister_script_types() {
	if (script_multi_script) {
		ScriptServer::unregister_language(script_multi_script);
		memdelete(script_multi_script);
	}
}

} // namespace goost
