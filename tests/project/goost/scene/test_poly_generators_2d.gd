extends "res://addons/gut/test.gd"


func test_circle():
	var n = PolyCircle2D.new()
	add_child_autofree(n)
	n.radius = 128

	var outlines = n.build_outlines()
	assert_false(outlines.empty())

	var circle = outlines[0]
	var outlines_size = circle.size()
	assert_ne(outlines_size, 26)
	assert_eq(outlines_size, 51)
	
	assert_true(GoostGeometry2D.point_in_polygon(Vector2(n.radius - 1, 0), circle) as bool)

	n.queue_free()


func test_capsule():
	var n = PolyCapsule2D.new()
	add_child_autofree(n)
	n.radius = 128
	n.height = 256

	var outlines = n.build_outlines()
	assert_false(outlines.empty())

	var capsule = outlines[0]

	var outlines_size = capsule.size()
	assert_eq(outlines_size, 54)

	var points = []
	for p in capsule:
		assert_false(points.has(p), "Capsule polygon has duplicate points.")
		points.push_back(p)

	assert_true(GoostGeometry2D.point_in_polygon(Vector2(-73, -229.5), capsule) as bool)
	assert_true(GoostGeometry2D.point_in_polygon(Vector2(0, -240), capsule) as bool)
	assert_true(GoostGeometry2D.point_in_polygon(Vector2(0, 240), capsule) as bool)

	n.queue_free()


func test_rectangle():
	var n = PolyRectangle2D.new()
	add_child_autofree(n)
	n.extents = Vector2(128, 128)

	var outlines = n.build_outlines()
	assert_false(outlines.empty())

	var rectangle = outlines[0]
	var outlines_size = rectangle.size()
	assert_eq(outlines_size, 4)

	assert_true(GoostGeometry2D.point_in_polygon(Vector2(128, 0), rectangle) as bool)

	n.queue_free()


func test_poly_path():
	var np = Path2D.new()
	var c = np.get_curve()
	c.add_point(Vector2(0, 0))
	c.add_point(Vector2(100, 100))

	var n = PolyPath2D.new()
	n.add_child(np)
	add_child_autofree(n)

	n.buffer_offset = 32.0
	var params = PolyOffsetParameters2D.new()
	params.join_type = PolyOffsetParameters2D.JOIN_ROUND
	params.end_type = PolyOffsetParameters2D.END_ROUND
	n.buffer_parameters = params

	var outlines = n.build_outlines()
	assert_false(outlines.empty())

	var deflated = outlines[0]
	var control = [
		Vector2(-31, 0),
		Vector2(0, 0),
		Vector2(0, -31),
		Vector2(100 - 31, 100),
		Vector2(100, 100),
		Vector2(100, 100 - 31),
	]
	for p in control:
		assert_true(GoostGeometry2D.point_in_polygon(p, deflated) as bool)
