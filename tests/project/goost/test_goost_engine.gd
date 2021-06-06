extends "res://addons/gut/test.gd"

func test_goost():
	var goost = GoostEngine
	assert_true(goost is Object)
	
	
func test_get_version_info():
	var v = GoostEngine.get_version_info()
	gut.p(v)
	assert_true(v.has("major"))
	assert_typeof(v.major, TYPE_INT)
	assert_true(v.has("minor"))
	assert_typeof(v.minor, TYPE_INT)
	assert_true(v.has("patch"))
	assert_typeof(v.patch, TYPE_INT)
	assert_true(v.has("hex"))
	assert_typeof(v.hex, TYPE_INT)
	assert_true(v.has("status"))
	assert_typeof(v.status, TYPE_STRING)
	assert_true(v.has("year"))
	assert_typeof(v.year, TYPE_INT)
	assert_true(v.has("hash"))
	assert_typeof(v.hash, TYPE_STRING)

	assert_false(v.has("build"), "This should not be defined, as it relates to the engine.")


func test_get_author_info():
	var author = GoostEngine.get_author_info()
	assert_typeof(author, TYPE_DICTIONARY)


func test_get_copyright_info():
	var copyright = GoostEngine.get_copyright_info()
	assert_typeof(copyright, TYPE_ARRAY)


func test_get_license_info():
	var license = GoostEngine.get_license_info()
	assert_typeof(license, TYPE_DICTIONARY)


func test_get_license_text():
	var license_text = GoostEngine.get_license_text()
	assert_typeof(license_text, TYPE_STRING)


func test_get_color_constants():
	var colors = GoostEngine.get_color_constants()
	assert_true(not colors.empty())
	for name in colors:
		assert_typeof(name, TYPE_STRING)
		var color = colors[name]
		assert_typeof(color, TYPE_COLOR)


class Counter:
	var counter = 0
	func inc():
		counter += 1
	func inc_custom(arg1, arg2=0, arg3=0, arg4=0, arg5=0):
		counter += arg1
		counter += arg2
		counter += arg3
		counter += arg4
		counter += arg5


func test_defer_call():
	var m = Counter.new()
	assert_eq(m.counter, 0)

	for i in 100:
		GoostEngine.defer_call(m, "inc")

	assert_eq(m.counter, 0)
	yield(get_tree(), "physics_frame") # Make it flush calls.
	assert_eq(m.counter, 100)


func test_defer_call_unique():
	var m = Counter.new()
	assert_eq(m.counter, 0)

	GoostEngine.defer_call_unique(m, "inc")
	yield(get_tree(), "physics_frame") # Make it flush calls.
	assert_eq(m.counter, 1)

	for i in 100:
		GoostEngine.defer_call_unique(m, "inc")
	yield(get_tree(), "physics_frame") # Make it flush calls.
	assert_eq(m.counter, 2)
	assert_ne(m.counter, 101)

	GoostEngine.defer_call_unique(m, "inc")
	GoostEngine.defer_call_unique(m, "inc_custom", 10)
	GoostEngine.defer_call_unique(m, "inc_custom", 0, 10)
	GoostEngine.defer_call_unique(m, "inc_custom", 0, 10) # Not unique!
	GoostEngine.defer_call_unique(m, "inc_custom", 10) # Not unique!
	GoostEngine.defer_call_unique(m, "inc_custom", 1, 1, 1, 1, 1)
	GoostEngine.defer_call_unique(m, "inc_custom", 1, 1, 1, 1, 1) # Not unique!
	GoostEngine.defer_call_unique(m, "inc_custom", 1, 1, 1, 1)
	GoostEngine.defer_call_unique(m, "inc") # Not unique!
	yield(get_tree(), "physics_frame") # Make it flush calls.
	assert_eq(m.counter, 2 + 1 + 10 + 10 + 0 + 0 + 5 + 0 + 4 + 0) # In order above.


func test_invoke_delay():
	var m = Counter.new()
	assert_eq(m.counter, 0)

	var state: InvokeState = GoostEngine.invoke(m, "inc", 0.5)
	assert_eq(state.target, m)
	assert_eq(state.method, "inc")
	assert_true(state.is_active())
	assert_false(state.is_repeating())

	yield(get_tree().create_timer(0.25), "timeout")
	assert_eq(m.counter, 0)
	assert_almost_eq(state.time_left, 0.25, get_process_delta_time() * 2)
	yield(get_tree().create_timer(0.25), "timeout")
	assert_eq(m.counter, 1)

	state = GoostEngine.invoke_deferred(m, "inc", 0.25)
	# Since it's deferred, it may not take exactly 0.25 seconds.
	yield(state, "completed")
	assert_eq(m.counter, 2)


func test_invoke_repeating():
	var m = Counter.new()
	assert_eq(m.counter, 0)

	var state: InvokeState = GoostEngine.invoke(m, "inc", 0.5, 0.1)
	assert_true(state.is_repeating())
	# 0.5 for delay, 0.5 for total time.
	yield(get_tree().create_timer(0.5 + 0.5), "timeout")
	assert_eq(m.counter, 5)

	yield(state, "pre_call")
	assert_eq(m.counter, 5)

	for i in 5:
		yield(state, "post_call")
	assert_eq(m.counter, 10)

	state.cancel()

	assert_not_null(state)
	assert_false(state.is_active())
	assert_false(GoostEngine.get_invokes().empty())

	yield(get_tree(), "idle_frame")

	assert_not_null(state) # We still hold a reference inside test script.
	assert_true(GoostEngine.get_invokes().empty())

	state = GoostEngine.invoke_deferred(m, "inc", 0.5, 0.1)
	assert_eq(state, GoostEngine.get_invokes().back())
	yield(get_tree().create_timer(0.5 + 0.5), "timeout")
	assert_eq(m.counter, 15)
	state.cancel()
