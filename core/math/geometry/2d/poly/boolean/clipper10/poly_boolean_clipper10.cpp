#include "poly_boolean_clipper10.h"
#include "goost/core/math/geometry/2d/goost_geometry_2d.h"
#include "goost/core/math/geometry/2d/poly/utils/godot_clipper10_path_convert.h"

Vector<Vector<Point2>> PolyBoolean2DClipper10::boolean_polypaths(const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b, Operation p_op) {
	clipperlib::Clipper clp = configure(p_op, parameters);

	clipperlib::Paths subject;
	GodotClipperUtils::scale_up_polypaths(p_polypaths_a, subject);
	clp.AddPaths(subject, clipperlib::ptSubject, subject_open);

	if (!p_polypaths_b.empty()) { // Optional for merge operation.
		clipperlib::Paths clip;
		GodotClipperUtils::scale_up_polypaths(p_polypaths_b, clip);
		clp.AddPaths(clip, clipperlib::ptClip, false);
	}

	clipperlib::Paths solution_closed, solution_open;
	clp.Execute(clip_type, solution_closed, solution_open, subject_fill_rule);

	Vector<Vector<Point2>> ret;
	GodotClipperUtils::scale_down_polypaths(subject_open ? solution_open : solution_closed, ret);

	return ret;
}

void PolyBoolean2DClipper10::boolean_polypaths_tree(const Vector<Vector<Point2>> &p_polypaths_a, const Vector<Vector<Point2>> &p_polypaths_b, Operation p_op, PolyNode2D *r_root) {
	ERR_FAIL_NULL(r_root);

	clipperlib::Clipper clp = configure(p_op, parameters);

	clipperlib::Paths subject;
	GodotClipperUtils::scale_up_polypaths(p_polypaths_a, subject);
	clp.AddPaths(subject, clipperlib::ptSubject, subject_open);

	if (!p_polypaths_b.empty()) { // Optional for merge operation.
		clipperlib::Paths clip;
		GodotClipperUtils::scale_up_polypaths(p_polypaths_b, clip);
		clp.AddPaths(clip, clipperlib::ptClip, false);
	}

	clipperlib::PolyPath tree;
	clipperlib::Paths solution_open; // Ignored here but required.
	clp.Execute(clip_type, tree, solution_open, subject_fill_rule);

	List<clipperlib::PolyPath *> to_visit;
	Map<clipperlib::PolyPath *, PolyNode2D *> nodes;

	nodes.insert(&tree, r_root);
	to_visit.push_back(&tree);

	while (!to_visit.empty()) {
		clipperlib::PolyPath *parent = to_visit.back()->get();
		to_visit.pop_back();

		for (int i = 0; i < parent->ChildCount(); ++i) {
			clipperlib::PolyPath *child = &parent->GetChild(i);
			Vector<Point2> child_path;
			GodotClipperUtils::scale_down_polypath(child->GetPath(), child_path);
			PolyNode2D *new_child = nodes[parent]->new_child(child_path);
			nodes.insert(child, new_child);
			to_visit.push_back(child);
		}
	}
}

clipperlib::Clipper PolyBoolean2DClipper10::configure(Operation p_op, const Ref<PolyBooleanParameters2D> &p_parameters) {
	using namespace clipperlib;

	switch (p_op) {
		case OP_NONE:
			clip_type = ctNone;
			break;
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
	subject_fill_rule = FillRule(p_parameters->subject_fill_rule);
	clip_fill_rule = FillRule(p_parameters->clip_fill_rule);
	subject_open = p_parameters->subject_open;

	return Clipper();
}
