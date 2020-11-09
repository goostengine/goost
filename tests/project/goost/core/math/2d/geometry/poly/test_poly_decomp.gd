extends "res://addons/gut/test.gd"

const SIZE = 50.0

var base_poly = PoolVector2Array([Vector2(-1, -1), Vector2(1, -1), Vector2(1, 1), Vector2(-1, 1)])
var poly = Transform2D(0, Vector2.ONE * SIZE).xform(base_poly)
var poly_boundary = GoostGeometry2D.regular_polygon(8, SIZE * 2)
var poly_hole = GoostGeometry2D.regular_polygon(4, SIZE)

var solution = []


func before_all():
	poly_hole.invert()


func test_create_local_instance():
	var global = PolyDecomp2D
	var local = PolyDecomp2D.new_instance()
	assert_ne(local, PolyDecomp2D)
	assert_eq(global, PolyDecomp2D)
	# Should be possible to override in local instance, but not in global.
#	PolyDecomp2D.parameters.fill_rule = PolyDecompParameters2D.FILL_RULE_EVEN_ODD
	local.parameters.fill_rule = PolyDecompParameters2D.FILL_RULE_EVEN_ODD


func test_triangulate_polygons():
	solution = PolyDecomp2D.triangulate_polygons([poly_boundary, poly_hole])
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


func test_decompose_polygons_into_convex():
	solution = PolyDecomp2D.decompose_polygons_into_convex([poly_boundary, poly_hole, poly])
	assert_eq(solution.size(), 6)
	assert_eq(solution[0].size(), 4)
	assert_eq(solution[1].size(), 4)
	assert_eq(solution[2].size(), 4)
	assert_eq(solution[3].size(), 5)
	assert_eq(solution[4].size(), 5)
	assert_eq(solution[5].size(), 4)


func test_decompose_polygons_triangles_opt():
	solution = PolyDecomp2D.decompose_polygons([poly_boundary], PolyDecomp2D.DECOMP_TRIANGLES_OPT)
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

	solution = PolyDecomp2D.decompose_polygons([poly_boundary, poly_hole], PolyDecomp2D.DECOMP_TRIANGLES_MONO)
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
	solution = PolyDecomp2D.decompose_polygons([poly_boundary], PolyDecomp2D.DECOMP_CONVEX_OPT)
	assert_eq(solution.size(), 1)
	assert_eq(solution[0].size(), 8)
