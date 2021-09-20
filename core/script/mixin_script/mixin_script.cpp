#include "mixin_script.h"

bool MixinScriptInstance::set(const StringName &p_name, const Variant &p_value) {
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i]) {
			bool found = sarr[i]->set(p_name, p_value);
			if (found) {
				return true;
			}
		}
	}
	return false;
}

bool MixinScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i]) {
			bool found = sarr[i]->get(p_name, r_ret);
			if (found) {
				return true;
			}
		}
	}
	return false;
}

void MixinScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
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
}

void MixinScriptInstance::get_method_list(List<MethodInfo> *p_list) const {
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
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i] && sarr[i]->has_method(p_method)) {
			return true;
		}
	}
	return false;
}

Variant MixinScriptInstance::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
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
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i]) {
			sarr[i]->call_multilevel(p_method, p_args, p_argcount);
		}
	}
}

void MixinScriptInstance::notification(int p_notification) {
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
	ScriptInstance *const *sarr = instances.ptr();

	for (int i = 0; i < instances.size(); i++) {
		if (sarr[i] && sarr[i]->has_method(p_method)) {
			return sarr[i]->get_rpc_mode(p_method);
		}
	}
	return MultiplayerAPI::RPC_MODE_DISABLED;
}

MultiplayerAPI::RPCMode MixinScriptInstance::get_rset_mode(const StringName &p_variable) const {
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

bool MixinScript::is_tool() const {
	for (int i = 0; i < mixins.size(); i++) {
		if (mixins[i]->is_tool()) {
			return true;
		}
	}
	return false;
}

bool MixinScript::is_valid() const {
	for (int i = 0; i < mixins.size(); i++) {
		if (mixins[i]->is_valid()) {
			return true;
		}
	}
	return false;
}

bool MixinScript::inherits_script(const Ref<Script> &p_script) const {
	// There is no inheritance in mixin scripts, so this is enough.
	return this == p_script.ptr(); 
}

void MixinScript::clear_mixins() {
	while (mixins.size()) {
		remove_mixin(0);
	}
}

void MixinScript::set_mixins(const Array &p_mixins) {
	clear_mixins();
	for (int i = 0; i < p_mixins.size(); ++i) {
		Ref<Script> m = p_mixins[i];
		ERR_CONTINUE_MSG(m.is_null(), "Not a script.");
		add_mixin(m);
	}
}

Array MixinScript::get_mixins() const {
	Array ret;
	for (int i = 0; i < mixins.size(); ++i) {
		ret.push_back(mixins[i]);
	}
	return ret;
}

void MixinScript::set_mixin(int p_idx, const Ref<Script> &p_script) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_INDEX(p_idx, mixins.size());
	ERR_FAIL_COND(p_script.is_null());

	mixins.set(p_idx, p_script);
	Ref<Script> s = p_script;

	for (Map<Object *, MixinScriptInstance *>::Element *E = instances.front(); E; E = E->next()) {
		MixinScriptInstance *msi = E->get();
		// Looks like there's no need to explicitly free a previous
		// script instance in `msi->instances`, because `s->instance_create`
		// will free it by itself, otherwise this leads to random crashes.
		if (s->can_instance()) {
			msi->instances.set(p_idx, s->instance_create(mixin_instances[p_idx]));
		} else {
			msi->instances.set(p_idx, nullptr);
		}
	}
	emit_changed();
}

void MixinScript::move_mixin(int p_to_pos, const Ref<Script> &p_script) {
	int to_pos = p_to_pos;
	if (to_pos == mixins.size()) {
		to_pos--;
	}
	int from_pos = -1;
	for (int i = 0; i < mixins.size(); ++i) {
		const Ref<Script> &s = mixins[i];
		if (s == p_script) {
			from_pos = i;
			break;
		}
	}
	ERR_FAIL_COND_MSG(from_pos == -1, "Cannot move script: not part of MixinScript.")

	if (from_pos == to_pos) {
		return;
	}
	remove_mixin(from_pos);
	insert_mixin(to_pos, p_script);
}

Ref<Script> MixinScript::get_mixin(int p_idx) const {
	_THREAD_SAFE_METHOD_
	ERR_FAIL_INDEX_V(p_idx, mixins.size(), Ref<Script>());

	return mixins[p_idx];
}

void MixinScript::add_mixin(const Ref<Script> &p_script) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_COND(p_script.is_null());
	ERR_FAIL_COND_MSG(p_script.ptr() == this, "Cannot add MixinScript to itself.");

	Mixin *script_owner = memnew(Mixin);
	mixin_instances.push_back(script_owner);
	mixins.push_back(p_script);
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

void MixinScript::remove_mixin(int p_idx) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_INDEX(p_idx, mixins.size());

	mixins.remove(p_idx);
	Mixin *m = mixin_instances.get(p_idx);
	if (m) {
		memdelete(m);
	}
	mixin_instances.remove(p_idx);

	for (Map<Object *, MixinScriptInstance *>::Element *E = instances.front(); E; E = E->next()) {
		MixinScriptInstance *msi = E->get();
		msi->instances.remove(p_idx);
		msi->object->_change_notify();
	}
	emit_changed();
}

