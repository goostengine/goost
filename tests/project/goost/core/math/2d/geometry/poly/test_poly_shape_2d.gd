extends "res://addons/gut/test.gd"


func test_collision_shape():
	var body = StaticBody2D.new()
	add_child_autofree(body)

	var shape = PolyCollisionShape2D.new()
	shape.build_mode = PolyCollisionShape2D.BUILD_CONVEX

	var circle = PolyCircle2D.new()
	shape.add_child(circle)

	body.add_child(shape)
	yield(shape, "shapes_applied")

	var body_shape = body.shape_owner_get_shape(0, 0)
	assert_not_null(body_shape)

	var shape_count = body.shape_owner_get_shape_count(0)
	assert_eq(shape_count, 1)
