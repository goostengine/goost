#include "mixin_script.h"

bool MixinScriptInstance::set(const StringName &p_name, const Variant &p_value) {
	if (main_instance) {
		bool found = main_instance->set(p_name, p_value);
		if (found) {
			return true;
		}
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i]) {
			bool found = sarr[i]->set(p_name, p_value);
			if (found) {
				return true;
			}
		}
	}
	if (String(p_name).begins_with("script_")) {
		bool valid;
		owner->set(p_name, p_value, &valid);
		return valid;
	}
	return false;
}

bool MixinScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
	if (main_instance) {
		bool found = main_instance->get(p_name, r_ret);
		if (found) {
			return true;
		}
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i]) {
			bool found = sarr[i]->get(p_name, r_ret);
			if (found) {
				return true;
			}
		}
	}
	if (String(p_name).begins_with("script_")) {
		bool valid;
		r_ret = owner->get(p_name, &valid);
		return valid;
	}
	return false;
}

void MixinScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
	if (main_instance) {
		main_instance->get_property_list(p_properties);
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	Set<String> existing;

	for (int i = 0; i < instances.size(); i++) {
		if (!sarr[i]) {
			continue;
		}
		List<PropertyInfo> pl;
		sarr[i]->get_property_list(&pl);

		for (List<PropertyInfo>::Element *E = pl.front(); E; E = E->next()) {
			if (existing.has(E->get().name)) {
				continue;
			}
			p_properties->push_back(E->get());
			existing.insert(E->get().name);
		}
	}
	p_properties->push_back(PropertyInfo(Variant::NIL, "Scripts", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_CATEGORY));

	for (int i = 0; i < owner->scripts.size(); i++) {
		p_properties->push_back(PropertyInfo(Variant::OBJECT, "script_" + String::chr('a' + i), PROPERTY_HINT_RESOURCE_TYPE, "Script", PROPERTY_USAGE_EDITOR));
	}
	if (owner->scripts.size() < 25) {
		p_properties->push_back(PropertyInfo(Variant::OBJECT, "script_" + String::chr('a' + (owner->scripts.size())), PROPERTY_HINT_RESOURCE_TYPE, "Script", PROPERTY_USAGE_EDITOR));
	}
}

void MixinScriptInstance::get_method_list(List<MethodInfo> *p_list) const {
	if (main_instance) {
		main_instance->get_method_list(p_list);
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	Set<StringName> existing;

	for (int i = 0; i < instances.size(); i++) {
		if (!sarr[i]) {
			continue;
		}
		List<MethodInfo> ml;
		sarr[i]->get_method_list(&ml);

		for (List<MethodInfo>::Element *E = ml.front(); E; E = E->next()) {
			if (existing.has(E->get().name)) {
				continue;
			}
			p_list->push_back(E->get());
			existing.insert(E->get().name);
		}
	}
}

bool MixinScriptInstance::has_method(const StringName &p_method) const {
	if (main_instance && main_instance->has_method(p_method)) {
		return true;
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i] && sarr[i]->has_method(p_method)) {
			return true;
		}
	}
	return false;
}

Variant MixinScriptInstance::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	// Main script.
	if (main_instance) {
		Variant r = main_instance->call(p_method, p_args, p_argcount, r_error);
		if (r_error.error == Variant::CallError::CALL_OK)
			return r;
		else if (r_error.error != Variant::CallError::CALL_ERROR_INVALID_METHOD)
			return r;
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (!sarr[i]) {
			continue;
		}
		Variant r = sarr[i]->call(p_method, p_args, p_argcount, r_error);
		if (r_error.error == Variant::CallError::CALL_OK)
			return r;
		else if (r_error.error != Variant::CallError::CALL_ERROR_INVALID_METHOD)
			return r;
	}
	r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;

	return Variant();
}

void MixinScriptInstance::call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount) {
	if (main_instance) {
		main_instance->call_multilevel(p_method, p_args, p_argcount);
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i]) {
			sarr[i]->call_multilevel(p_method, p_args, p_argcount);
		}
	}
}

void MixinScriptInstance::notification(int p_notification) {
	if (main_instance) {
		main_instance->notification(p_notification);
	}
	// Mixins.
	for (int i = 0; i < instances.size(); i++) {
		ScriptInstance *instance = instances[i];
		if (instance) {
			instance->notification(p_notification);
		}
	}
}

ScriptLanguage *MixinScriptInstance::get_language() {
	return MixinScriptLanguage::get_singleton();
}

