extends "res://addons/gut/test.gd"


func test_collision_shape():
	var body = StaticBody2D.new()
	add_child_autofree(body)

	var shape = PolyCollisionShape2D.new()
	shape.build_mode = PolyCollisionShape2D.BUILD_CONVEX

	var circle = PolyCircle2D.new()
	shape.add_child(circle)

	body.add_child(shape)
	# yield_to(shape, "shapes_applied", 1)

	# var body_shape = body.shape_owner_get_shape(0, 0)
	# assert_not_null(body_shape)

	# shape.connect("shapes_applied", self, "_on_shapes_applied", [body], CONNECT_ONESHOT)


# func _on_shapes_applied(body):
# 	var body_shape = body.shape_owner_get_shape(0, 0)
# 	assert_not_null(body_shape)
	
# 	var shape_count = body.shape_owner_get_shape_count(0)
	# print(shape_count)
	# gut.p(shape_count)
