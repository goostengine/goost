#ifndef GOOST_GEOMETRY_POLY_H
#define GOOST_GEOMETRY_POLY_H

#include "core/reference.h"

class PolyNode2D;
class PolyBooleanParameters2D;

class PolyBoolean2D {
public:
	enum Operation {
		OPERATION_NONE,
		OPERATION_UNION,
		OPERATION_DIFFERENCE,
		OPERATION_INTERSECTION,
		OPERATION_XOR,
	};
	virtual Vector<Vector<Point2> > polypaths_boolean(Operation p_op, const Vector<Vector<Point2> > &p_polypaths_a, const Vector<Vector<Point2> > &p_polypaths_b) = 0;
	virtual Ref<PolyNode2D> polypaths_boolean_tree(Operation p_op, const Vector<Vector<Point2> > &p_polypaths_a, const Vector<Vector<Point2> > &p_polypaths_b) = 0;
	
	virtual ~PolyBoolean2D() {}
	
public:
	void set_params(const Ref<PolyBooleanParameters2D> &p_params) { params = p_params; }
protected:
	Ref<PolyBooleanParameters2D> params;
};

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
	Vector<Ref<PolyNode2D> > children;
};

#endif // GOOST_GEOMETRY_POLY_H
