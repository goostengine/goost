extends "res://addons/gut/test.gd"

var testbed

const SIZE = 50.0

var base_poly = PoolVector2Array([Vector2(-1, -1), Vector2(1, -1), Vector2(1, 1), Vector2(-1, 1)])

var poly_a = Transform2D(0, Vector2.ONE).scaled(Vector2.ONE * SIZE).xform(base_poly)
var poly_b = Transform2D(0, Vector2.ONE * SIZE).xform(poly_a)
var poly_c = Transform2D(0, Vector2.ONE * SIZE).xform(poly_b)
var poly_d = Transform2D(0, Vector2.ONE * SIZE).xform(poly_c)

var poly_boundary = GoostGeometry2D.regular_polygon(8, SIZE * 2)
var poly_hole = GoostGeometry2D.regular_polygon(4, SIZE)

var solution = []


func before_all():
	poly_hole.invert()


func setup():
	testbed = preload("geometry_2d_testbed.tscn").instance()
	get_tree().get_root().call_deferred("add_child", testbed)


func after_each():
	testbed.solution = solution
	testbed.test = gut._current_test.name
	testbed.update()


func teardown():
	testbed.queue_free()


func test_merge_polygons():
	solution = GoostGeometry2D.merge_polygons(poly_a, poly_b)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 8)


func test_clip_polygons():
	solution = GoostGeometry2D.clip_polygons(poly_a, poly_b)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 6)


func test_intersect_polygons():
	solution = GoostGeometry2D.intersect_polygons(poly_a, poly_b)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 4)


func test_exclude_polygons():
	solution = GoostGeometry2D.exclude_polygons(poly_a, poly_b)
	assert_eq(solution.size(), 2)
	assert_eq(solution[0].size(), 6)
	assert_eq(solution[1].size(), 6)


func test_merge_multiple_polygons():
	solution = GoostGeometry2D.merge_multiple_polygons([poly_a, poly_b, poly_c, poly_d])
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 16)


func test_clip_multiple_polygons():
	solution = GoostGeometry2D.clip_multiple_polygons([poly_a, poly_b], [poly_c, poly_d])
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 10)


func test_intersect_multiple_polygons():
	solution = GoostGeometry2D.intersect_multiple_polygons([poly_a, poly_c], [poly_b, poly_d])
	assert_eq(solution.size(), 3)
	assert_eq(solution[0].size(), 4)
	assert_eq(solution[1].size(), 4)
	assert_eq(solution[2].size(), 4)


func test_exclude_multiple_polygons():
	solution = GoostGeometry2D.exclude_multiple_polygons([poly_a, poly_b], [poly_c, poly_d])
	assert_eq(solution.size(), 2)
	assert_eq(solution[0].size(), 10)
	assert_eq(solution[1].size(), 10)


func test_polygons_boolean():
	solution = GoostGeometry2D.polygons_boolean(GoostGeometry2D.OPERATION_UNION, [poly_a, poly_b, poly_c, poly_d])
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 16)


func test_polygons_boolean_tree():
	var a = GoostGeometry2D.regular_polygon(4, 150)
	var b = GoostGeometry2D.regular_polygon(4, 100)
	var clip = GoostGeometry2D.clip_polygons(a, b)
	var c = GoostGeometry2D.regular_polygon(4, 50)
	solution = GoostGeometry2D.polygons_boolean_tree(GoostGeometry2D.OPERATION_UNION, clip, [c])
	var inner = solution.get_child(0).get_child(0).get_child(0).path
	assert_eq(inner.size(), c.size())


func test_clip_polyline_with_polygon():
	solution = GoostGeometry2D.clip_polyline_with_polygon(poly_a, poly_b)
	assert_eq(solution.size(), 2)
	assert_eq(solution[0].size(), 2)
	assert_eq(solution[1].size(), 3)


func test_intersect_polyline_with_polygon():
	solution = GoostGeometry2D.intersect_polyline_with_polygon(poly_a, poly_b)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 3)


func test_clip_multiple_polylines_with_polygons():
	solution = GoostGeometry2D.clip_multiple_polylines_with_polygons([poly_a, poly_c], [poly_b, poly_d])
	assert_eq(solution.size(), 4)
	assert_eq(solution[0].size(), 2)
	assert_eq(solution[1].size(), 3)
	assert_eq(solution[2].size(), 2)
	assert_eq(solution[3].size(), 3)


func test_intersect_multiple_polylines_with_polygons():
	solution = GoostGeometry2D.intersect_multiple_polylines_with_polygons([poly_a, poly_c], [poly_b, poly_d])
	assert_eq(solution.size(), 3)
	assert_eq(solution[0].size(), 3)
	assert_eq(solution[1].size(), 2)
	assert_eq(solution[2].size(), 3)


func test_inflate_polygon():
	solution = GoostGeometry2D.inflate_polygon(poly_a, SIZE / 2.0)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 4)


func test_deflate_polygon():
	solution = GoostGeometry2D.deflate_polygon(poly_a, SIZE / 2.0)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 8)


func test_inflate_multiple_polygons():
	solution = GoostGeometry2D.inflate_multiple_polygons([poly_a, poly_c], SIZE / 2.0)
	assert_eq(solution.size(), 2)
	assert_eq(solution[0].size(), 4)
	assert_eq(solution[1].size(), 4)


func test_deflate_multiple_polygons():
	solution = GoostGeometry2D.deflate_multiple_polygons([poly_a, poly_c], SIZE / 2.0)
	assert_eq(solution.size(), 1) # Successfully merged together.
	assert_eq(solution[0].size(), 14)


func test_deflate_polyline():
	solution = GoostGeometry2D.deflate_polyline(poly_a, SIZE / 2.0)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 10)


