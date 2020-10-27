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
		assert_lt(f, 1.0)
		assert_gt(f, 0.0)
	for x in 100:
		var i = Random.randi() % 100
		assert_lt(i, 99)
		assert_gt(i, 0)
