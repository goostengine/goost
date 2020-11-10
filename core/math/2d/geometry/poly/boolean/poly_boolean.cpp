#include "poly_boolean.h"

PolyBoolean2DBackend *PolyBoolean2D::backend = nullptr;

void PolyBoolean2DBackend::set_parameters(const Ref<PolyBooleanParameters2D> &p_parameters) {
	if (p_parameters.is_valid()) {
		parameters = p_parameters;
	} else {
		parameters = Ref<PolyBooleanParameters2D>();
		parameters = default_parameters;
		parameters->reset();
	}
}

Vector<Vector<Point2>> PolyBoolean2D::merge_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	backend->get_parameters()->subject_open = false;
	return backend->boolean_polypaths(p_polygons_a, p_polygons_b, PolyBoolean2DBackend::OP_UNION);
}

Vector<Vector<Point2>> PolyBoolean2D::clip_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	backend->get_parameters()->subject_open = false;
	return backend->boolean_polypaths(p_polygons_a, p_polygons_b, PolyBoolean2DBackend::OP_DIFFERENCE);
}

Vector<Vector<Point2>> PolyBoolean2D::intersect_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	backend->get_parameters()->subject_open = false;
	return backend->boolean_polypaths(p_polygons_a, p_polygons_b, PolyBoolean2DBackend::OP_INTERSECTION);
}

Vector<Vector<Point2>> PolyBoolean2D::exclude_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	backend->get_parameters()->subject_open = false;
	return backend->boolean_polypaths(p_polygons_a, p_polygons_b, PolyBoolean2DBackend::OP_XOR);
}

Vector<Vector<Point2>> PolyBoolean2D::boolean_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Operation p_op, const Ref<PolyBooleanParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	backend->get_parameters()->subject_open = false;
	return backend->boolean_polypaths(p_polygons_a, p_polygons_b, PolyBoolean2DBackend::Operation(p_op));
}

PolyNode2D *PolyBoolean2D::boolean_polygons_tree(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Operation p_op, const Ref<PolyBooleanParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	backend->get_parameters()->subject_open = false;
	return backend->boolean_polypaths_tree(p_polygons_a, p_polygons_b, PolyBoolean2DBackend::Operation(p_op));
}

Vector<Vector<Point2>> PolyBoolean2D::clip_polylines_with_polygons(const Vector<Vector<Point2>> &p_polylines, const Vector<Vector<Point2>> &p_polygons, const Ref<PolyBooleanParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	backend->get_parameters()->subject_open = true;
	return backend->boolean_polypaths(p_polylines, p_polygons, PolyBoolean2DBackend::OP_DIFFERENCE);
}

Vector<Vector<Point2>> PolyBoolean2D::intersect_polylines_with_polygons(const Vector<Vector<Point2>> &p_polylines, const Vector<Vector<Point2>> &p_polygons, const Ref<PolyBooleanParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	backend->get_parameters()->subject_open = true;
	return backend->boolean_polypaths(p_polylines, p_polygons, PolyBoolean2DBackend::OP_INTERSECTION);
}

// BIND

_PolyBoolean2D *_PolyBoolean2D::singleton = nullptr;

void _PolyBoolean2D::set_parameters(const Ref<PolyBooleanParameters2D> &p_parameters) {
#ifdef DEBUG_ENABLED
	if (singleton == this) {
		ERR_FAIL_MSG("Configuring parameters is forbidden for a global instance. Please create a new local instance of PolyBoolean2D with `new_instance()` method");
	}
#endif
	parameters = p_parameters;
}

Ref<PolyBooleanParameters2D> _PolyBoolean2D::get_parameters() const {
#ifdef DEBUG_ENABLED
	if (singleton == this) {
		ERR_FAIL_V_MSG(Ref<PolyBooleanParameters2D>(), "Configuring parameters is forbidden for a global instance. Please create a new local instance of PolyBoolean2D with `new_instance()` method");
	}
#endif
	return parameters;
}

