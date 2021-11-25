extends "res://addons/gut/test.gd"


func test_is_equal_approx():
	assert_true(GoostMath.is_equal_approx(0.05, 0.050001))
	

func test_is_zero_approx():
	assert_true(GoostMath.is_zero_approx(0.0000001))


func test_catmull_rom():
	var r = GoostMath.catmull_rom(0, 1, 2, 3, 0.5)
	assert_eq(r, 1.5)


func test_bezier():
	var r = GoostMath.bezier(10, 0.1, -0.1, 20, 0.5)
	assert_eq(r, 3.75)
