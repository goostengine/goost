#ifndef POLY_COLLISION_SHAPE_2D_H
#define POLY_COLLISION_SHAPE_2D_H

#include "poly_shape_2d.h"
#include "scene/2d/collision_object_2d.h"

class PolyCollisionShape2D : public PolyShape2D {
	GDCLASS(PolyCollisionShape2D, PolyShape2D);

protected:
	CollisionObject2D *parent = nullptr;
	uint32_t owner_id = 0;
	bool disabled = false;
	bool one_way_collision = false;
	float one_way_collision_margin = 1.0;
	void _update_in_shape_owner(bool p_xform_only = false);

protected:
	void _notification(int p_what);
	static void _bind_methods();

	virtual void _apply_shapes();

public:
	void set_disabled(bool p_disabled);
	bool is_disabled() const { return disabled; }

	void set_one_way_collision(bool p_enable);
	bool is_one_way_collision_enabled() const { return one_way_collision; }

	void set_one_way_collision_margin(float p_margin);
	float get_one_way_collision_margin() const { return one_way_collision_margin; }

	PolyCollisionShape2D();
};

#endif // POLY_COLLISION_SHAPE_2D_H
