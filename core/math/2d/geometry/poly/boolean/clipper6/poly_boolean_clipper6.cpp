#include "poly_boolean_clipper6.h"
#include "goost/core/math/2d/geometry/goost_geometry_2d.h"
#include "goost/core/math/2d/geometry/poly/utils/godot_clipper6_path_convert.h"

Vector<Vector<Point2>> PolyBoolean2DClipper6::polypaths_boolean(Operation p_op, const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b) {
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

Ref<PolyNode2D> PolyBoolean2DClipper6::polypaths_boolean_tree(Operation p_op, const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b) {
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

	Ref<PolyNode2D> root;
	root.instance();

	List<ClipperLib::PolyNode *> to_visit;
	Map<ClipperLib::PolyNode *, Ref<PolyNode2D>> nodes;

	nodes.insert(&tree, root);
	to_visit.push_back(&tree);

	while (!to_visit.empty()) {
		ClipperLib::PolyNode *parent = to_visit.back()->get();
		to_visit.pop_back();

		for (int i = 0; i < parent->ChildCount(); ++i) {
			ClipperLib::PolyNode *child = parent->Childs[i];
			Vector<Point2> child_path;
			GodotClipperUtils::scale_down_polypath(child->Contour, child_path);
			Ref<PolyNode2D> new_child = nodes[parent]->new_child(child_path);
			nodes.insert(child, new_child);
			to_visit.push_back(child);
		}
	}
	return root;
}

ClipperLib::Clipper PolyBoolean2DClipper6::configure(Operation p_op, const Ref<PolyBooleanParameters2D> &p_parameters) {
	using namespace ClipperLib;

	switch (p_op) {
		case OPERATION_NONE: {
			clip_type = ctUnion;
			WARN_PRINT_ONCE("OPERATION_NONE is not available in clipper6 backend, fallback to OPERATION_UNION.");
		} break;
		case OPERATION_UNION:
			clip_type = ctUnion;
			break;
		case OPERATION_DIFFERENCE:
			clip_type = ctDifference;
			break;
		case OPERATION_INTERSECTION:
			clip_type = ctIntersection;
			break;
		case OPERATION_XOR:
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