MixinScriptInstance::~MixinScriptInstance() {
	owner->remove_instance(object);
}

Variant::Type MixinScriptInstance::get_property_type(const StringName &p_name, bool *r_is_valid) const {
	bool valid = false;
	Variant::Type type;

	if (main_instance) {
		type = main_instance->get_property_type(p_name, &valid);
		if (valid) {
			*r_is_valid = valid;
			return type;
		}
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (!sarr[i]) {
			continue;
		}
		type = sarr[i]->get_property_type(p_name, &valid);
		if (valid) {
			*r_is_valid = valid;
			return type;
		}
	}
	*r_is_valid = false;

	return Variant::NIL;
}

MultiplayerAPI::RPCMode MixinScriptInstance::get_rpc_mode(const StringName &p_method) const {
	if (main_instance && main_instance->has_method(p_method)) {
		return main_instance->get_rpc_mode(p_method);
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i] && sarr[i]->has_method(p_method)) {
			return sarr[i]->get_rpc_mode(p_method);
		}
	}
	return MultiplayerAPI::RPC_MODE_DISABLED;
}

MultiplayerAPI::RPCMode MixinScriptInstance::get_rset_mode(const StringName &p_variable) const {
	if (main_instance) {
		List<PropertyInfo> properties;
		main_instance->get_property_list(&properties);

		for (List<PropertyInfo>::Element *P = properties.front(); P; P = P->next()) {
			if (P->get().name == p_variable) {
				return main_instance->get_rset_mode(p_variable);
			}
		}
	}
	// Mixins.
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (!sarr[i]) {
			continue;
		}
		List<PropertyInfo> properties;
		sarr[i]->get_property_list(&properties);

		for (List<PropertyInfo>::Element *P = properties.front(); P; P = P->next()) {
			if (P->get().name == p_variable) {
				return sarr[i]->get_rset_mode(p_variable);
			}
		}
	}
	return MultiplayerAPI::RPC_MODE_DISABLED;
}

///////////////////

void MixinScript::set_main_script(const Ref<Script> &p_script) {
	Ref<MixinScript> ms = p_script;
	if (ms == Ref<MixinScript>(this)) {
		ERR_FAIL_MSG("Cannot set a main script pointing to itself");
		return;
	}
	main_script = p_script;
	emit_changed();
}

bool MixinScript::is_tool() const {
	if (main_script.is_valid()) {
		if (main_script->is_tool()) {
			return true;
		}
	}
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->is_tool()) {
			return true;
		}
	}
	return false;
}

bool MixinScript::is_valid() const {
	if (main_script.is_valid()) {
		if (main_script->is_valid()) {
			return true;
		}
	}
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->is_valid()) {
			return true;
		}
	}
	return false;
}

bool MixinScript::_set(const StringName &p_name, const Variant &p_value) {
	_THREAD_SAFE_METHOD_

	String s = String(p_name);
	if (s.begins_with("script_")) {
		int idx = s[7];
		if (idx == 0) {
			return false;
		}
		idx -= 'a';

		ERR_FAIL_COND_V(idx < 0, false);

		Ref<Script> s = p_value;

		if (idx < scripts.size()) {
			if (s.is_null()) {
				remove_script(idx);
			} else {
				set_script_at_index(idx, s);
			}
		} else if (idx == scripts.size()) {
			if (s.is_null()) {
				return false;
			}
			add_script(s);
		} else {
			return false;
		}
		return true;
	}
	return false;
}

bool MixinScript::_get(const StringName &p_name, Variant &r_ret) const {
	_THREAD_SAFE_METHOD_

	String s = String(p_name);
	if (s.begins_with("script_")) {
		int idx = s[7];
		if (idx == 0) {
			return false;
		}
		idx -= 'a';

		ERR_FAIL_COND_V(idx < 0, false);

		if (idx < scripts.size()) {
			r_ret = get_script_at_index(idx);
			return true;
		} else if (idx == scripts.size()) {
			r_ret = Ref<Script>();
			return true;
		}
	}
	return false;
}

void MixinScript::_get_property_list(List<PropertyInfo> *p_list) const {
	_THREAD_SAFE_METHOD_

	for (int i = 0; i < scripts.size(); i++) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "script_" + String::chr('a' + i), PROPERTY_HINT_RESOURCE_TYPE, "Script"));
	}
	if (scripts.size() < 25) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "script_" + String::chr('a' + (scripts.size())), PROPERTY_HINT_RESOURCE_TYPE, "Script"));
	}
}

