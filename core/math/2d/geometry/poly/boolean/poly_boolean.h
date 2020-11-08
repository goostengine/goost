#ifndef GOOST_GEOMETRY_POLY_BOOLEAN_H
#define GOOST_GEOMETRY_POLY_BOOLEAN_H

#include "core/reference.h"

class PolyBoolean2D;
class PolyBooleanParameters2D;
class PolyNode2D;

class PolyBoolean2DBackend {
public:
	virtual void set_parameters(const Ref<PolyBooleanParameters2D> &p_parameters);
	virtual Ref<PolyBooleanParameters2D> get_parameters() { return parameters; }

	enum Operation {
		OPERATION_NONE,
		OPERATION_UNION,
		OPERATION_DIFFERENCE,
		OPERATION_INTERSECTION,
		OPERATION_XOR,
	};
	virtual Vector<Vector<Point2>> polypaths_boolean(Operation p_op, const Vector<Vector<Point2>> &p_polypaths_A, const Vector<Vector<Point2>> &p_polypaths_B) = 0;
	virtual Ref<PolyNode2D> polypaths_boolean_tree(Operation p_op, const Vector<Vector<Point2>> &p_polypaths_A, const Vector<Vector<Point2>> &p_polypaths_B) = 0;

	PolyBoolean2DBackend() {
		default_parameters.instance();
		parameters.instance();
	}
	virtual ~PolyBoolean2DBackend() {}

protected:
	Ref<PolyBooleanParameters2D> default_parameters;
	Ref<PolyBooleanParameters2D> parameters;
};

class PolyBoolean2D {
public:
	enum Operation {
		OPERATION_NONE,
		OPERATION_UNION,
		OPERATION_DIFFERENCE,
		OPERATION_INTERSECTION,
		OPERATION_XOR,
	};
	static Vector<Vector<Point2>> merge_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b = Vector<Vector<Point2>>(), const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());
	static Vector<Vector<Point2>> clip_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());
	static Vector<Vector<Point2>> intersect_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());
	static Vector<Vector<Point2>> exclude_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());

	static Vector<Vector<Point2>> polygons_boolean(Operation p_op, const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b = Vector<Vector<Point2>>(), const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());
	static Ref<PolyNode2D> polygons_boolean_tree(Operation p_op, const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b = Vector<Vector<Point2>>(), const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());

	static Vector<Vector<Point2>> clip_polylines_with_polygons(const Vector<Vector<Point2>> &p_polylines, const Vector<Vector<Point2>> &p_polygons, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());
	static Vector<Vector<Point2>> intersect_polylines_with_polygons(const Vector<Vector<Point2>> &p_polylines, const Vector<Vector<Point2>> &p_polygons, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());

	static void set_backend(PolyBoolean2DBackend *p_backend) { backend = p_backend; }
	static PolyBoolean2DBackend *get_backend() { return backend; }

private:
	static PolyBoolean2DBackend *backend;
};

// BIND

class _PolyBoolean2D : public Reference {
	GDCLASS(_PolyBoolean2D, Reference);

private:
	static _PolyBoolean2D *singleton;

protected:
	static void _bind_methods();

public:
	static _PolyBoolean2D *get_singleton() { return singleton; }

	void set_parameters(const Ref<PolyBooleanParameters2D> &p_parameters) { parameters = p_parameters; }
	Ref<PolyBooleanParameters2D> get_parameters() const { return parameters; }

	enum Operation {
		OPERATION_NONE,
		OPERATION_UNION,
		OPERATION_DIFFERENCE,
		OPERATION_INTERSECTION,
		OPERATION_XOR,
	};
	Array merge_polygons(Array p_polygons_a, Array p_polygons_b) const;
	Array clip_polygons(Array p_polygons_a, Array p_polygons_b) const;
	Array intersect_polygons(Array p_polygons_a, Array p_polygons_b) const;
	Array exclude_polygons(Array p_polygons_a, Array p_polygons_b) const;

	Array polygons_boolean(Operation p_op, Array p_polygons_a, Array p_polygons_b) const;
	Ref<PolyNode2D> polygons_boolean_tree(Operation p_op, Array p_polygons_a, Array p_polygons_b) const;

	Array clip_polylines_with_polygons(Array p_polylines, Array p_polygons) const;
	Array intersect_polylines_with_polygons(Array p_polylines, Array p_polygons) const;

	_PolyBoolean2D::_PolyBoolean2D() {
		if (!singleton) {
			singleton = this;
		}
		parameters.instance();
	}

protected:
	Ref<PolyBooleanParameters2D> parameters;
};

VARIANT_ENUM_CAST(_PolyBoolean2D::Operation);

class PolyBooleanParameters2D : public Reference {
	GDCLASS(PolyBooleanParameters2D, Reference);

public:
	enum FillRule {
		FILL_RULE_EVEN_ODD,
		FILL_RULE_NON_ZERO,
		FILL_RULE_POSITIVE,
		FILL_RULE_NEGATIVE,
	};

public:
	FillRule subject_fill_rule = FILL_RULE_NON_ZERO;
	FillRule clip_fill_rule = FILL_RULE_NON_ZERO;
	bool subject_open = false;
	bool reverse_solution = false;
	bool strictly_simple = false;
	bool preserve_collinear = false;

protected:
	static void _bind_methods();

public:
	void set_subject_fill_rule(FillRule p_subject_fill_rule) { subject_fill_rule = p_subject_fill_rule; }
	FillRule get_subject_fill_rule() const { return subject_fill_rule; }

	void set_clip_fill_rule(FillRule p_clip_fill_rule) { clip_fill_rule = p_clip_fill_rule; }
	FillRule get_clip_fill_rule() const { return clip_fill_rule; }

	void set_subject_open(bool p_subject_open) { subject_open = p_subject_open; }
	bool is_subject_open() const { return subject_open; }

	void set_reverse_solution(bool p_reverse_solution) { reverse_solution = p_reverse_solution; }
	bool is_reverse_solution() const { return reverse_solution; }

	void set_strictly_simple(bool p_strictly_simple) { strictly_simple = p_strictly_simple; }
	bool is_strictly_simple() const { return strictly_simple; }

	void set_preserve_collinear(bool p_preserve_collinear) { preserve_collinear = p_preserve_collinear; }
	bool is_preserve_collinear() const { return preserve_collinear; }

	void reset() {
		subject_fill_rule = FILL_RULE_NON_ZERO;
		clip_fill_rule = FILL_RULE_NON_ZERO;
		subject_open = false;
		reverse_solution = false;
		strictly_simple = false;
		preserve_collinear = false;
	}
};

VARIANT_ENUM_CAST(PolyBooleanParameters2D::FillRule);

class PolyNode2D : public Reference {
	GDCLASS(PolyNode2D, Reference);

public:
	Ref<PolyNode2D> new_child(const Vector<Point2> &p_path);
	Ref<PolyNode2D> get_child(int p_idx);
	int get_child_count() const { return children.size(); }
	Array get_children() const;
	Ref<PolyNode2D> get_parent() const { return parent; }
	void set_path(const Vector<Point2> &p_path) { path = p_path; }
	Vector<Point2> get_path() const { return path; }
	bool is_hole() const;
	void clear();

protected:
	static void _bind_methods();

private:
	Ref<PolyNode2D> parent;
	Vector<Point2> path;
	Vector<Ref<PolyNode2D>> children;
};

#endif // GOOST_GEOMETRY_POLY_BOOLEAN_H
