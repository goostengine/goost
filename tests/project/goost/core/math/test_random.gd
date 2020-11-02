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
		assert_true(i <= 10)
		assert_true(i >= 5)
	for x in 100:
		var f = Random.range(5.0, 10.0)
		assert_typeof(f, TYPE_REAL)
		assert_true(f <= 10.0)
		assert_true(f >= 5.0)


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