void MixinScript::set_script_at_index(int p_idx, const Ref<Script> &p_script) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_INDEX(p_idx, scripts.size());
	ERR_FAIL_COND(p_script.is_null());

	scripts.set(p_idx, p_script);
	Ref<Script> s = p_script;

	for (Map<Object *, MixinScriptInstance *>::Element *E = instances.front(); E; E = E->next()) {
		MixinScriptInstance *msi = E->get();
		// Looks like there's no need to explicitly free a previous
		// script instance in `msi->instances`, because `s->instance_create`
		// will free it by itself, otherwise this leads to random crashes.
		if (p_script->can_instance()) {
			msi->instances.set(p_idx, s->instance_create(script_instances[p_idx]));
		}
	}
	emit_changed();
}

Ref<Script> MixinScript::get_script_at_index(int p_idx) const {
	_THREAD_SAFE_METHOD_
	ERR_FAIL_INDEX_V(p_idx, scripts.size(), Ref<Script>());

	return scripts[p_idx];
}

void MixinScript::add_script(const Ref<Script> &p_script) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_COND(p_script.is_null());
	Mixin *script_owner = memnew(Mixin);
	script_instances.push_back(script_owner);
	scripts.push_back(p_script);
	Ref<Script> s = p_script;

	for (Map<Object *, MixinScriptInstance *>::Element *E = instances.front(); E; E = E->next()) {
		MixinScriptInstance *msi = E->get();

		if (p_script->can_instance()) {
			script_owner->real_owner = msi->object;
			msi->instances.push_back(s->instance_create(script_owner));
		} else {
			msi->instances.push_back(nullptr);
		}
		msi->object->_change_notify();
	}
	_change_notify();
	emit_changed();
}

void MixinScript::remove_script(int p_idx) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_INDEX(p_idx, scripts.size());

	scripts.remove(p_idx);
	Mixin *m = script_instances.get(p_idx);
	if (m) {
		memdelete(m);
	}
	script_instances.remove(p_idx);

	for (Map<Object *, MixinScriptInstance *>::Element *E = instances.front(); E; E = E->next()) {
		MixinScriptInstance *msi = E->get();
		msi->instances.remove(p_idx);
		msi->object->_change_notify();
	}
	emit_changed();
}

void MixinScript::remove_instance(Object *p_object) {
	_THREAD_SAFE_METHOD_
	instances.erase(p_object);
}

StringName MixinScript::get_instance_base_type() const {
	return base_class_name;
}

ScriptInstance *MixinScript::instance_create(Object *p_this) {
	_THREAD_SAFE_METHOD_

	MixinScriptInstance *msi = memnew(MixinScriptInstance);
	msi->object = p_this;
	msi->owner = this;

	// Main script.
	if (main_script.is_valid()) {
		if (main_script->can_instance()) {
			// We still have to create dummy node to reflect main instance.
			msi->main_object = ClassDB::instance(p_this->get_class());
			msi->main_instance = main_script->instance_create(msi->main_object);
			base_class_name = p_this->get_class();
		}
	}
	// Mixins scripts.
	for (int i = 0; i < scripts.size(); i++) {
		ScriptInstance *si = nullptr;
		Ref<Script> script = scripts[i];

		if (script->can_instance()) {
			script_instances[i]->real_owner = p_this;
			si = script->instance_create(script_instances[i]);
		} else {
			si = nullptr;
		}
		msi->instances.push_back(si);
	}
	instances[p_this] = msi;
	p_this->_change_notify();

	return msi;
}

bool MixinScript::instance_has(const Object *p_this) const {
	_THREAD_SAFE_METHOD_
	return instances.has((Object *)p_this);
}

Error MixinScript::reload(bool p_keep_state) {
	if (main_script.is_valid()) {
		main_script->reload();
	}
	for (int i = 0; i < scripts.size(); i++) {
		Ref<Script> script = scripts[i];
		script->reload(p_keep_state);
	}
	return OK;
}

void MixinScript::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_main_script", "script"), &MixinScript::set_main_script);
	ClassDB::bind_method(D_METHOD("get_main_script"), &MixinScript::get_main_script);

	ClassDB::bind_method(D_METHOD("add_script", "script"), &MixinScript::add_script);
	ClassDB::bind_method(D_METHOD("remove_script", "index"), &MixinScript::remove_script);
	ClassDB::bind_method(D_METHOD("set_script_at_index", "index", "script"), &MixinScript::set_script_at_index);
	ClassDB::bind_method(D_METHOD("get_script_at_index", "index"), &MixinScript::get_script_at_index);
	ClassDB::bind_method(D_METHOD("get_script_count"), &MixinScript::get_script_count);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "main_script", PROPERTY_HINT_RESOURCE_TYPE, "Script"), "set_main_script", "get_main_script");
}