func test_deflate_multiple_polylines():
	solution = GoostGeometry2D.deflate_multiple_polylines([poly_a, poly_c], SIZE / 2.0)
	assert_eq(solution.size(), 1) # Successfully merged together.
	assert_eq(solution[0].size(), 17)


func test_offset_polygon():
	var params = PolyOffsetParameters2D.new()
	params.join_type = PolyOffsetParameters2D.JOIN_ROUND
	params.end_type = PolyOffsetParameters2D.END_ROUND
	solution = GoostGeometry2D.offset_polygon(poly_a, SIZE / 2.0, params)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 28)


func test_offset_multiple_polygons():
	var params = PolyOffsetParameters2D.new()
	params.join_type = PolyOffsetParameters2D.JOIN_ROUND
	params.end_type = PolyOffsetParameters2D.END_ROUND
	solution = GoostGeometry2D.offset_multiple_polygons([poly_a, poly_c], SIZE / 2.0, params)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 44)


func test_triangulate_polygon():
	solution = GoostGeometry2D.triangulate_polygon(poly_boundary)
	assert_eq(solution.size(), 6)
	assert_eq(solution[0].size(), 3)
	assert_eq(solution[1].size(), 3)
	assert_eq(solution[2].size(), 3)
	assert_eq(solution[3].size(), 3)
	assert_eq(solution[4].size(), 3)
	assert_eq(solution[5].size(), 3)


func test_triangulate_multiple_polygons():
	solution = GoostGeometry2D.triangulate_multiple_polygons([poly_boundary, poly_hole])
	assert_eq(solution.size(), 12)
	assert_eq(solution[0].size(), 3)
	assert_eq(solution[1].size(), 3)
	assert_eq(solution[2].size(), 3)
	assert_eq(solution[3].size(), 3)
	assert_eq(solution[4].size(), 3)
	assert_eq(solution[5].size(), 3)
	assert_eq(solution[6].size(), 3)
	assert_eq(solution[7].size(), 3)
	assert_eq(solution[8].size(), 3)
	assert_eq(solution[9].size(), 3)
	assert_eq(solution[10].size(), 3)
	assert_eq(solution[11].size(), 3)


func test_decompose_polygon_into_convex():
	solution = GoostGeometry2D.decompose_polygon_into_convex(poly_boundary)
	assert_eq(solution.size(), 1)


func test_decompose_multiple_polygons_into_convex():
	solution = GoostGeometry2D.decompose_multiple_polygons_into_convex([poly_boundary, poly_hole, poly_c])
	assert_eq(solution.size(), 6)
	assert_eq(solution[0].size(), 4)
	assert_eq(solution[1].size(), 4)
	assert_eq(solution[2].size(), 4)
	assert_eq(solution[3].size(), 5)
	assert_eq(solution[4].size(), 5)
	assert_eq(solution[5].size(), 4)


func test_decompose_polygons_triangles_opt():
	solution = GoostGeometry2D.decompose_polygons(GoostGeometry2D.DECOMP_TRIANGLES_OPT, [poly_boundary])
	assert_eq(solution.size(), 6)
	assert_eq(solution[0].size(), 3)
	assert_eq(solution[1].size(), 3)
	assert_eq(solution[2].size(), 3)
	assert_eq(solution[3].size(), 3)
	assert_eq(solution[4].size(), 3)
	assert_eq(solution[5].size(), 3)


func test_decompose_polygons_triangles_mono():
	if ProjectSettings.get_setting("goost/geometry/2d/backends/poly_decomp") == "polypartition":
		push_error("Skip, internal bug in PolyPartition.Triangulate_MONO...")
		return true

	solution = GoostGeometry2D.decompose_polygons(GoostGeometry2D.DECOMP_TRIANGLES_MONO, [poly_boundary, poly_hole])
	assert_eq(solution.size(), 12)
	assert_eq(solution[0].size(), 3)
	assert_eq(solution[1].size(), 3)
	assert_eq(solution[2].size(), 3)
	assert_eq(solution[3].size(), 3)
	assert_eq(solution[4].size(), 3)
	assert_eq(solution[5].size(), 3)
	assert_eq(solution[6].size(), 3)
	assert_eq(solution[7].size(), 3)
	assert_eq(solution[8].size(), 3)
	assert_eq(solution[9].size(), 3)
	assert_eq(solution[10].size(), 3)
	assert_eq(solution[11].size(), 3)


func test_decompose_polygons_convex_opt():
	solution = GoostGeometry2D.decompose_polygons(GoostGeometry2D.DECOMP_CONVEX_OPT, [poly_boundary])
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 8)


func test_polygon_centroid():
	solution = GoostGeometry2D.polygon_centroid(poly_a)
	assert_eq(solution, Vector2(50, 50))


func test_polygon_perimeter():
	solution = GoostGeometry2D.polygon_perimeter(poly_a)
	assert_eq(solution, 400.0)


func test_polyline_length():
	solution = GoostGeometry2D.polyline_length(poly_a)
	assert_eq(solution, 300.0)


func test_point_in_polygon():
	solution = GoostGeometry2D.point_in_polygon(Vector2(50, 50), poly_a)
	assert_eq(solution, 1) # inside
	solution = GoostGeometry2D.point_in_polygon(Vector2(-50, 50), poly_a)
	assert_eq(solution, 0) # outside
	solution = GoostGeometry2D.point_in_polygon(Vector2(0, 50), poly_a)
	assert_eq(solution, -1) # exactly


func test_regular_polygon():
	solution = GoostGeometry2D.regular_polygon(64, SIZE)
	assert_eq(solution.size(), 64)


func test_circle():
	solution = GoostGeometry2D.circle(SIZE, 0.25)
	assert_eq(solution.size(), 32)
