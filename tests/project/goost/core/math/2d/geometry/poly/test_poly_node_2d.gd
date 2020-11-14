extends "res://addons/gut/test.gd"

const gen = GoostGeometry2D

var n: PolyNode2D

# Outer object.
var outer_a = gen.regular_polygon(8, 256)
var inner_a = gen.regular_polygon(8, 128)

# Nested inner object.
var outer_b = gen.regular_polygon(8, 64)
# var inner_b = gen.regular_polygon(8, 32)


func before_all():
	inner_a.invert() # hole
	# inner_b.invert() # hole


func before_each():
	n = PolyNode2D.new()


func after_each():
	if is_instance_valid(n):
		n.free()
		n = null


func test_is_hole_empty():
	assert_true(n.is_hole())	


func test_is_hole_outer():
	n.make_from_outlines([outer_a])
	assert_false(n.is_hole())


func test_is_hole_outer_inner():
	n.make_from_outlines([outer_a, inner_a])
	assert_false(n.is_hole())
	assert_true(n.get_child(0).is_hole())


func test_is_hole_outer_inner_outer():
	n.make_from_outlines([outer_a, inner_a, outer_b])
	assert_false(n.is_hole())
	assert_true(n.get_child(0).is_hole())
	assert_false(n.get_child(0).get_child(0).is_hole())