ScriptLanguage *MixinScript::get_language() const {
	return MixinScriptLanguage::get_singleton();
}

///////////////

MixinScript::MixinScript() {}

MixinScript::~MixinScript() {
	for (int i = 0; i < script_instances.size(); i++) {
		memdelete(script_instances[i]);
	}
	script_instances.resize(0);
}

Ref<Script> MixinScript::get_base_script() const {
	Ref<MixinScript> base_script;
	return base_script;
}

bool MixinScript::has_method(const StringName &p_method) const {
	if (main_script.is_valid()) {
		if (main_script->has_method(p_method)) {
			return true;
		}
	}
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->has_method(p_method)) {
			return true;
		}
	}
	return false;
}

MethodInfo MixinScript::get_method_info(const StringName &p_method) const {
	if (main_script.is_valid()) {
		if (main_script->has_method(p_method)) {
			return main_script->get_method_info(p_method);
		}
	}
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->has_method(p_method)) {
			return scripts[i]->get_method_info(p_method);
		}
	}
	return MethodInfo();
}

bool MixinScript::has_script_signal(const StringName &p_signal) const {
	if (main_script.is_valid()) {
		if (main_script->has_script_signal(p_signal)) {
			return true;
		}
	}
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->has_script_signal(p_signal)) {
			return true;
		}
	}
	return false;
}

void MixinScript::get_script_signal_list(List<MethodInfo> *r_signals) const {
	if (main_script.is_valid()) {
		main_script->get_script_signal_list(r_signals);
	}
	for (int i = 0; i < scripts.size(); i++) {
		scripts[i]->get_script_signal_list(r_signals);
	}
}

bool MixinScript::get_property_default_value(const StringName &p_property, Variant &r_value) const {
	if (main_script.is_valid()) {
		if (main_script->get_property_default_value(p_property, r_value)) {
			return true;
		}
	}
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->get_property_default_value(p_property, r_value)) {
			return true;
		}
	}
	return false;
}

void MixinScript::get_script_method_list(List<MethodInfo> *p_list) const {
	if (main_script.is_valid()) {
		main_script->get_script_method_list(p_list);
	}
	for (int i = 0; i < scripts.size(); i++) {
		scripts[i]->get_script_method_list(p_list);
	}
}

void MixinScript::get_script_property_list(List<PropertyInfo> *p_list) const {
	if (main_script.is_valid()) {
		main_script->get_script_property_list(p_list);
	}
	for (int i = 0; i < scripts.size(); i++) {
		scripts[i]->get_script_property_list(p_list);
	}
}

void MixinScript::update_exports() {
	if (main_script.is_valid()) {
		main_script->update_exports();
	}
	for (int i = 0; i < scripts.size(); i++) {
		Ref<Script> script = scripts[i];
		script->update_exports();
	}
}

MixinScriptLanguage *MixinScriptLanguage::singleton = nullptr;

Ref<Script> MixinScriptLanguage::get_template(const String &p_class_name, const String &p_base_class_name) const {
	MixinScript *s = memnew(MixinScript);
	s->base_class_name = p_base_class_name;
	return Ref<MixinScript>(s);
}

Script *MixinScriptLanguage::create_script() const {
	return memnew(MixinScript);
}

void MixinScriptLanguage::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("ms");
}

MixinScriptLanguage::MixinScriptLanguage() {
	singleton = this;
}

MixinScriptLanguage::~MixinScriptLanguage() {}

void Mixin::_bind_methods() {
	ClassDB::bind_method("get_owner", &Mixin::get_owner);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "owner"), "", "get_owner");
}

Variant Mixin::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	if (real_owner) {
		return real_owner->call(p_method, p_args, p_argcount, r_error);
	}
	return Variant();
}

void Mixin::call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount) {
	if (real_owner) {
		real_owner->call_multilevel(p_method, p_args, p_argcount);
	}
}

void Mixin::call_multilevel_reversed(const StringName &p_method, const Variant **p_args, int p_argcount) {
	if (real_owner) {
		real_owner->call_multilevel_reversed(p_method, p_args, p_argcount);
	}
}
