extends "res://addons/gut/test.gd"


func test_is_equal_approx():
	assert_true(GoostMath.is_equal_approx(0.05, 0.050001))
	

func test_is_zero_approx():
	assert_true(GoostMath.is_zero_approx(0.0000001))


func test_is_between():
	var s = 37

	assert_true(GoostMath.is_between(s, 0, 100))
	assert_true(GoostMath.is_between(s, -100, 100), "Must handle negative values.")
	assert_true(GoostMath.is_between(s, 100, -100), "Min and max values should be determined automatically.")

	assert_false(GoostMath.is_between(s, 0, 10))
	assert_false(GoostMath.is_between(s, 10, 0), "Min and max values should be determined automatically.")

	assert_true(GoostMath.is_between(s, 37, 37), "If a and b are the same, then `s` should be considered in range.")


func test_is_in_range():
	var s = 37

	assert_true(GoostMath.is_in_range(s, 0, 100))
	assert_true(GoostMath.is_in_range(s, -100, 100), "Must handle negative values.")

	Engine.print_error_messages = false
	assert_false(GoostMath.is_in_range(s, 100, -100), "Invalid range should be detected.")
	Engine.print_error_messages = true

	assert_false(GoostMath.is_in_range(s, 0, 10))
	assert_true(GoostMath.is_in_range(s, 37, 37), "If `min` and `max` are the same, then `s` should be considered in range.")


func test_log():
	assert_almost_eq(GoostMath.log(0.99, 0.9), 0.095389964549, 0.000001)
	assert_almost_eq(GoostMath.log(10), 2.302585092994046, 0.000001, "ln(10)")
	assert_true(is_nan(GoostMath.log(-1, 10)))
	assert_true(is_nan(GoostMath.log(-1, 10)))

	assert_eq(GoostMath.log2(16), 4.0)
	assert_eq(GoostMath.log10(1000), 3.0)


func test_catmull_rom():
	var r = GoostMath.catmull_rom(0, 1, 2, 3, 0.5)
	assert_eq(r, 1.5)


func test_bezier():
	var r = GoostMath.bezier(10, 0.1, -0.1, 20, 0.5)
	assert_eq(r, 3.75)
