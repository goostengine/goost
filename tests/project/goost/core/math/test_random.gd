extends "res://addons/gut/test.gd"


func test_create_local_instance():
	var rng = Random.new_instance()
	Random.seed = hash("Goost")
	rng.seed = 37
	assert_ne(rng, Random, "The new local instance should not override the global one")
	assert_ne(rng.seed, Random.seed)
	assert_lt(rng.randf(), 1.0)


func test_singleton():
	Random.randomize()
	Random.seed = 37
	for x in 100:
		var f = Random.randf()
		assert_true(f >= 0.0)
		assert_true(f <= 1.0)
	for x in 100:
		var i = Random.randi() % 100
		assert_true(i >= 0)
		assert_true(i <= 99)


func test_range():
	for x in 100:
		var i = Random.range(5, 10)
		assert_typeof(i, TYPE_INT)
		assert_true(i >= 5)
		assert_true(i <= 10)
	for x in 100:
		var f = Random.range(5.0, 10.0)
		assert_typeof(f, TYPE_REAL)
		assert_true(f >= 5.0)
		assert_true(f <= 10.0)
	# Interpolation.
	for x in 100:
		var point = Random.range(Vector2(0, 0), Vector2(100, 0))
		assert_true(point.x >= 0.0)
		assert_true(point.x <= 100.0)


func test_value():
	for x in 100:
		var f = Random.value
		assert_typeof(f, TYPE_REAL)
		assert_true(f >= 0.0)
		assert_true(f <= 1.0)


func test_number():
	for x in 100:
		var i = randi() % 100
		assert_typeof(i, TYPE_INT)
		assert_true(i >= 0)
		assert_true(i <= 99)


func test_color():
	for x in 100:
		var c = Random.color
		assert_typeof(c, TYPE_COLOR)
		assert_true(c.r >= 0.0)
		assert_true(c.r <= 1.0)
		assert_true(c.g >= 0.0)
		assert_true(c.g <= 1.0)
		assert_true(c.b >= 0.0)
		assert_true(c.b <= 1.0)
		assert_true(c.a == 1.0)


func test_color_rgb():
	for x in 100:
		var c = Random.color_rgb()
		assert_typeof(c, TYPE_COLOR)
		assert_true(c.r >= 0.0)
		assert_true(c.r <= 1.0)
		assert_true(c.g >= 0.0)
		assert_true(c.g <= 1.0)
		assert_true(c.b >= 0.0)
		assert_true(c.b <= 1.0)
		assert_true(c.a == 1.0)


func test_color_hsv():
	for x in 100:
		var c = Random.color_hsv()
		assert_typeof(c, TYPE_COLOR)
		assert_true(c.h >= 0.0)
		assert_true(c.h <= 1.0)
		assert_true(c.s >= 0.0)
		assert_true(c.s <= 1.0)
		assert_true(c.v >= 0.0)
		assert_true(c.v <= 1.0)
		assert_true(c.a == 1.0)


func test_condition():
	var false_count = 0
	var true_count = 0
	for x in 100000:
		if Random.condition:
			true_count += 1
		else:
			false_count += 1
	assert_almost_eq(false_count / float(true_count), 1.0, 0.05)


func test_decision_high():
	var hit_count = 0
	for x in 100000:
		if Random.decision(0.7):
			hit_count += 1
	assert_almost_eq(hit_count / 100000.0, 0.7, 0.05)


func test_decision_low():
	var hit_count = 0
	for x in 100000:
		if Random.decision(0.1):
			hit_count += 1
	assert_almost_eq(hit_count / 100000.0, 0.1, 0.05)


func test_randi_range_unbiased():
	var zero_count = 0
	var one_count = 0
	for x in 10000:
		var i = Random.randi_range(0, 1)
		if i == 0:
			zero_count += 1
		if i == 1:
			one_count += 1
	assert_almost_eq(zero_count / float(one_count), 1.0, 0.1)


func test_choice():
	var rng = Random.new_instance()

	rng.seed = 58885
	var element = rng.choice(["Godot", Color.blue, "Goost", Color.red])
	assert_eq(element, "Goost")

	rng.seed = 222
	var ch = rng.choice("Goost")
	assert_eq(ch, "G")

	Engine.print_error_messages = false

	assert_null(rng.choice(""))
	assert_null(rng.choice([]))

	Engine.print_error_messages = true


func test_shuffle_array():
	var rng = Random.new_instance()

	rng.seed = 100
	var array = [
		"Godot",
		37,
		"Goost",
		Color.red,
		Vector2.UP
	]
	rng.shuffle(array)
	assert_eq(array[0], Color.red)
	assert_eq(array[1], "Godot")
	assert_eq(array[2], "Goost")
	assert_eq(array[3], 37)
	assert_eq(array[4], Vector2.UP)

	rng.shuffle(array)
	assert_eq(array[0], "Goost")
	assert_eq(array[1], Vector2.UP)
	assert_eq(array[2], Color.red)
	assert_eq(array[3], 37)
	assert_eq(array[4], "Godot")
