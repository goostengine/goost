#include "spawner_2d.h"

#include "core/engine.h"
#include "scene/resources/packed_scene.h"

#include "scene/scene_string_names.h"

StringName Spawner2D::node_spawned;

void Spawner2D::set_resource(const Ref<Resource> &p_resource) {
	const Ref<PackedScene> &scene = p_resource;
	const Ref<Script> &script = p_resource;

	if (scene.is_valid()) {
		resource = scene;
	} else if (script.is_valid()) {
		resource = script;
	} else {
		resource = Ref<Resource>();
		if (p_resource.is_valid()) {
			ERR_FAIL_MSG("Resource is not a PackedScene nor a Script");
		}
	}
}

void Spawner2D::set_enabled(bool p_enabled) {
	_set_process(p_enabled);

	delay_time = 0.0;
	time = 0.0;
	amount = 0;

	if (p_enabled && delay <= 0.0) {
		spawn();
	}
}

Node *Spawner2D::spawn() {
#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint()) {
		return nullptr;
	}
#endif
	ERR_FAIL_COND_V_MSG(!is_inside_tree(), nullptr,
			"Spawner2D must be added to the SceneTree in order to spawn nodes");

	const Ref<PackedScene> &scene = resource;
	const Ref<Script> &script = resource;

	Node *node = nullptr;

	if (scene.is_valid()) {
		node = scene->instance();

	} else if (script.is_valid()) {
		StringName base_type = script->get_instance_base_type();
		bool valid = !ClassDB::is_parent_class(base_type, "Node");
		ERR_FAIL_COND_V_MSG(valid, nullptr, "Script does not inherit a Node: " + script->get_path());

		Object *obj = ClassDB::instance(base_type);
		node = Object::cast_to<Node>(obj);
		Variant s = script;
		node->set_script(s);
	} else {
		ERR_FAIL_V_MSG(nullptr, "Could not spawn an object: the resource is not a scene nor a script.");
	}

	// Add child now, because we may need to set position of the spawned node.
	add_child(node);
	amount += 1;

	CanvasItem *c = Object::cast_to<CanvasItem>(node);
	if (c) {
		if (!local) {
			c->set_as_toplevel(true);
		}
		Node2D *n = Object::cast_to<Node2D>(node);
		if (n) {
			// Implementation from `RemoteTransform2D::_modify_remote()`
			if (!local) { // Using global coordinates.
				if (modify_position && modify_rotation && modify_scale) {
					n->set_global_transform(get_global_transform());
				} else {
					Transform2D n_trans = n->get_global_transform();
					Transform2D our_trans = get_global_transform();
					Vector2 n_scale = n->get_scale();

					if (!modify_position) {
						our_trans.set_origin(n_trans.get_origin());
					}
					if (!modify_rotation) {
						our_trans.set_rotation(n_trans.get_rotation());
					}

					n->set_global_transform(our_trans);

					if (modify_scale) {
						n->set_scale(get_global_scale());
					} else {
						n->set_scale(n_scale);
					}
				}
			}
		} else { // Using local coordinates.
			if (modify_position && modify_rotation && modify_scale) {
				n->set_transform(get_transform());
			} else {
				Transform2D n_trans = n->get_transform();
				Transform2D our_trans = get_transform();
				Vector2 n_scale = n->get_scale();

				if (!modify_position) {
					our_trans.set_origin(n_trans.get_origin());
				}
				if (!modify_rotation) {
					our_trans.set_rotation(n_trans.get_rotation());
				}

				n->set_transform(our_trans);

				if (modify_scale) {
					n->set_scale(get_scale());
				} else {
					n->set_scale(n_scale);
				}
			}
		}
	}
	emit_signal(node_spawned, node);

	return node;
}

void Spawner2D::set_rate(int p_rate) {
	ERR_FAIL_COND(p_rate < 1);
	rate = p_rate;
}

void Spawner2D::set_step(double p_step) {
	ERR_FAIL_COND(p_step <= 0.0);
	step = p_step;
}

void Spawner2D::set_delay(double p_delay) {
	ERR_FAIL_COND(p_delay < 0.0);
	delay = p_delay;
}

// void Spawner2D::set_lifetime(double p_lifetime) {
// 	ERR_FAIL_COND(p_lifetime < 0.0);
// 	lifetime = p_lifetime;
// }

void Spawner2D::set_limit(int p_limit) {
	ERR_FAIL_COND(p_limit < 0);
	limit = p_limit;
}

void Spawner2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (enabled) {
				set_enabled(true);
			}
		} break;
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (!enabled || process_mode == PROCESS_PHYSICS || !is_processing_internal()) {
				return;
			}
			delay_time += get_process_delta_time();
			time += get_process_delta_time();
			_process_spawn();
		} break;
		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			if (!enabled || process_mode == PROCESS_IDLE || !is_physics_processing_internal()) {
				return;
			}
			delay_time += get_physics_process_delta_time();
			time += get_physics_process_delta_time();
			_process_spawn();
		} break;
	}
}

