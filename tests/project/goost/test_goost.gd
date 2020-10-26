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
