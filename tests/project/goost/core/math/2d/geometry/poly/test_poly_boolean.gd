extends "res://addons/gut/test.gd"

const SIZE = 50.0

var base_poly = PoolVector2Array([Vector2(-1, -1), Vector2(1, -1), Vector2(1, 1), Vector2(-1, 1)])

var poly_a = Transform2D(0, Vector2.ONE).scaled(Vector2.ONE * SIZE).xform(base_poly)
var poly_b = Transform2D(0, Vector2.ONE * SIZE).xform(poly_a)
var poly_c = Transform2D(0, Vector2.ONE * SIZE).xform(poly_b)
var poly_d = Transform2D(0, Vector2.ONE * SIZE).xform(poly_c)

var solution = []


func test_merge_polygons():
	solution = PolyBoolean2D.merge_polygons([poly_a, poly_b, poly_c, poly_d])
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 16)


func test_clip_polygons():
	solution = PolyBoolean2D.clip_polygons([poly_a, poly_b], [poly_c, poly_d])
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 10)


func test_intersect_polygons():
	solution = PolyBoolean2D.intersect_polygons([poly_a, poly_c], [poly_b, poly_d])
	assert_eq(solution.size(), 3)
	assert_eq(solution[0].size(), 4)
	assert_eq(solution[1].size(), 4)
	assert_eq(solution[2].size(), 4)


func test_exclude_polygons():
	solution = PolyBoolean2D.exclude_polygons([poly_a, poly_b], [poly_c, poly_d])
	assert_eq(solution.size(), 2)
	assert_eq(solution[0].size(), 10)
	assert_eq(solution[1].size(), 10)


func test_boolean_polygons():
	solution = PolyBoolean2D.boolean_polygons([poly_a, poly_b], [poly_c, poly_d], PolyBoolean2D.OPERATION_UNION)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 16)


func test_boolean_polygons_tree():
	var a = GoostGeometry2D.regular_polygon(4, 150)
	var b = GoostGeometry2D.regular_polygon(4, 100)
	var clip = GoostGeometry2D.clip_polygons(a, b)
	var c = GoostGeometry2D.regular_polygon(4, 50)
	solution = PolyBoolean2D.boolean_polygons_tree(clip, [c], PolyBoolean2D.OPERATION_UNION)
	var inner = solution.get_child(0).get_child(0).get_child(0).path
	assert_eq(inner.size(), c.size())


func test_clip_polylines_with_polygons():
	solution = PolyBoolean2D.clip_polylines_with_polygons([poly_a, poly_c], [poly_b, poly_d])
	assert_eq(solution.size(), 4)
	assert_eq(solution[0].size(), 2)
	assert_eq(solution[1].size(), 3)
	assert_eq(solution[2].size(), 2)
	assert_eq(solution[3].size(), 3)


func test_intersect_polylines_with_polygons():
	solution = PolyBoolean2D.intersect_polylines_with_polygons([poly_a, poly_c], [poly_b, poly_d])
	assert_eq(solution.size(), 3)
	assert_eq(solution[0].size(), 3)
	assert_eq(solution[1].size(), 2)
	assert_eq(solution[2].size(), 3)
