#include "multi_script.h"

bool MultiScriptInstance::set(const StringName &p_name, const Variant &p_value) {
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

bool MultiScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
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

void MultiScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
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

void MultiScriptInstance::get_method_list(List<MethodInfo> *p_list) const {
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

bool MultiScriptInstance::has_method(const StringName &p_method) const {
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i] && sarr[i]->has_method(p_method)) {
			return true;
		}
	}
	return false;
}

Variant MultiScriptInstance::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
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

void MultiScriptInstance::call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount) {
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i]) {
			sarr[i]->call_multilevel(p_method, p_args, p_argcount);
		}
	}
}

void MultiScriptInstance::notification(int p_notification) {
	for (int i = 0; i < instances.size(); i++) {
		ScriptInstance *instance = instances[i];
		if (instance) {
			instance->notification(p_notification);
		}
	}
}

ScriptLanguage *MultiScriptInstance::get_language() {
	return MultiScriptLanguage::get_singleton();
}

MultiScriptInstance::~MultiScriptInstance() {
	owner->remove_instance(object);
}

Variant::Type MultiScriptInstance::get_property_type(const StringName &p_name, bool *r_is_valid) const {
	bool valid = false;
	Variant::Type type;

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

MultiplayerAPI::RPCMode MultiScriptInstance::get_rpc_mode(const StringName &p_method) const {
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i]) {
			if (sarr[i]->has_method(p_method)) {
				return sarr[i]->get_rpc_mode(p_method);
			}
		}
	}
	return MultiplayerAPI::RPC_MODE_DISABLED;
}

MultiplayerAPI::RPCMode MultiScriptInstance::get_rset_mode(const StringName &p_variable) const {
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

bool MultiScript::is_tool() const {
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->is_tool()) {
			return true;
		}
	}
	return false;
}

bool MultiScript::is_valid() const {
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->is_valid()) {
			return true;
		}
	}
	return false;
}

bool MultiScript::_set(const StringName &p_name, const Variant &p_value) {
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
				remove_owner_script(idx);
			} else {
				set_owner_script(idx, s);
			}
		} else if (idx == scripts.size()) {
			if (s.is_null()) {
				return false;
			}
			add_owner_script(s);
		} else {
			return false;
		}
		return true;
	}
	return false;
}

bool MultiScript::_get(const StringName &p_name, Variant &r_ret) const {
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
			r_ret = get_owner_script(idx);
			return true;
		} else if (idx == scripts.size()) {
			r_ret = Ref<Script>();
			return true;
		}
	}
	return false;
}

void MultiScript::_get_property_list(List<PropertyInfo> *p_list) const {
	_THREAD_SAFE_METHOD_

	for (int i = 0; i < scripts.size(); i++) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "script_" + String::chr('a' + i), PROPERTY_HINT_RESOURCE_TYPE, "Script"));
	}
	if (scripts.size() < 25) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "script_" + String::chr('a' + (scripts.size())), PROPERTY_HINT_RESOURCE_TYPE, "Script"));
	}
}

void MultiScript::set_owner_script(int p_idx, const Ref<Script> &p_script) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_INDEX(p_idx, scripts.size());
	ERR_FAIL_COND(p_script.is_null());

	scripts.set(p_idx, p_script);
	Ref<Script> s = p_script;

	for (Map<Object *, MultiScriptInstance *>::Element *E = instances.front(); E; E = E->next()) {
		MultiScriptInstance *msi = E->get();
		ScriptInstance **si = msi->instances.ptrw();
		if (si[p_idx]) {
			si[p_idx] = nullptr;
			memdelete(si);
		}
		if (p_script->can_instance()) {
			si[p_idx] = s->instance_create(msi->object);
		}
	}
}

Ref<Script> MultiScript::get_owner_script(int p_idx) const {
	_THREAD_SAFE_METHOD_
	ERR_FAIL_INDEX_V(p_idx, scripts.size(), Ref<Script>());

	return scripts[p_idx];
}

void MultiScript::add_owner_script(const Ref<Script> &p_script) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_COND(p_script.is_null());
	MultiScriptOwner *script_owner = memnew(MultiScriptOwner);
	script_instances.push_back(script_owner);
	scripts.push_back(p_script);
	Ref<Script> s = p_script;

	for (Map<Object *, MultiScriptInstance *>::Element *E = instances.front(); E; E = E->next()) {
		MultiScriptInstance *msi = E->get();

		if (p_script->can_instance()) {
			script_owner->real_owner = msi->object;
			msi->instances.push_back(s->instance_create(script_owner));
		} else {
			msi->instances.push_back(nullptr);
		}
		msi->object->_change_notify();
	}
	_change_notify();
}

void MultiScript::remove_owner_script(int p_idx) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_INDEX(p_idx, scripts.size());

	scripts.remove(p_idx);
	script_instances.remove(p_idx);

	for (Map<Object *, MultiScriptInstance *>::Element *E = instances.front(); E; E = E->next()) {
		MultiScriptInstance *msi = E->get();
		ScriptInstance *si = msi->instances[p_idx];
		msi->instances.remove(p_idx);
		if (si) {
			memdelete(si);
		}
		msi->object->_change_notify();
	}
}

