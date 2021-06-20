extends "res://addons/gut/test.gd"


func test_start_stop_idle():
	var stopwatch = Stopwatch.new()
	add_child_autofree(stopwatch)

	yield(get_tree().create_timer(0.1), "timeout")

	assert_eq(stopwatch.time_elapsed, 0.0, "Should not autostart.")

	stopwatch.start()
	yield(get_tree().create_timer(0.5), "timeout")

	assert_gt(stopwatch.time_elapsed, 0.49, "Should start.")

	stopwatch.stop()
	yield(get_tree().create_timer(0.5), "timeout")
	
	assert_gt(stopwatch.time_elapsed, 0.49, "Should stop.")

	stopwatch.start()
	assert_gt(stopwatch.time_elapsed, 0.49, "Should resume, not reset.")
	stopwatch.stop()

	stopwatch.reset()
	yield(get_tree().create_timer(0.5), "timeout")

	assert_eq(stopwatch.time_elapsed, 0.0, "Should reset.")


func test_autostart_stop_physics():
	var stopwatch = Stopwatch.new()
	stopwatch.autostart = true
	stopwatch.process_mode = Stopwatch.PROCESS_PHYSICS
	add_child_autofree(stopwatch)

	yield(get_tree().create_timer(0.5), "timeout")

	# Yielding seems to produce unreliable results in physics process
	# (lacks a frame), so we just increase allowed error here.
	assert_gt(stopwatch.time_elapsed, 0.35, "Should autostart.")

	stopwatch.stop()
	yield(get_tree().create_timer(0.5), "timeout")

	assert_gt(stopwatch.time_elapsed, 0.35, "Should stop.")

	stopwatch.reset()
	yield(get_tree().create_timer(0.5), "timeout")

	assert_eq(stopwatch.time_elapsed, 0.0, "Should reset.")


func test_measured_signal():
	var stopwatch = Stopwatch.new()
	add_child_autofree(stopwatch)
	watch_signals(stopwatch)

	stopwatch.start()
	stopwatch.stop()

	assert_signal_not_emitted(stopwatch, "interval_measured")

	stopwatch.start()
	yield(get_tree().create_timer(0.5), "timeout")
	stopwatch.stop()

	assert_signal_emitted(stopwatch, "interval_measured")
