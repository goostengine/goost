#include "poly_boolean_clipper6.h"
#include "goost/core/math/geometry/2d/goost_geometry_2d.h"
#include "goost/core/math/geometry/2d/poly/utils/godot_clipper6_path_convert.h"

Vector<Vector<Point2>> PolyBoolean2DClipper6::boolean_polypaths(const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b, Operation p_op) {
	ClipperLib::Clipper clp = configure(p_op, parameters);

	ClipperLib::Paths subject;
	GodotClipperUtils::scale_up_polypaths(p_polypaths_a, subject);
	clp.AddPaths(subject, ClipperLib::ptSubject, !subject_open);

	if (!p_polypaths_b.empty()) { // Optional for merge operation.
		ClipperLib::Paths clip;
		GodotClipperUtils::scale_up_polypaths(p_polypaths_b, clip);
		clp.AddPaths(clip, ClipperLib::ptClip, true);
	}

	ClipperLib::Paths solution;
	if (!subject_open) {
		clp.Execute(clip_type, solution, subject_fill_type, clip_fill_type);
	} else {
		ClipperLib::PolyTree tree;
		clp.Execute(clip_type, tree, subject_fill_type, clip_fill_type);
		ClipperLib::OpenPathsFromPolyTree(tree, solution);
	}

	Vector<Vector<Point2>> ret;
	GodotClipperUtils::scale_down_polypaths(solution, ret);

	return ret;
}

void PolyBoolean2DClipper6::boolean_polypaths_tree(const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b, Operation p_op, PolyNode2D *r_root) {
	ERR_FAIL_NULL(r_root);

	ClipperLib::Clipper clp = configure(p_op, parameters);

	ClipperLib::Paths subject;
	GodotClipperUtils::scale_up_polypaths(p_polypaths_a, subject);
	clp.AddPaths(subject, ClipperLib::ptSubject, !subject_open);

	if (!p_polypaths_b.empty()) { // Optional for merge operation.
		ClipperLib::Paths clip;
		GodotClipperUtils::scale_up_polypaths(p_polypaths_b, clip);
		clp.AddPaths(clip, ClipperLib::ptClip, true);
	}

	ClipperLib::PolyTree tree;
	clp.Execute(clip_type, tree, subject_fill_type, clip_fill_type);

	List<ClipperLib::PolyNode *> to_visit;
	Map<ClipperLib::PolyNode *, PolyNode2D *> nodes;

	nodes.insert(&tree, r_root);
	to_visit.push_back(&tree);

	while (!to_visit.empty()) {
		ClipperLib::PolyNode *parent = to_visit.back()->get();
		to_visit.pop_back();

		for (int i = 0; i < parent->ChildCount(); ++i) {
			ClipperLib::PolyNode *child = parent->Childs[i];
			Vector<Point2> child_path;
			GodotClipperUtils::scale_down_polypath(child->Contour, child_path);
			PolyNode2D *new_child = nodes[parent]->new_child(child_path);
			nodes.insert(child, new_child);
			to_visit.push_back(child);
		}
	}
}

ClipperLib::Clipper PolyBoolean2DClipper6::configure(Operation p_op, const Ref<PolyBooleanParameters2D> &p_parameters) {
	using namespace ClipperLib;

	switch (p_op) {
		case OP_NONE: {
			// OP_NONE is not available in clipper6 backend, fallback to OP_UNION.
			clip_type = ctUnion;
		} break;
		case OP_UNION:
			clip_type = ctUnion;
			break;
		case OP_DIFFERENCE:
			clip_type = ctDifference;
			break;
		case OP_INTERSECTION:
			clip_type = ctIntersection;
			break;
		case OP_XOR:
			clip_type = ctXor;
			break;
	}

	int init_options = 0;

	subject_fill_type = PolyFillType(p_parameters->subject_fill_rule);
	clip_fill_type = PolyFillType(p_parameters->clip_fill_rule);
	init_options |= p_parameters->reverse_solution ? InitOptions::ioReverseSolution : 0;
	init_options |= p_parameters->strictly_simple ? InitOptions::ioStrictlySimple : 0;
	init_options |= p_parameters->preserve_collinear ? InitOptions::ioPreserveCollinear : 0;
	subject_open = p_parameters->subject_open;

	return Clipper(init_options);
}
