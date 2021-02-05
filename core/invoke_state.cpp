#include "invoke_state.h"
#include "scene/scene_string_names.h"

void InvokeState::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_target"), &InvokeState::get_target);
	ClassDB::bind_method(D_METHOD("get_target_method"), &InvokeState::get_target_method);

	ClassDB::bind_method(D_METHOD("cancel"), &InvokeState::cancel);
	ClassDB::bind_method(D_METHOD("is_active"), &InvokeState::is_active);
	ClassDB::bind_method(D_METHOD("is_repeating"), &InvokeState::is_repeating);

	ClassDB::bind_method(D_METHOD("get_time_left"), &InvokeState::get_time_left);
	
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "target"), "", "get_target");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "method"), "", "get_target_method");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "time_left"), "", "get_time_left");

	ADD_SIGNAL(MethodInfo("pre_call"));
	ADD_SIGNAL(MethodInfo("post_call"));
	ADD_SIGNAL(MethodInfo("completed"));
}