void MixinScript::insert_mixin(int p_pos, const Ref<Script> &p_script) {
	_THREAD_SAFE_METHOD_

	ERR_FAIL_COND(p_script.is_null());
	ERR_FAIL_INDEX(p_pos, mixins.size() + 1);

	Mixin *script_owner = memnew(Mixin);
	mixin_instances.insert(p_pos, script_owner);
	mixins.insert(p_pos, p_script);
	Ref<Script> s = p_script;
	
	for (Map<Object *, MixinScriptInstance *>::Element *E = instances.front(); E; E = E->next()) {
		MixinScriptInstance *msi = E->get();

		if (s->can_instance()) {
			script_owner->real_owner = msi->object;
			msi->instances.insert(p_pos, s->instance_create(script_owner));
		} else {
			msi->instances.insert(p_pos, nullptr);
		}
		msi->object->_change_notify();
	}
	_change_notify();
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

	for (int i = 0; i < mixins.size(); i++) {
		ScriptInstance *si = nullptr;
		Ref<Script> script = mixins[i];

		if (script->can_instance()) {
			mixin_instances[i]->real_owner = p_this;
			si = script->instance_create(mixin_instances[i]);
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
	for (int i = 0; i < mixins.size(); i++) {
		Ref<Script> script = mixins[i];
		script->reload(p_keep_state);
	}
	return OK;
}

void MixinScript::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_mixin", "script"), &MixinScript::add_mixin);
	ClassDB::bind_method(D_METHOD("remove_mixin", "index"), &MixinScript::remove_mixin);
	ClassDB::bind_method(D_METHOD("move_mixin", "position", "script"), &MixinScript::move_mixin);
	ClassDB::bind_method(D_METHOD("insert_mixin", "position", "script"), &MixinScript::insert_mixin);
	ClassDB::bind_method(D_METHOD("set_mixin", "index", "script"), &MixinScript::set_mixin);
	ClassDB::bind_method(D_METHOD("get_mixin", "index"), &MixinScript::get_mixin);
	ClassDB::bind_method(D_METHOD("get_mixin_count"), &MixinScript::get_mixin_count);
	ClassDB::bind_method(D_METHOD("clear_mixins"), &MixinScript::clear_mixins);

	ClassDB::bind_method(D_METHOD("set_mixins", "mixins"), &MixinScript::set_mixins);
	ClassDB::bind_method(D_METHOD("get_mixins"), &MixinScript::get_mixins);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "mixins", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "set_mixins", "get_mixins");
}

ScriptLanguage *MixinScript::get_language() const {
	return MixinScriptLanguage::get_singleton();
}

///////////////

MixinScript::MixinScript() {}

MixinScript::~MixinScript() {
	for (int i = 0; i < mixin_instances.size(); i++) {
		memdelete(mixin_instances[i]);
	}
	mixin_instances.resize(0);
}

Ref<Script> MixinScript::get_base_script() const {
	Ref<MixinScript> base_script;
	return base_script;
}

bool MixinScript::has_method(const StringName &p_method) const {
	for (int i = 0; i < mixins.size(); i++) {
		if (mixins[i]->has_method(p_method)) {
			return true;
		}
	}
	return false;
}

MethodInfo MixinScript::get_method_info(const StringName &p_method) const {
	for (int i = 0; i < mixins.size(); i++) {
		if (mixins[i]->has_method(p_method)) {
			return mixins[i]->get_method_info(p_method);
		}
	}
	return MethodInfo();
}

bool MixinScript::has_script_signal(const StringName &p_signal) const {
	for (int i = 0; i < mixins.size(); i++) {
		if (mixins[i]->has_script_signal(p_signal)) {
			return true;
		}
	}
	return false;
}

void MixinScript::get_script_signal_list(List<MethodInfo> *r_signals) const {
	for (int i = 0; i < mixins.size(); i++) {
		mixins[i]->get_script_signal_list(r_signals);
	}
}

bool MixinScript::get_property_default_value(const StringName &p_property, Variant &r_value) const {
	for (int i = 0; i < mixins.size(); i++) {
		if (mixins[i]->get_property_default_value(p_property, r_value)) {
			return true;
		}
	}
	return false;
}

void MixinScript::get_script_method_list(List<MethodInfo> *p_list) const {
	for (int i = 0; i < mixins.size(); i++) {
		mixins[i]->get_script_method_list(p_list);
	}
}

void MixinScript::get_script_property_list(List<PropertyInfo> *p_list) const {
	for (int i = 0; i < mixins.size(); i++) {
		mixins[i]->get_script_property_list(p_list);
	}
}

void MixinScript::update_exports() {
	for (int i = 0; i < mixins.size(); i++) {
		Ref<Script> script = mixins[i];
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
	ERR_FAIL_COND_MSG(singleton != nullptr, "Singleton already exists");
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
#ifdef DEBUG_ENABLED
	// The following allows to call `Mixin.free()`.
	// This is not strictly needed because Mixin objects should not be instantiated directly,
	// and doing so is discouraged, but this can make debug builds more robust for fuzz testing. 
	return Object::call(p_method, p_args, p_argcount, r_error);
#endif
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