void MultiScript::remove_instance(Object *p_object) {
	_THREAD_SAFE_METHOD_
	instances.erase(p_object);
}

StringName MultiScript::get_instance_base_type() const {
	return base_class_name;
}

ScriptInstance *MultiScript::instance_create(Object *p_this) {
	_THREAD_SAFE_METHOD_

	MultiScriptInstance *msi = memnew(MultiScriptInstance);
	msi->object = p_this;
	msi->owner = this;

	for (int i = 0; i < scripts.size(); i++) {
		ScriptInstance *si;
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

bool MultiScript::instance_has(const Object *p_this) const {
	_THREAD_SAFE_METHOD_
	return instances.has((Object *)p_this);
}

Error MultiScript::reload(bool p_keep_state) {
	for (int i = 0; i < scripts.size(); i++) {
		Ref<Script> script = scripts[i];
		script->reload(p_keep_state);
	}
	return OK;
}

void MultiScript::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_owner_script", "script"), &MultiScript::add_owner_script);
	ClassDB::bind_method(D_METHOD("remove_owner_script", "index"), &MultiScript::remove_owner_script);
	ClassDB::bind_method(D_METHOD("set_owner_script", "index", "script"), &MultiScript::set_owner_script);
	ClassDB::bind_method(D_METHOD("get_owner_script", "index"), &MultiScript::get_owner_script);
}

ScriptLanguage *MultiScript::get_language() const {
	return MultiScriptLanguage::get_singleton();
}

///////////////

MultiScript::MultiScript() {}

MultiScript::~MultiScript() {
	for (int i = 0; i < script_instances.size(); i++) {
		memdelete(script_instances[i]);
	}
	script_instances.resize(0);
}

Ref<Script> MultiScript::get_base_script() const {
	Ref<MultiScript> base_script;
	return base_script;
}

bool MultiScript::has_method(const StringName &p_method) const {
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->has_method(p_method)) {
			return true;
		}
	}
	return false;
}

MethodInfo MultiScript::get_method_info(const StringName &p_method) const {
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->has_method(p_method)) {
			return scripts[i]->get_method_info(p_method);
		}
	}
	return MethodInfo();
}

bool MultiScript::has_script_signal(const StringName &p_signal) const {
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->has_script_signal(p_signal)) {
			return true;
		}
	}
	return false;
}

void MultiScript::get_script_signal_list(List<MethodInfo> *r_signals) const {
	for (int i = 0; i < scripts.size(); i++) {
		scripts[i]->get_script_signal_list(r_signals);
	}
}

bool MultiScript::get_property_default_value(const StringName &p_property, Variant &r_value) const {
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i]->get_property_default_value(p_property, r_value)) {
			return true;
		}
	}
	return false;
}

void MultiScript::get_script_method_list(List<MethodInfo> *p_list) const {
	for (int i = 0; i < scripts.size(); i++) {
		scripts[i]->get_script_method_list(p_list);
	}
}

void MultiScript::get_script_property_list(List<PropertyInfo> *p_list) const {
	for (int i = 0; i < scripts.size(); i++) {
		scripts[i]->get_script_property_list(p_list);
	}
}

void MultiScript::update_exports() {
	for (int i = 0; i < scripts.size(); i++) {
		Ref<Script> script = scripts[i];
		script->update_exports();
	}
}

MultiScriptLanguage *MultiScriptLanguage::singleton = nullptr;

Ref<Script> MultiScriptLanguage::get_template(const String &p_class_name, const String &p_base_class_name) const {
	MultiScript *s = memnew(MultiScript);
	s->base_class_name = p_base_class_name;
	return Ref<MultiScript>(s);
}

Script *MultiScriptLanguage::create_script() const {
	return memnew(MultiScript);
}

void MultiScriptLanguage::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("ms");
}

void MultiScriptLanguage::get_public_functions(List<MethodInfo> *p_functions) const {}

MultiScriptLanguage::MultiScriptLanguage() {
	singleton = this;
}

MultiScriptLanguage::~MultiScriptLanguage() {}

void MultiScriptOwner::_bind_methods() {
	ClassDB::bind_method("get_owner", &MultiScriptOwner::get_owner);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "owner"), "", "get_owner");
}

Variant MultiScriptOwner::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	if (real_owner) {
		return real_owner->call(p_method, p_args, p_argcount, r_error);
	}
	return Variant();
}

void MultiScriptOwner::call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount) {
	if (real_owner) {
		real_owner->call_multilevel(p_method, p_args, p_argcount);
	}
}

void MultiScriptOwner::call_multilevel_reversed(const StringName &p_method, const Variant **p_args, int p_argcount) {
	if (real_owner) {
		real_owner->call_multilevel_reversed(p_method, p_args, p_argcount);
	}
}
