tool
extends Node2D

export var test := ""
var solution = []


func _draw():
	draw_set_transform(get_viewport().size / 2.0, 0.0, Vector2.ONE)

	# Some specific methods which have to be drawn with `draw_polyline`.
	var polyline_test_methods = [
		"test_clip_polyline_with_polygon",
		"test_intersect_polyline_with_polygon",
		"test_clip_multiple_polylines_with_polygons",
		"test_intersect_multiple_polylines_with_polygons"
	]
	var polygons_only = not (test in polyline_test_methods)

	if polygons_only:
		match typeof(solution):
			TYPE_ARRAY:
				for poly in solution:
					draw_colored_polygon(poly, Color(randf(), randf(), randf()))
			TYPE_OBJECT:
				var c = Color.green
				if solution is PolyNode2D:
					var nodes = [solution]
					while not nodes.empty():
						var node = nodes.back()
						nodes.pop_back()

						if node.path.size() > 0:
							draw_colored_polygon(node.path, c)

						for child in node.get_children():
							nodes.push_back(child)

						c.v -= 0.25
	else: # polylines
		for poly in solution:
			draw_polyline(poly, Color(randf(), randf(), randf()), 2)
