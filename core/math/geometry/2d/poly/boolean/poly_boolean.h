#pragma once

#include "core/resource.h"
#include "../poly_node_2d.h"

class PolyBoolean2D;
class PolyBooleanParameters2D;

class PolyBoolean2DBackend {
public:
	virtual void set_parameters(const Ref<PolyBooleanParameters2D> &p_parameters);
	virtual Ref<PolyBooleanParameters2D> get_parameters() { return parameters; }

	enum Operation {
		OP_NONE,
		OP_UNION,
		OP_DIFFERENCE,
		OP_INTERSECTION,
		OP_XOR,
	};
	virtual Vector<Vector<Point2>> boolean_polypaths(const Vector<Vector<Point2>> &p_polypaths_A, const Vector<Vector<Point2>> &p_polypaths_B, Operation p_op) = 0;
	// Note: `r_root` should point to an existing node.
	virtual void boolean_polypaths_tree(const Vector<Vector<Point2>> &p_polypaths_A, const Vector<Vector<Point2>> &p_polypaths_B, Operation p_op, PolyNode2D *r_root) = 0;

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
		OP_NONE,
		OP_UNION,
		OP_DIFFERENCE,
		OP_INTERSECTION,
		OP_XOR,
	};
	static Vector<Vector<Point2>> merge_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b = Vector<Vector<Point2>>(), const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());
	static Vector<Vector<Point2>> clip_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());
	static Vector<Vector<Point2>> intersect_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());
	static Vector<Vector<Point2>> exclude_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());

	static Vector<Vector<Point2>> boolean_polygons(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Operation p_op, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());
	static void boolean_polygons_tree(const Vector<Vector<Point2>> &p_polygons_a, const Vector<Vector<Point2>> &p_polygons_b, Operation p_op, PolyNode2D *r_tree, const Ref<PolyBooleanParameters2D> &p_parameters = Ref<PolyBooleanParameters2D>());

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
	// Not using Ref<_PolyBoolean2D> as return type due to namespace issues in Godot.
	virtual Ref<Reference> new_instance() const { return memnew(_PolyBoolean2D); }

	void set_parameters(const Ref<PolyBooleanParameters2D> &p_parameters);
	Ref<PolyBooleanParameters2D> get_parameters() const;

	enum Operation {
		OP_NONE,
		OP_UNION,
		OP_DIFFERENCE,
		OP_INTERSECTION,
		OP_XOR,
	};
	Array merge_polygons(Array p_polygons_a, Array p_polygons_b) const;
	Array clip_polygons(Array p_polygons_a, Array p_polygons_b) const;
	Array intersect_polygons(Array p_polygons_a, Array p_polygons_b) const;
	Array exclude_polygons(Array p_polygons_a, Array p_polygons_b) const;

	Array boolean_polygons(Array p_polygons_a, Array p_polygons_b, Operation p_op) const;
	PolyNode2D *boolean_polygons_tree(Array p_polygons_a, Array p_polygons_b, Operation p_op) const;

	Array clip_polylines_with_polygons(Array p_polylines, Array p_polygons) const;
	Array intersect_polylines_with_polygons(Array p_polylines, Array p_polygons) const;

	_PolyBoolean2D() {
		if (!singleton) {
			singleton = this;
		}
		parameters.instance();
	}

protected:
	Ref<PolyBooleanParameters2D> parameters;
};

VARIANT_ENUM_CAST(_PolyBoolean2D::Operation);

class PolyBooleanParameters2D : public Resource {
	GDCLASS(PolyBooleanParameters2D, Resource);

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
	void set_subject_fill_rule(FillRule p_subject_fill_rule);
	FillRule get_subject_fill_rule() const { return subject_fill_rule; }

	void set_clip_fill_rule(FillRule p_clip_fill_rule);
	FillRule get_clip_fill_rule() const { return clip_fill_rule; }

	void set_subject_open(bool p_subject_open);
	bool is_subject_open() const { return subject_open; }

	void set_reverse_solution(bool p_reverse_solution);
	bool is_reverse_solution() const { return reverse_solution; }

	void set_strictly_simple(bool p_strictly_simple);
	bool is_strictly_simple() const { return strictly_simple; }

	void set_preserve_collinear(bool p_preserve_collinear);
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

