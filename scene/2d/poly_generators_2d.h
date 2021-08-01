#pragma once

#include "goost/core/math/geometry/2d/poly/poly_node_2d.h"
#include "goost/core/math/geometry/2d/poly/offset/poly_offset.h"

#include "scene/2d/path_2d.h"

class PolyCircle2D : public PolyNode2D {
	GDCLASS(PolyCircle2D, PolyNode2D);

	real_t radius = 32.0;
	real_t max_error = 0.25;

protected:
	static void _bind_methods();
	virtual Vector<Vector<Point2>> _build_outlines();

public:
	void set_radius(real_t p_radius);
	real_t get_radius() const { return radius; }

	void set_max_error(real_t p_max_error);
	real_t get_max_error() const { return max_error; }

	PolyCircle2D() {
		build_outlines();
	}
};

class PolyRectangle2D : public PolyNode2D {
	GDCLASS(PolyRectangle2D, PolyNode2D);

	Vector2 extents = Vector2(32, 32);

protected:
	static void _bind_methods();
	virtual Vector<Vector<Point2>> _build_outlines();

public:
	void set_extents(const Vector2 &p_extents);
	Vector2 get_extents() const { return extents; }

	PolyRectangle2D() {
		build_outlines();
	}
};

class PolyPath2D : public PolyNode2D {
	GDCLASS(PolyPath2D, PolyNode2D);

	Map<ObjectID, Ref<Curve2D>> paths; // Path2D : Cached Curve2D

	real_t buffer_offset = 32.0;
	Ref<PolyOffsetParameters2D> buffer_parameters;

	int tessellation_stages = 4;
	float tessellation_tolerance_degrees = 4.0f;

protected:
	void _notification(int p_what);
	static void _bind_methods();
	virtual Vector<Vector<Point2>> _build_outlines();

	virtual void add_child_notify(Node *p_child);
	virtual void remove_child_notify(Node *p_child);

public:
	void set_buffer_offset(real_t p_buffer_offset);
	real_t get_buffer_offset() const { return buffer_offset; }

	void set_buffer_parameters(const Ref<PolyOffsetParameters2D> &p_buffer_parameters);
	Ref<PolyOffsetParameters2D> get_buffer_parameters() const { return buffer_parameters; }

	void set_tessellation_stages(int p_tessellation_stages);
	int get_tessellation_stages() const { return tessellation_stages; }
	
	void set_tessellation_tolerance_degrees(float p_tessellation_tolerance_degrees);
	float get_tessellation_tolerance_degrees() const { return tessellation_tolerance_degrees; }

	virtual String get_configuration_warning() const;

	PolyPath2D() {
		set_process_internal(true);
		build_outlines();
	}
	~PolyPath2D() {
		paths.clear();
	}
};

