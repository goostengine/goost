#ifndef GOOST_GEOMETRY_POLY_DECOMP_H
#define GOOST_GEOMETRY_POLY_DECOMP_H

#include "core/reference.h"

class PolyDecomp2D;
class PolyDecompParameters2D;

class PolyDecomp2DBackend {
public:
	virtual void set_parameters(const Ref<PolyDecompParameters2D> &p_parameters);
	virtual Ref<PolyDecompParameters2D> get_parameters() { return parameters; }

	enum Decomposition {
		DECOMP_TRIANGLES_EC,
		DECOMP_TRIANGLES_OPT,
		DECOMP_TRIANGLES_MONO,
		DECOMP_CONVEX_HM,
		DECOMP_CONVEX_OPT,
	};
	virtual Vector<Vector<Point2>> decompose_polygons(const Vector<Vector<Point2>> &p_polygons, Decomposition p_type);

	virtual Vector<Vector<Point2>> triangulate_ec(const Vector<Vector<Point2>> &p_polygons) = 0;
	virtual Vector<Vector<Point2>> triangulate_opt(const Vector<Vector<Point2>> &p_polygons) = 0;
	virtual Vector<Vector<Point2>> triangulate_mono(const Vector<Vector<Point2>> &p_polygons) = 0;
	virtual Vector<Vector<Point2>> decompose_convex_hm(const Vector<Vector<Point2>> &p_polygons) = 0;
	virtual Vector<Vector<Point2>> decompose_convex_opt(const Vector<Vector<Point2>> &p_polygons) = 0;

	PolyDecomp2DBackend() {
		default_parameters.instance();
		parameters.instance();
	}
	virtual ~PolyDecomp2DBackend() {}

protected:
	Ref<PolyDecompParameters2D> default_parameters;
	Ref<PolyDecompParameters2D> parameters;
};

class PolyDecomp2D {
public:
	enum Decomposition {
		DECOMP_TRIANGLES_EC,
		DECOMP_TRIANGLES_OPT,
		DECOMP_TRIANGLES_MONO,
		DECOMP_CONVEX_HM,
		DECOMP_CONVEX_OPT,
	};
	static Vector<Vector<Point2>> triangulate_polygons(const Vector<Vector<Point2>> &p_polygons, const Ref<PolyDecompParameters2D> &p_parameters = Ref<PolyDecompParameters2D>());
	static Vector<Vector<Point2>> decompose_polygons_into_convex(const Vector<Vector<Point2>> &p_polygons, const Ref<PolyDecompParameters2D> &p_parameters = Ref<PolyDecompParameters2D>());
	static Vector<Vector<Point2>> decompose_polygons(const Vector<Vector<Point2>> &p_polygons, Decomposition p_type, const Ref<PolyDecompParameters2D> &p_parameters = Ref<PolyDecompParameters2D>());

	static void set_backend(PolyDecomp2DBackend *p_backend) { backend = p_backend; }
	static PolyDecomp2DBackend *get_backend() { return backend; }

private:
	static PolyDecomp2DBackend *backend;
};

// BIND

class _PolyDecomp2D : public Reference {
	GDCLASS(_PolyDecomp2D, Reference);

private:
	static _PolyDecomp2D *singleton;

protected:
	static void _bind_methods();

public:
	static _PolyDecomp2D *get_singleton() { return singleton; }
	// Not using Ref<_PolyDecomp2D> as return type due to namespace issues in Godot.
	virtual Ref<Reference> new_instance() const { return memnew(_PolyDecomp2D); }

	void set_parameters(const Ref<PolyDecompParameters2D> &p_parameters);
	Ref<PolyDecompParameters2D> get_parameters() const;

	enum Decomposition {
		DECOMP_TRIANGLES_EC,
		DECOMP_TRIANGLES_OPT,
		DECOMP_TRIANGLES_MONO,
		DECOMP_CONVEX_HM,
		DECOMP_CONVEX_OPT,
	};
	Array triangulate_polygons(Array p_polygons) const;
	Array decompose_polygons_into_convex(Array p_polygons) const;
	Array decompose_polygons(Array p_polygons, Decomposition p_type) const;

	_PolyDecomp2D() {
		if (!singleton) {
			singleton = this;
		}
		parameters.instance();
	}

protected:
	Ref<PolyDecompParameters2D> parameters;
};

VARIANT_ENUM_CAST(_PolyDecomp2D::Decomposition);

class PolyDecompParameters2D : public Reference {
	GDCLASS(PolyDecompParameters2D, Reference);

public:
	enum FillRule {
		FILL_RULE_EVEN_ODD,
		FILL_RULE_NON_ZERO,
		FILL_RULE_POSITIVE,
		FILL_RULE_NEGATIVE,
	};

public:
	FillRule fill_rule = FILL_RULE_NON_ZERO;
	// Steiner points...
	// Inner polygons indices...

protected:
	static void _bind_methods();

public:
	void set_fill_rule(FillRule p_fill_rule) { fill_rule = p_fill_rule; }
	FillRule get_fill_rule() const { return fill_rule; }

	void reset() {
		fill_rule = FILL_RULE_NON_ZERO;
	}
};

VARIANT_ENUM_CAST(PolyDecompParameters2D::FillRule);

#endif // GOOST_GEOMETRY_POLY_DECOMP_H
