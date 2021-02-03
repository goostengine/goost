#ifndef GOOST_ENGINE_H
#define GOOST_ENGINE_H

#include "scene/main/scene_tree.h"
#include "core/object.h"
#include "func_buffer.h"

class GoostEngine : public Object {
	GDCLASS(GoostEngine, Object);

private:
	static GoostEngine *singleton;
	FuncBuffer deferred_calls;

protected:
	static void _bind_methods();

	Variant _defer_call_unique_bind(const Variant **p_args, int p_argcount, Variant::CallError &r_error);

	void _invoke(Object *p_obj, StringName p_method, real_t p_delay_seconds, bool p_pause_mode_process, bool p_deferred);
	void _on_invoke_timeout(ObjectID p_id, StringName p_method);

public:
	static GoostEngine *get_singleton() { return singleton; }

	Dictionary get_color_constants() const;

	void defer_call_unique(Object *p_obj, StringName p_method, VARIANT_ARG_LIST);

	void invoke(Object *p_obj, StringName p_method, real_t p_delay_seconds, bool p_pause_mode_process = true);
	void invoke_deferred(Object *p_obj, StringName p_method, real_t p_delay_seconds, bool p_pause_mode_process = true);

	static void flush_calls();

	GoostEngine() {
		ERR_FAIL_COND_MSG(singleton != nullptr, "Singleton already exists");
		singleton = this;
	}
};

#endif // GOOST_ENGINE_H
