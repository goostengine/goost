extends "res://addons/gut/test.gd"

const SIZE = 50.0

var base_poly = PoolVector2Array([Vector2(-1, -1), Vector2(1, -1), Vector2(1, 1), Vector2(-1, 1)])

var poly_a = Transform2D(0, Vector2.ONE).scaled(Vector2.ONE * SIZE).xform(base_poly)
var poly_b = Transform2D(0, Vector2.ONE * SIZE).xform(poly_a)
var poly_c = Transform2D(0, Vector2.ONE * SIZE).xform(poly_b)

var solution = []


func test_create_local_instance():
	var global = PolyOffset2D
	var local = PolyOffset2D.new_instance()
	assert_ne(local, PolyOffset2D)
	assert_eq(global, PolyOffset2D)
	# Should be possible to override in local instance, but not in global.
#	PolyOffset2D.parameters.miter_limit = 3.0
	local.parameters.miter_limit = 3.0


func test_inflate_polygons():
	solution = PolyOffset2D.inflate_polygons([poly_a, poly_c], SIZE / 2.0)
	assert_eq(solution.size(), 2)
	assert_eq(solution[0].size(), 4)
	assert_eq(solution[1].size(), 4)


func test_deflate_polygons():
	solution = PolyOffset2D.deflate_polygons([poly_a, poly_c], SIZE / 2.0)
	assert_eq(solution.size(), 1) # Successfully merged together.
	assert_eq(solution[0].size(), 14)


func test_deflate_polylines():
	var polyofs = PolyOffset2D.new_instance()
	solution = polyofs.deflate_polylines([poly_a, poly_c], SIZE / 2.0)
	assert_ne(polyofs.parameters.end_type, PolyOffsetParameters2D.END_POLYGON)
	assert_eq(solution.size(), 1) # Successfully merged together.
	assert_eq(solution[0].size(), 17)
