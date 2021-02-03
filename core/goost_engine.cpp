#include "goost_engine.h"

#include "core/color_names.inc"
#include "core/os/os.h"

GoostEngine *GoostEngine::singleton = nullptr;

Dictionary GoostEngine::get_color_constants() const {
	if (_named_colors.empty()) {
		_populate_named_colors(); // color_names.inc
	}
	Dictionary colors;
	for (Map<String, Color>::Element *E = _named_colors.front(); E; E = E->next()) {
		colors[E->key()] = E->get();
	}
	return colors;
}

void GoostEngine::defer_call_unique(Object *p_obj, StringName p_method, VARIANT_ARG_DECLARE) {
	deferred_calls.push_call_unique(p_obj->get_instance_id(), p_method, VARIANT_ARG_PASS);
}

Variant GoostEngine::_defer_call_unique_bind(const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	if (p_argcount < 2) {
		r_error.error = Variant::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
		r_error.argument = 0;
		return Variant();
	}
	if (p_args[0]->get_type() != Variant::OBJECT) {
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_ARGUMENT;
		r_error.argument = 0;
		r_error.expected = Variant::OBJECT;
		return Variant();
	}
	if (p_args[1]->get_type() != Variant::STRING) {
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_ARGUMENT;
		r_error.argument = 1;
		r_error.expected = Variant::STRING;
		return Variant();
	}
	r_error.error = Variant::CallError::CALL_OK;

	Object *obj = *p_args[0];
	StringName method = *p_args[1];
	deferred_calls.push_call_unique(obj->get_instance_id(), method, &p_args[2], p_argcount - 2);

	return Variant();
}

void GoostEngine::_invoke(Object *p_obj, StringName p_method, real_t p_sec, bool p_pause_mode, bool p_deferred) {
	ERR_FAIL_NULL_MSG(p_obj, "Invalid object");

	SceneTree *tree = Object::cast_to<SceneTree>(OS::get_singleton()->get_main_loop());
	ERR_FAIL_NULL_MSG(tree, "The `invoke()` method requires a SceneTree to work.");

	Ref<SceneTreeTimer> timer = tree->create_timer(p_sec, p_pause_mode);
	timer->connect("timeout", this, "_on_invoke_timeout",
			varray(p_obj->get_instance_id(), p_method), p_deferred ? CONNECT_DEFERRED : 0);
}

void GoostEngine::invoke(Object *p_obj, StringName p_method, real_t p_sec, bool p_pause_mode) {
	_invoke(p_obj, p_method, p_sec, p_pause_mode, false);
}

void GoostEngine::invoke_deferred(Object *p_obj, StringName p_method, real_t p_sec, bool p_pause_mode) {
	_invoke(p_obj, p_method, p_sec, p_pause_mode, true);
}

void GoostEngine::_on_invoke_timeout(ObjectID p_id, StringName p_method) {
	Object *obj = ObjectDB::get_instance(p_id);
	if (!obj) {
		return; // Might be gone by this time.
	}
	Variant::CallError ce;
	obj->call(p_method, nullptr, 0, ce);
	if (ce.error != Variant::CallError::CALL_OK) {
		ERR_PRINT("Error invoking method: " + Variant::get_call_error_text(obj, p_method, nullptr, 0, ce) + ".");
	}
}

void GoostEngine::flush_calls() {
	singleton->deferred_calls.flush();
}

void GoostEngine::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_color_constants"), &GoostEngine::get_color_constants);
	{
		MethodInfo mi;
		mi.name = "defer_call_unique";
		mi.arguments.push_back(PropertyInfo(Variant::OBJECT, "object"));
		mi.arguments.push_back(PropertyInfo(Variant::STRING, "method"));
		ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "defer_call_unique", &GoostEngine::_defer_call_unique_bind, mi, varray(), false);
	}
	ClassDB::bind_method(D_METHOD("invoke", "object", "method", "delay_seconds", "pause_mode_process"), &GoostEngine::invoke, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("invoke_deferred", "object", "method", "delay_seconds", "pause_mode_process"), &GoostEngine::invoke_deferred, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("_on_invoke_timeout"), &GoostEngine::_on_invoke_timeout);
}
