#pragma once

#include "scene/main/node.h"

class Stopwatch : public Node {
	GDCLASS(Stopwatch, Node);

	bool autostart = false;
    bool running = false;

	double time_elapsed = 0.0;
    double time_start = 0.0;
    double time_stop = 0.0;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	enum ProcessMode {
		PROCESS_PHYSICS,
		PROCESS_IDLE,
	};
	void set_process_mode(ProcessMode p_mode);
	ProcessMode get_process_mode() const { return process_mode; };

	void set_autostart(bool p_enable) { autostart = p_enable; }
	bool has_autostart() const { return autostart; }

	void start();
	bool is_running() const { return running; }

	void stop();
	bool is_stopped() const { return !running; }

    void reset();

    double get_time_elapsed() { return time_elapsed; }

private:
	ProcessMode process_mode = PROCESS_IDLE;
	void _set_process(bool p_process);
};

VARIANT_ENUM_CAST(Stopwatch::ProcessMode);

