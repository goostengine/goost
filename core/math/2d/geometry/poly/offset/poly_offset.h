#ifndef GOOST_GEOMETRY_POLY_OFFSET_2D_H
#define GOOST_GEOMETRY_POLY_OFFSET_2D_H

#include "core/reference.h"

class PolyOffset2D;
class PolyOffsetParameters2D;

class PolyOffset2DBackend {
public:
	virtual void set_parameters(const Ref<PolyOffsetParameters2D> &p_parameters);
	virtual Ref<PolyOffsetParameters2D> get_parameters() { return parameters; }

	virtual Vector<Vector<Point2>> offset_polypaths(const Vector<Vector<Point2>> &p_polypaths, real_t p_delta) = 0;

	PolyOffset2DBackend() {
		default_parameters.instance();
		parameters.instance();
	}
	virtual ~PolyOffset2DBackend() {}

protected:
	Ref<PolyOffsetParameters2D> default_parameters;
	Ref<PolyOffsetParameters2D> parameters;
};


class PolyOffset2D {
public:
	static Vector<Vector<Point2>> inflate_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, const Ref<PolyOffsetParameters2D> &p_parameters = Ref<PolyOffsetParameters2D>());
	static Vector<Vector<Point2>> deflate_polygons(const Vector<Vector<Point2>> &p_polygons, real_t p_delta, const Ref<PolyOffsetParameters2D> &p_parameters = Ref<PolyOffsetParameters2D>());
	static Vector<Vector<Point2>> deflate_polylines(const Vector<Vector<Point2>> &p_polylines, real_t p_delta, const Ref<PolyOffsetParameters2D> &p_parameters = Ref<PolyOffsetParameters2D>());

	static void set_backend(PolyOffset2DBackend *p_backend) { backend = p_backend; }
	static PolyOffset2DBackend *get_backend() { return backend; }

private:
	static PolyOffset2DBackend *backend;
};

// BIND

class _PolyOffset2D : public Reference {
	GDCLASS(_PolyOffset2D, Reference);

private:
	static _PolyOffset2D *singleton;

protected:
	static void _bind_methods();

public:
	static _PolyOffset2D *get_singleton() { return singleton; }
	virtual Ref<_PolyOffset2D> new_instance() const { return memnew(_PolyOffset2D); }

	void set_parameters(const Ref<PolyOffsetParameters2D> &p_parameters);
	Ref<PolyOffsetParameters2D> get_parameters() const;

	Array inflate_polygons(Array p_polygons, real_t p_delta) const;
	Array deflate_polygons(Array p_polygons, real_t p_delta) const;
	Array deflate_polylines(Array p_polylines, real_t p_delta) const;

	_PolyOffset2D() {
		if (!singleton) {
			singleton = this;
		}
		parameters.instance();
	}

protected:
	Ref<PolyOffsetParameters2D> parameters;
};

class PolyOffsetParameters2D : public Reference {
	GDCLASS(PolyOffsetParameters2D, Reference);

public:
	enum JoinType {
		JOIN_SQUARE,
		JOIN_ROUND,
		JOIN_MITER,
	};
	enum EndType {
		END_POLYGON,
		END_JOINED,
		END_BUTT,
		END_SQUARE,
		END_ROUND,
	};

public:
	JoinType join_type = JOIN_SQUARE;
	EndType end_type = END_SQUARE;
	real_t arc_tolerance = 0.25;
	real_t miter_limit = 2.0;

protected:
	static void _bind_methods();

public:
	void set_join_type(JoinType p_join_type) { join_type = p_join_type; }
	JoinType get_join_type() const { return join_type; }

	void set_end_type(EndType p_end_type) { end_type = p_end_type; }
	EndType get_end_type() const { return end_type; }

	void set_arc_tolerance(real_t p_arc_tolerance) { arc_tolerance = p_arc_tolerance; }
	real_t get_arc_tolerance() const { return arc_tolerance; }

	void set_miter_limit(real_t p_miter_limit) { miter_limit = p_miter_limit; }
	real_t get_miter_limit() const { return miter_limit; }

	void reset() {
		join_type = JOIN_SQUARE;
		end_type = END_SQUARE;
		arc_tolerance = 0.25;
		miter_limit = 2.0;
	}
};

VARIANT_ENUM_CAST(PolyOffsetParameters2D::JoinType);
VARIANT_ENUM_CAST(PolyOffsetParameters2D::EndType);

#endif // GOOST_GEOMETRY_POLY_OFFSET_2D_H
