extends "res://addons/gut/test.gd"


func test_create_local_instance():
	var rng = Random2D.new_instance()
	Random2D.seed = hash("Goost")
	rng.seed = 37
	assert_has_method(Random2D, "point_in_circle")
	assert_ne(rng, Random2D, "The new local instance should not override the global one")
	assert_ne(rng.seed, Random2D.seed)


func test_singleton():
	Random2D.randomize()
	Random2D.seed = 37
	for x in 100:
		var f = Random2D.randf()
		assert_true(f >= 0.0)
		assert_true(f <= 1.0)
	for x in 100:
		var i = Random2D.randi() % 100
		assert_true(i >= 0)
		assert_true(i <= 99)


func test_direction():
	for i in 100:
		var point = Random2D.direction
		assert_almost_eq(point.length(), 1.0, 0.000001)
		assert_true(point.is_normalized())


func test_rotation():
	for i in 100:
		var rotation = Random2D.rotation
		assert_true(rotation >= 0.0)
		assert_true(rotation <= TAU)


func test_unit_circle_point():
	for i in 100:
		var point = Random2D.point_in_circle(1.0, 1.0)
		assert_almost_eq(point.length(), 1.0, 0.000001)
		assert_true(point.is_normalized())


func test_point_inside_unit_circle():
	for i in 100:
		var point = Random2D.point_in_circle(0.0, 1.0)
		assert_gt(point.length(), 0.0)
		assert_lt(point.length(), 1.0)


func test_point_within_circle_range():
	for i in 100:
		var point = Random2D.point_in_circle(0.5, 1.0)
		assert_gt(point.length(), 0.5)
		assert_lt(point.length(), 1.0)


func test_point_in_region():
	var rect = Rect2(Vector2(-100, -100), Vector2(200, 200))
	for i in 100:
		var point = Random2D.point_in_region(rect)
		assert_true(rect.has_point(point))


func test_point_in_triangle():
	var triangle = [Vector2(0, 0), Vector2(100, 0), Vector2(100, 100)]
	for i in 100:
		var point = Random2D.point_in_triangle(triangle)
		assert_true(GoostGeometry2D.point_in_polygon(point, triangle) as bool)


func test_point_in_polygon():
	var polygon = PoolVector2Array([Vector2(0, 0), Vector2(100, 0), Vector2(100, 100), Vector2(50, 50), Vector2(10, 20)])
	for p in 100:
		var point = Random2D.point_in_polygon(polygon)
		assert_true(GoostGeometry2D.point_in_polygon(point, polygon) as bool)


func test_point_in_polygon_array():
	var polygon = PoolVector2Array([Vector2(0, 0), Vector2(100, 0), Vector2(100, 100), Vector2(50, 50), Vector2(10, 20)])
	var points = Random2D.point_in_polygon(polygon, 100)
	for p in points:
		assert_true(GoostGeometry2D.point_in_polygon(p, polygon) as bool)