Array _PolyBoolean2D::merge_polygons(Array p_polygons_a, Array p_polygons_b) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	const auto &params = singleton == this ? Ref<PolyBooleanParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyBoolean2D::merge_polygons(polygons_a, polygons_b, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _PolyBoolean2D::clip_polygons(Array p_polygons_a, Array p_polygons_b) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	const auto &params = singleton == this ? Ref<PolyBooleanParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyBoolean2D::clip_polygons(polygons_a, polygons_b, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _PolyBoolean2D::intersect_polygons(Array p_polygons_a, Array p_polygons_b) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	const auto &params = singleton == this ? Ref<PolyBooleanParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyBoolean2D::intersect_polygons(polygons_a, polygons_b, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _PolyBoolean2D::exclude_polygons(Array p_polygons_a, Array p_polygons_b) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	const auto &params = singleton == this ? Ref<PolyBooleanParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyBoolean2D::exclude_polygons(polygons_a, polygons_b, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _PolyBoolean2D::boolean_polygons(Array p_polygons_a, Array p_polygons_b, Operation p_op) const {
	Vector<Vector<Vector2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); ++i) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Vector2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); ++i) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	const auto &params = singleton == this ? Ref<PolyBooleanParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyBoolean2D::boolean_polygons(polygons_a, polygons_b, PolyBoolean2D::Operation(p_op), params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

PolyNode2D *_PolyBoolean2D::boolean_polygons_tree(Array p_polygons_a, Array p_polygons_b, Operation p_op) const {
	Vector<Vector<Point2>> polygons_a;
	for (int i = 0; i < p_polygons_a.size(); i++) {
		polygons_a.push_back(p_polygons_a[i]);
	}
	Vector<Vector<Point2>> polygons_b;
	for (int i = 0; i < p_polygons_b.size(); i++) {
		polygons_b.push_back(p_polygons_b[i]);
	}
	const auto &params = singleton == this ? Ref<PolyBooleanParameters2D>() : parameters;
	return PolyBoolean2D::boolean_polygons_tree(polygons_a, polygons_b, PolyBoolean2D::Operation(p_op), params);
}

Array _PolyBoolean2D::clip_polylines_with_polygons(Array p_polylines, Array p_polygons) const {
	Vector<Vector<Point2>> polylines;
	for (int i = 0; i < p_polylines.size(); i++) {
		polylines.push_back(p_polylines[i]);
	}
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	const auto &params = singleton == this ? Ref<PolyBooleanParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyBoolean2D::clip_polylines_with_polygons(polylines, polygons, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _PolyBoolean2D::intersect_polylines_with_polygons(Array p_polylines, Array p_polygons) const {
	Vector<Vector<Point2>> polylines;
	for (int i = 0; i < p_polylines.size(); i++) {
		polylines.push_back(p_polylines[i]);
	}
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	const auto &params = singleton == this ? Ref<PolyBooleanParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyBoolean2D::intersect_polylines_with_polygons(polylines, polygons, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

void _PolyBoolean2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("new_instance"), &_PolyBoolean2D::new_instance);

	ClassDB::bind_method(D_METHOD("set_parameters", "parameters"), &_PolyBoolean2D::set_parameters);
	ClassDB::bind_method(D_METHOD("get_parameters"), &_PolyBoolean2D::get_parameters);

	ClassDB::bind_method(D_METHOD("merge_polygons", "polygons_a", "polygons_b"), &_PolyBoolean2D::merge_polygons, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("clip_polygons", "polygons_a", "polygons_b"), &_PolyBoolean2D::clip_polygons);
	ClassDB::bind_method(D_METHOD("intersect_polygons", "polygons_a", "polygons_b"), &_PolyBoolean2D::intersect_polygons);
	ClassDB::bind_method(D_METHOD("exclude_polygons", "polygons_a", "polygons_b"), &_PolyBoolean2D::exclude_polygons);

	ClassDB::bind_method(D_METHOD("boolean_polygons", "polygons_a", "polygons_b", "operation"), &_PolyBoolean2D::boolean_polygons);
	ClassDB::bind_method(D_METHOD("boolean_polygons_tree", "polygons_a", "polygons_b", "operation"), &_PolyBoolean2D::boolean_polygons_tree);

	ClassDB::bind_method(D_METHOD("clip_polylines_with_polygons", "polylines", "polygons"), &_PolyBoolean2D::clip_polylines_with_polygons);
	ClassDB::bind_method(D_METHOD("intersect_polylines_with_polygons", "polylines", "polygons"), &_PolyBoolean2D::intersect_polylines_with_polygons);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "parameters"), "set_parameters", "get_parameters");

	BIND_ENUM_CONSTANT(OP_NONE);
	BIND_ENUM_CONSTANT(OP_UNION);
	BIND_ENUM_CONSTANT(OP_DIFFERENCE);
	BIND_ENUM_CONSTANT(OP_INTERSECTION);
	BIND_ENUM_CONSTANT(OP_XOR);
}

void PolyBooleanParameters2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_subject_fill_rule", "subject_fill_rule"), &PolyBooleanParameters2D::set_subject_fill_rule);
	ClassDB::bind_method(D_METHOD("get_subject_fill_rule"), &PolyBooleanParameters2D::get_subject_fill_rule);
	ClassDB::bind_method(D_METHOD("set_clip_fill_rule", "clip_fill_rule"), &PolyBooleanParameters2D::set_clip_fill_rule);
	ClassDB::bind_method(D_METHOD("get_clip_fill_rule"), &PolyBooleanParameters2D::get_clip_fill_rule);
	ClassDB::bind_method(D_METHOD("set_subject_open", "subject_open"), &PolyBooleanParameters2D::set_subject_open);
	ClassDB::bind_method(D_METHOD("is_subject_open"), &PolyBooleanParameters2D::is_subject_open);
	ClassDB::bind_method(D_METHOD("set_reverse_solution", "reverse_solution"), &PolyBooleanParameters2D::set_reverse_solution);
	ClassDB::bind_method(D_METHOD("is_reverse_solution"), &PolyBooleanParameters2D::is_reverse_solution);
	ClassDB::bind_method(D_METHOD("set_strictly_simple", "strictly_simple"), &PolyBooleanParameters2D::set_strictly_simple);
	ClassDB::bind_method(D_METHOD("is_strictly_simple"), &PolyBooleanParameters2D::is_strictly_simple);
	ClassDB::bind_method(D_METHOD("set_preserve_collinear", "preserve_collinear"), &PolyBooleanParameters2D::set_preserve_collinear);
	ClassDB::bind_method(D_METHOD("is_preserve_collinear"), &PolyBooleanParameters2D::is_preserve_collinear);

	BIND_ENUM_CONSTANT(FILL_RULE_EVEN_ODD);
	BIND_ENUM_CONSTANT(FILL_RULE_NON_ZERO);
	BIND_ENUM_CONSTANT(FILL_RULE_POSITIVE);
	BIND_ENUM_CONSTANT(FILL_RULE_NEGATIVE);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "subject_fill_rule"), "set_subject_fill_rule", "get_subject_fill_rule");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "clip_fill_rule"), "set_clip_fill_rule", "get_clip_fill_rule");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "subject_open"), "set_subject_open", "is_subject_open");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "reverse_solution"), "set_reverse_solution", "is_reverse_solution");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "strictly_simple"), "set_strictly_simple", "is_strictly_simple");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "preserve_collinear"), "set_preserve_collinear", "is_preserve_collinear");
}
