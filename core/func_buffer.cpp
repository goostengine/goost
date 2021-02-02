#include "func_buffer.h"

Error FuncBuffer::push_call(ObjectID p_id, const StringName &p_method, const Variant **p_args, int p_argcount) {
	Call *c = memnew(Call);
	c->argcount = p_argcount;
	c->instance_id = p_id;
	c->target = p_method;

	for (int i = 0; i < p_argcount; i++) {
		c->args.push_back(*p_args[i]);
	}
	buffer.push_back(c);

	return OK;
}

Error FuncBuffer::push_call(ObjectID p_id, const StringName &p_method, VARIANT_ARG_DECLARE) {
	VARIANT_ARGPTRS;

	int argc = 0;
	for (int i = 0; i < VARIANT_ARG_MAX; i++) {
		if (argptr[i]->get_type() == Variant::NIL) {
			break;
        }
		argc++;
	}
	return push_call(p_id, p_method, argptr, argc);
}

Error FuncBuffer::push_call_unique(ObjectID p_id, const StringName &p_method, const Variant **p_args, int p_argcount) {
	bool found_exact = false;
	for (int i = 0; i < buffer.size(); ++i) {
		const Call *c = buffer[i];
		if (c->instance_id == p_id && c->target == p_method) {
			if (c->argcount == p_argcount) {
				bool match = true;
				for (int j = 0; j < p_argcount; ++j) {
					if (c->args[j] != *p_args[j]) {
						match = false;
						break;
					}
				}
				if (match) {
					found_exact = true;
					break;
				}
			}
		}
	}
	if (!found_exact) {
		return push_call(p_id, p_method, p_args, p_argcount);
	}
	return OK;
}

Error FuncBuffer::push_call_unique(ObjectID p_id, const StringName &p_method, VARIANT_ARG_DECLARE) {
	VARIANT_ARGPTRS;

	int argc = 0;
	for (int i = 0; i < VARIANT_ARG_MAX; i++) {
		if (argptr[i]->get_type() == Variant::NIL) {
			break;
        }
		argc++;
	}
	return push_call_unique(p_id, p_method, argptr, argc);
}

void FuncBuffer::flush() {
	for (int i = 0; i < buffer.size(); ++i) {
		Call *c = buffer[i];
		const Variant **argptrs = nullptr;
		if (c->argcount > 0) {
			argptrs = (const Variant **)alloca(sizeof(Variant *) * c->argcount);
			Variant *r = c->args.ptrw();
			for (int j = 0; j < c->argcount; ++j) {
				argptrs[j] = &r[j];
			}
		}
		Variant::CallError ce;
		Object *obj = ObjectDB::get_instance(c->instance_id);
		if (obj != nullptr) {
			obj->call(c->target, argptrs, c->argcount, ce);
			if (ce.error != Variant::CallError::CALL_OK) {
				ERR_PRINT("Error calling deferred method: " + Variant::get_call_error_text(obj, c->target, argptrs, c->argcount, ce) + ".");
			}
		}
		memdelete(c);
	}
	buffer.clear();
}