void Spawner2D::_process_spawn() {
	if (delay > 0.0) {
		if (delay_time < delay) {
			return;
		}
	}
	if (time > (step / rate)) {
		spawn();
		time = 0.0;

		if (limit > 0 && amount >= limit) {
			set_enabled(false);
			emit_signal(SceneStringNames::get_singleton()->finished);
		}
	}
}

void Spawner2D::set_process_mode(ProcessMode p_mode) {
	if (process_mode == p_mode) {
		return;
	}
	switch (process_mode) {
		case PROCESS_PHYSICS:
			if (is_physics_processing_internal()) {
				set_physics_process_internal(false);
				set_process_internal(true);
			}
			break;
		case PROCESS_IDLE:
			if (is_processing_internal()) {
				set_process_internal(false);
				set_physics_process_internal(true);
			}
			break;
	}
	process_mode = p_mode;
}

void Spawner2D::_set_process(bool p_process) {
	switch (process_mode) {
		case PROCESS_PHYSICS: {
			set_physics_process_internal(p_process);
		} break;
		case PROCESS_IDLE: {
			set_process_internal(p_process);
		} break;
	}
	enabled = p_process;
}

void Spawner2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_resource", "object"), &Spawner2D::set_resource);
	ClassDB::bind_method(D_METHOD("get_resource"), &Spawner2D::get_resource);

	ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &Spawner2D::set_enabled);
	ClassDB::bind_method(D_METHOD("is_enabled"), &Spawner2D::is_enabled);

	ClassDB::bind_method(D_METHOD("set_local", "enabled"), &Spawner2D::set_local);
	ClassDB::bind_method(D_METHOD("is_local"), &Spawner2D::is_local);

	ClassDB::bind_method(D_METHOD("spawn"), &Spawner2D::spawn);

	ClassDB::bind_method(D_METHOD("set_rate", "rate"), &Spawner2D::set_rate);
	ClassDB::bind_method(D_METHOD("get_rate"), &Spawner2D::get_rate);

	ClassDB::bind_method(D_METHOD("set_step", "step"), &Spawner2D::set_step);
	ClassDB::bind_method(D_METHOD("get_step"), &Spawner2D::get_step);

	ClassDB::bind_method(D_METHOD("set_delay", "delay"), &Spawner2D::set_delay);
	ClassDB::bind_method(D_METHOD("get_delay"), &Spawner2D::get_delay);

	// ClassDB::bind_method(D_METHOD("set_lifetime", "lifetime"), &Spawner2D::set_lifetime);
	// ClassDB::bind_method(D_METHOD("get_lifetime"), &Spawner2D::get_lifetime);

	ClassDB::bind_method(D_METHOD("set_limit", "limit"), &Spawner2D::set_limit);
	ClassDB::bind_method(D_METHOD("get_limit"), &Spawner2D::get_limit);

	ClassDB::bind_method(D_METHOD("set_modify_position", "enabled"), &Spawner2D::set_modify_position);
	ClassDB::bind_method(D_METHOD("is_modifying_position"), &Spawner2D::is_modifying_position);

	ClassDB::bind_method(D_METHOD("set_modify_rotation", "enabled"), &Spawner2D::set_modify_rotation);
	ClassDB::bind_method(D_METHOD("is_modifying_rotation"), &Spawner2D::is_modifying_rotation);

	ClassDB::bind_method(D_METHOD("set_modify_scale", "enabled"), &Spawner2D::set_modify_scale);
	ClassDB::bind_method(D_METHOD("is_modifying_scale"), &Spawner2D::is_modifying_scale);

	ClassDB::bind_method(D_METHOD("set_process_mode", "process_mode"), &Spawner2D::set_process_mode);
	ClassDB::bind_method(D_METHOD("get_process_mode"), &Spawner2D::get_process_mode);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "resource", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene,Script"), "set_resource", "get_resource");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "local"), "set_local", "is_local");

	ADD_GROUP("Quantity", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rate", PROPERTY_HINT_RANGE, "1,20,1,or_greater"), "set_rate", "get_rate");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "limit", PROPERTY_HINT_RANGE, "0,20,1,or_greater"), "set_limit", "get_limit");

	ADD_GROUP("Time", "");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "step", PROPERTY_HINT_RANGE, "0.1,60.0,0.1,or_greater"), "set_step", "get_step");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "delay", PROPERTY_HINT_RANGE, "0.0,60.0,0.1,or_greater"), "set_delay", "get_delay");
	// ADD_PROPERTY(PropertyInfo(Variant::REAL, "lifetime", PROPERTY_HINT_RANGE, "0.0,5.0,0.1,or_greater"), "set_lifetime", "get_lifetime");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "process_mode", PROPERTY_HINT_ENUM, "Physics,Idle"), "set_process_mode", "get_process_mode");

	ADD_GROUP("Modify", "modify_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "modify_position"), "set_modify_position", "is_modifying_position");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "modify_rotation"), "set_modify_rotation", "is_modifying_rotation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "modify_scale"), "set_modify_scale", "is_modifying_scale");

	ADD_SIGNAL(MethodInfo("node_spawned", PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	ADD_SIGNAL(MethodInfo("finished"));

	BIND_ENUM_CONSTANT(PROCESS_PHYSICS);
	BIND_ENUM_CONSTANT(PROCESS_IDLE);
}
