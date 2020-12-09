extends "res://addons/gut/test.gd"

const gen = GoostGeometry2D

var n: PolyNode2D

# Outer object.
var outer_a = gen.regular_polygon(8, 256)
var inner_a = gen.regular_polygon(8, 128)

# Nested inner object.
var outer_b = gen.regular_polygon(8, 64)
var inner_b = gen.regular_polygon(8, 32)


func before_all():
	inner_a.invert() # hole
	inner_b.invert() # hole


func before_each():
	n = PolyNode2D.new()


func after_each():
	if is_instance_valid(n):
		n.free()
		n = null


func test_make_from_outlines_nested():
	n.make_from_outlines([outer_a, inner_a, outer_b, inner_b])
	assert_true(not Geometry.is_polygon_clockwise(n.points), "Should be boundary.")
	assert_true(Geometry.is_polygon_clockwise(n.get_child(0).points), "Should be hole.")
	assert_eq(n.get_child(0).operation, PolyNode2D.OP_DIFFERENCE)


func test_clip():
	add_child(n)
	var subject = outer_a
	var clip = Transform2D(0.0, Vector2(256, 0)).xform(outer_b)
	n.points = subject
	var c = n.new_child(clip)
	c.operation = PolyNode2D.OP_DIFFERENCE
	var _err = n.connect("outlines_updated", self, "_on_clip_outlines_updated", [], CONNECT_ONESHOT)


func _on_clip_outlines_updated():
	var outlines = n.get_outlines()
	assert_false(outlines.empty())
	assert_eq(outlines[0].size(), 12)
	remove_child(n)


func test_is_hole_empty():
	assert_true(n.is_inner())	


func test_is_hole_outer():
	n.make_from_outlines([outer_a])
	assert_false(n.is_inner())


func test_is_hole_outer_inner():
	n.make_from_outlines([outer_a, inner_a])
	assert_false(n.is_inner())
	assert_true(n.get_child(0).is_inner())


func test_is_hole_outer_inner_outer():
	n.make_from_outlines([outer_a, inner_a, outer_b])
	assert_false(n.is_inner())
	assert_true(n.get_child(0).is_inner())
	assert_false(n.get_child(0).get_child(0).is_inner())


func test_new_child():
	var child = n.new_child(outer_a)
	assert_not_null(child)
	assert_eq(child.points[0], outer_a[0])
