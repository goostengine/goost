#include "stopwatch.h"

#include "core/engine.h"

void Stopwatch::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (autostart) {
#ifdef TOOLS_ENABLED
				if (Engine::get_singleton()->is_editor_hint() && get_tree()->get_edited_scene_root() && (get_tree()->get_edited_scene_root() == this || get_tree()->get_edited_scene_root()->is_a_parent_of(this))) {
					break;
				}
#endif
				start();
				autostart = false;
			}
		} break;
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (!running || process_mode == PROCESS_PHYSICS || !is_processing_internal()) {
				return;
			}
			time_elapsed += get_process_delta_time();
		} break;
		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			if (!running || process_mode == PROCESS_IDLE || !is_physics_processing_internal()) {
				return;
			}
			time_elapsed += get_physics_process_delta_time();
		} break;
	}
}

void Stopwatch::start() {
	ERR_FAIL_COND_MSG(!is_inside_tree(), "Stopwatch was not added to the SceneTree. Either add it or set autostart to true.");
	ERR_FAIL_COND_MSG(is_running(), "Stopwatch is already running. Finish measuring the current time interval with stop() first.");
	time_start = time_elapsed;
	_set_process(true);
}

void Stopwatch::stop() {
	_set_process(false);
	time_stop = time_elapsed;
	double time_interval = time_stop - time_start;
	if (time_interval > 0) {
		emit_signal("interval_measured", time_interval, time_start, time_stop);
	}
}

void Stopwatch::reset() {
	ERR_FAIL_COND_MSG(is_running(), "Stopwatch is currently running. Finish measuring the current time interval with stop() first.");
	time_elapsed = 0.0;
	time_start = 0.0;
	time_stop = 0.0;
}

void Stopwatch::set_process_mode(ProcessMode p_mode) {
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

void Stopwatch::_set_process(bool p_process) {
	switch (process_mode) {
		case PROCESS_PHYSICS: {
			set_physics_process_internal(p_process);
		} break;
		case PROCESS_IDLE: {
			set_process_internal(p_process);
		} break;
	}
	running = p_process;
}

void Stopwatch::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_autostart", "enable"), &Stopwatch::set_autostart);
	ClassDB::bind_method(D_METHOD("has_autostart"), &Stopwatch::has_autostart);

	ClassDB::bind_method(D_METHOD("start"), &Stopwatch::start);
	ClassDB::bind_method(D_METHOD("is_running"), &Stopwatch::is_running);

	ClassDB::bind_method(D_METHOD("stop"), &Stopwatch::stop);
	ClassDB::bind_method(D_METHOD("is_stopped"), &Stopwatch::is_stopped);

	ClassDB::bind_method(D_METHOD("reset"), &Stopwatch::reset);

	ClassDB::bind_method(D_METHOD("get_time_elapsed"), &Stopwatch::get_time_elapsed);

	ClassDB::bind_method(D_METHOD("set_process_mode", "callback"), &Stopwatch::set_process_mode);
	ClassDB::bind_method(D_METHOD("get_process_mode"), &Stopwatch::get_process_mode);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "process_mode", PROPERTY_HINT_ENUM, "Physics,Idle"), "set_process_mode", "get_process_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "autostart"), "set_autostart", "has_autostart");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "time_elapsed", PROPERTY_HINT_NONE, "", 0), "", "get_time_elapsed");

	ADD_SIGNAL(MethodInfo("interval_measured",
			PropertyInfo(Variant::REAL, "time_interval"),
			PropertyInfo(Variant::REAL, "time_start"),
			PropertyInfo(Variant::REAL, "time_stop")));

	BIND_ENUM_CONSTANT(PROCESS_PHYSICS);
	BIND_ENUM_CONSTANT(PROCESS_IDLE);
}
