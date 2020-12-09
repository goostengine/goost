extends "res://addons/gut/test.gd"


func test_circle():
	var n = PolyCircle2D.new()
	add_child(n)
	n.radius = 128

	var outlines = n.build_outlines()
	assert_false(outlines.empty())

	var circle = outlines[0]
	var outlines_size = circle.size()
	assert_ne(outlines_size, 26)
	assert_eq(outlines_size, 51)
	
	assert_true(GoostGeometry2D.point_in_polygon(Vector2(n.radius - 1, 0), circle))

	n.queue_free()


func test_rectangle():
	var n = PolyRectangle2D.new()
	add_child(n)
	n.extents = Vector2(128, 128)

	var outlines = n.build_outlines()
	assert_false(outlines.empty())

	var rectangle = outlines[0]
	var outlines_size = rectangle.size()
	assert_eq(outlines_size, 4)

	assert_true(GoostGeometry2D.point_in_polygon(Vector2(128, 0), rectangle))

	n.queue_free()
