#ifndef SHAPE_CAST_2D_H
#define SHAPE_CAST_2D_H

#include "scene/2d/node_2d.h"
#include "scene/resources/shape_2d.h"

class ShapeCast2D : public Node2D {
	GDCLASS(ShapeCast2D, Node2D);
	
	bool enabled;
	
	Ref<Shape2D> shape;
	RID shape_rid;
	Vector2 cast_to;
	
	Set<RID> exclude;
	real_t margin;
	uint32_t collision_mask;
	bool exclude_parent_body;
	bool collide_with_areas;
	bool collide_with_bodies;
	
	// Result
	int max_results;
	Vector<Physics2DDirectSpaceState::ShapeRestInfo> result;
	bool collided;
	real_t collision_safe_distance;
	real_t collision_unsafe_distance;
	
	Array _get_collision_result() const;

protected:
	void _notification(int p_what);
	void _update_shapecast_state();
	static void _bind_methods();

public:
	void set_collide_with_areas(bool p_clip);
	bool is_collide_with_areas_enabled() const;

	void set_collide_with_bodies(bool p_clip);
	bool is_collide_with_bodies_enabled() const;

	void set_enabled(bool p_enabled);
	bool is_enabled() const;
	
	void set_shape(const Ref<Shape2D> &p_shape);
	Ref<Shape2D> get_shape() const;
	
	void set_cast_to(const Vector2 &p_point);
	Vector2 get_cast_to() const;
	
	void set_margin(real_t p_margin);
	real_t get_margin() const;
	
	void set_max_results(int p_max_results);
	int get_max_results() const;

	void set_collision_mask(uint32_t p_mask);
	uint32_t get_collision_mask() const;

	void set_collision_mask_bit(int p_bit, bool p_value);
	bool get_collision_mask_bit(int p_bit) const;

	void set_exclude_parent_body(bool p_exclude_parent_body);
	bool get_exclude_parent_body() const;

	void force_shapecast_update();
	bool is_colliding() const;
	
	int get_collision_count() const;
	Object *get_collider(int p_idx) const;
	int get_collider_shape(int p_idx) const;
	Vector2 get_collision_point(int p_idx) const;
	Vector2 get_collision_normal(int p_idx) const;
	
	Object *get_closest_collider() const;
	int get_closest_collider_shape() const;
	Vector2 get_closest_collision_point() const;
	Vector2 get_closest_collision_normal() const;
	real_t get_closest_collision_safe_distance() const;
	real_t get_closest_collision_unsafe_distance() const;

	void add_exception_rid(const RID &p_rid);
	void add_exception(const Object *p_object);
	void remove_exception_rid(const RID &p_rid);
	void remove_exception(const Object *p_object);
	void clear_exceptions();
	
	String get_configuration_warning() const;

	ShapeCast2D();
};

#endif // SHAPE_CAST_2D_H
