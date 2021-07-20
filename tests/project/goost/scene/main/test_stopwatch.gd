extends "res://addons/gut/test.gd"


func test_start_stop_idle():
	var stopwatch = Stopwatch.new()
	add_child_autofree(stopwatch)

	yield(get_tree().create_timer(0.1), "timeout")

	assert_eq(stopwatch.time_elapsed, 0.0, "Should not autostart.")

	stopwatch.start()
	yield(get_tree().create_timer(0.1), "timeout")

	assert_true(stopwatch.is_running(), "Should still be running until stop() is called.")
	assert_gt(stopwatch.time_elapsed, 0.09, "Should start.")

	stopwatch.stop()
	yield(get_tree().create_timer(0.1), "timeout")

	assert_true(stopwatch.is_stopped(), "Should be stopped by now.")
	assert_gt(stopwatch.time_elapsed, 0.09, "Should stop measuring time interval.")

	stopwatch.start()
	assert_gt(stopwatch.time_elapsed, 0.09, "Should resume, not reset.")
	stopwatch.stop()

	stopwatch.reset()
	yield(get_tree().create_timer(0.1), "timeout")

	assert_eq(stopwatch.time_elapsed, 0.0, "Should reset.")


func test_autostart_stop_physics():
	var stopwatch = Stopwatch.new()
	stopwatch.autostart = true
	stopwatch.process_mode = Stopwatch.PROCESS_PHYSICS
	add_child_autofree(stopwatch)

	yield(get_tree().create_timer(0.25), "timeout")

	# Yielding seems to produce unreliable results in physics process
	# (lacks a frame), so we just increase allowed error here.
	assert_gt(stopwatch.time_elapsed, 0.15, "Should autostart.")

	stopwatch.stop()
	yield(get_tree().create_timer(0.25), "timeout")

	assert_gt(stopwatch.time_elapsed, 0.15, "Should stop measuring time interval.")

	stopwatch.reset()
	yield(get_tree().create_timer(0.25), "timeout")

	assert_eq(stopwatch.time_elapsed, 0.0, "Should reset.")


func test_measured_signal():
	var stopwatch = Stopwatch.new()
	add_child_autofree(stopwatch)
	watch_signals(stopwatch)

	stopwatch.start()
	stopwatch.stop()

	assert_signal_not_emitted(stopwatch, "interval_measured")

	stopwatch.start()
	yield(get_tree().create_timer(0.2), "timeout")
	stopwatch.stop()

	assert_signal_emitted(stopwatch, "interval_measured")
