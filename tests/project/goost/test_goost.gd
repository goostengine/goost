extends "res://addons/gut/test.gd"

func test_goost():
	var goost = GoostEngine
	assert_true(goost is Object)


func test_get_color_constants():
	var colors = GoostEngine.get_color_constants()
	assert_true(not colors.empty())
	for name in colors:
		gut.p(name)
		assert_typeof(name, TYPE_STRING)
		var color = colors[name]
		gut.p(color)
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
