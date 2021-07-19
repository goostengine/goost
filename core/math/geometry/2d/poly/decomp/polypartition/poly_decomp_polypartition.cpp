#include "poly_decomp_polypartition.h"
#include "goost/core/math/geometry/2d/goost_geometry_2d.h"
#include "thirdparty/misc/triangulator.h"

List<TriangulatorPoly> configure(PolyDecomp2DPolyPartition::Decomposition p_type, const Vector<Vector<Point2>> &p_polygons) {
	List<TriangulatorPoly> in_poly;

	// Split between inner and outer polygons first.
	Vector<int> outer_indices, inner_indices;
	for (int i = 0; i < p_polygons.size(); ++i) {
		if (GoostGeometry2D::polygon_area(p_polygons[i]) >= 0.0) {
			outer_indices.push_back(i);
		} else {
			if (p_type == PolyDecomp2DPolyPartition::DECOMP_TRIANGLES_OPT || p_type == PolyDecomp2DPolyPartition::DECOMP_CONVEX_OPT) {
				// The solution would no longer be optimal with the inner holes, skip.
				continue;
			}
			inner_indices.push_back(i);
		}
	}
	ERR_FAIL_COND_V_MSG(outer_indices.empty(), List<TriangulatorPoly>(), "No outer polygons detected.");

	// Setup outer polygons.
	for (int i = 0; i < outer_indices.size(); ++i) {
		TriangulatorPoly inp_outer;
		const Vector<Point2> &polygon_outer = p_polygons[outer_indices[i]];
		inp_outer.Init(polygon_outer.size());
		for (int j = 0; j < polygon_outer.size(); j++) {
			inp_outer.GetPoint(j) = polygon_outer[j];
		}
		in_poly.push_back(inp_outer);
	}
	// Setup inner polygons.
	for (int i = 0; i < inner_indices.size(); ++i) {
		TriangulatorPoly inp_inner;
		const Vector<Point2> &polygon_inner = p_polygons[inner_indices[i]];
		inp_inner.Init(polygon_inner.size());
		for (int j = 0; j < polygon_inner.size(); ++j) {
			inp_inner.GetPoint(j) = polygon_inner[j];
		}
		inp_inner.SetHole(true);
		in_poly.push_back(inp_inner);
	}
	return in_poly;
}

Vector<Vector<Point2>> partition(List<TriangulatorPoly> &p_out_poly) {
	Vector<Vector<Point2>> polys;
	polys.resize(p_out_poly.size());
	int idx = 0;
	for (List<TriangulatorPoly>::Element *E = p_out_poly.front(); E; E = E->next()) {
		TriangulatorPoly &tp = E->get();
		polys.write[idx].resize(tp.GetNumPoints());
		for (int64_t i = 0; i < tp.GetNumPoints(); i++) {
			polys.write[idx].write[i] = tp.GetPoint(i);
		}
		++idx;
	}
	return polys;
}

Vector<Vector<Point2>> PolyDecomp2DPolyPartition::triangulate_ec(const Vector<Vector<Point2>> &p_polygons) {
	List<TriangulatorPoly> in_poly = configure(DECOMP_TRIANGLES_EC, p_polygons);
	if (in_poly.empty()) {
		return Vector<Vector<Point2>>();
	}
	TriangulatorPartition tpart;
	List<TriangulatorPoly> out_poly;

	if (tpart.Triangulate_EC(&in_poly, &out_poly) == 0) {
		ERR_PRINT("Triangulation failed!");
		return Vector<Vector<Point2>>();
	}
	return partition(out_poly);
}

Vector<Vector<Point2>> PolyDecomp2DPolyPartition::triangulate_opt(const Vector<Vector<Point2>> &p_polygons) {
	List<TriangulatorPoly> in_poly = configure(DECOMP_TRIANGLES_OPT, p_polygons);
	if (in_poly.empty()) {
		return Vector<Vector<Point2>>();
	}
	TriangulatorPartition tpart;
	List<TriangulatorPoly> out_poly;

	if (tpart.Triangulate_OPT(&in_poly[0], &out_poly) == 0) {
		ERR_PRINT("Triangulation failed!");
		return Vector<Vector<Point2>>();
	}
	return partition(out_poly);
}

Vector<Vector<Point2>> PolyDecomp2DPolyPartition::triangulate_mono(const Vector<Vector<Point2>> &p_polygons) {
	List<TriangulatorPoly> in_poly = configure(DECOMP_TRIANGLES_MONO, p_polygons);
	if (in_poly.empty()) {
		return Vector<Vector<Point2>>();
	}
	TriangulatorPartition tpart;
	List<TriangulatorPoly> out_poly;

	// FIXME: This crashes for some reason!
	if (tpart.Triangulate_MONO(&in_poly, &out_poly) == 0) {
		ERR_PRINT("Triangulation failed!");
		return Vector<Vector<Point2>>();
	}
	return partition(out_poly);
}

Vector<Vector<Point2>> PolyDecomp2DPolyPartition::decompose_convex_hm(const Vector<Vector<Point2>> &p_polygons) {
	List<TriangulatorPoly> in_poly = configure(DECOMP_CONVEX_HM, p_polygons);
	if (in_poly.empty()) {
		return Vector<Vector<Point2>>();
	}
	TriangulatorPartition tpart;
	List<TriangulatorPoly> out_poly;

	if (tpart.ConvexPartition_HM(&in_poly, &out_poly) == 0) {
		ERR_PRINT("Convex decomposition failed!");
		return Vector<Vector<Point2>>();
	}
	return partition(out_poly);
}

Vector<Vector<Point2>> PolyDecomp2DPolyPartition::decompose_convex_opt(const Vector<Vector<Point2>> &p_polygons) {
	List<TriangulatorPoly> in_poly = configure(DECOMP_CONVEX_OPT, p_polygons);
	if (in_poly.empty()) {
		return Vector<Vector<Point2>>();
	}
	TriangulatorPartition tpart;
	List<TriangulatorPoly> out_poly;

	if (tpart.ConvexPartition_OPT(&in_poly[0], &out_poly) == 0) {
		ERR_PRINT("Convex decomposition failed!");
		return Vector<Vector<Point2>>();
	}
	return partition(out_poly);
}
