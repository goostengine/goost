#ifndef POLY_MESH_2D_H
#define POLY_MESH_2D_H

#include "goost/core/math/2d/geometry/poly/poly_node_2d.h"

class PolyMesh2D : public Node2D {
	GDCLASS(PolyMesh2D, Node2D);

public:
	enum BuildMode {
		BUILD_TRIANGLES,
		BUILD_CONVEX,
		BUILD_CONCAVE,
	};
    
private:
    Vector<Vector<Point2>> shapes;
    Vector<Vector<Point2>> _collect_outlines();

protected:
	BuildMode build_mode = BUILD_TRIANGLES;
	Rect2 rect = Rect2(-10, -10, 20, 20);
	PolyNode2D *child = nullptr;

	virtual Vector<Vector<Point2>> _build_shapes();
	virtual void _apply_shapes(){};
    void _update_shapes();

protected:
	virtual void add_child_notify(Node *p_child);
	virtual void remove_child_notify(Node *p_child);

	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_build_mode(BuildMode p_mode);
	BuildMode get_build_mode() const { return build_mode; }

	void update_shapes();

	virtual String get_configuration_warning() const;
};

VARIANT_ENUM_CAST(PolyMesh2D::BuildMode);

#endif // POLY_MESH_2D_H
