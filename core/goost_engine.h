#ifndef GOOST_ENGINE_H
#define GOOST_ENGINE_H

#include "core/object.h"
#include "func_buffer.h"

class GoostEngine : public Object {
	GDCLASS(GoostEngine, Object);

private:
	static GoostEngine *singleton;
	FuncBuffer deferred_calls;

protected:
	static void _bind_methods();

public:
	static GoostEngine *get_singleton() { return singleton; }

	Dictionary get_color_constants() const;

	void defer_call_unique(Object *p_obj, StringName p_method, VARIANT_ARG_LIST);
	Variant _defer_call_unique_bind(const Variant **p_args, int p_argcount, Variant::CallError &r_error);

	static void flush_calls();

	GoostEngine() {
		ERR_FAIL_COND_MSG(singleton != nullptr, "Singleton already exists");
		singleton = this;
	}
};

#endif // GOOST_ENGINE_H
