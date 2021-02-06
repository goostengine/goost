#ifndef GOOST_ENGINE_H
#define GOOST_ENGINE_H

#include "core/object.h"
#include "scene/main/scene_tree.h"

#include "func_buffer.h"
#include "invoke_state.h"

class GoostEngine : public Object {
	GDCLASS(GoostEngine, Object);

private:
	static GoostEngine *singleton;
	FuncBuffer deferred_calls;
	Vector<Ref<InvokeState>> invokes;

protected:
	static void _bind_methods();

	Variant _defer_call_bind(const Variant **p_args, int p_argcount, Variant::CallError &r_error);
	Variant _defer_call_unique_bind(const Variant **p_args, int p_argcount, Variant::CallError &r_error);

	Ref<InvokeState> _invoke(Object *p_obj, StringName p_method, real_t p_delay, real_t p_repeat_rate, bool p_pause_mode_process, bool p_deferred);
	void _on_invoke_timeout(Ref<InvokeState> p_state, bool p_pause_mode, bool p_deferred);

public:
	static GoostEngine *get_singleton() { return singleton; }

	Dictionary get_color_constants() const;

	void defer_call(Object *p_obj, StringName p_method, VARIANT_ARG_LIST);
	void defer_call_unique(Object *p_obj, StringName p_method, VARIANT_ARG_LIST);

	Ref<InvokeState> invoke(Object *p_obj, StringName p_method, real_t p_delay, real_t p_repeat_rate, bool p_pause_mode_process = true);
	Ref<InvokeState> invoke_deferred(Object *p_obj, StringName p_method, real_t p_delay, real_t p_repeat_rate, bool p_pause_mode_process = true);
	Array get_invokes() const;

	static void flush_calls();

	GoostEngine() {
		ERR_FAIL_COND_MSG(singleton != nullptr, "Singleton already exists");
		singleton = this;
	}
};

#endif // GOOST_ENGINE_H
