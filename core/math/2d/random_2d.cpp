#include "random_2d.h"

#include "core/method_bind_ext.gen.inc"

Random2D *Random2D::singleton = nullptr;

Vector2 Random2D::get_direction() {
	real_t t = randf_range(0.0, 1.0) * Math_TAU;
	return Vector2(Math::cos(t), Math::sin(t));
}

real_t Random2D::get_rotation() {
	return randf_range(0.0, Math_TAU);
}

Vector2 Random2D::point_in_region(const Vector2 &p_pos, const Vector2 &p_end) {
	Vector2 pos;
	pos.x = randf_range(p_pos.x, p_end.x);
	pos.y = randf_range(p_pos.y, p_end.y);
	return pos;
}

Vector2 Random2D::point_in_circle(real_t p_radius_min, real_t p_radius_max) {
	real_t r2_max = p_radius_max * p_radius_max;
	real_t r2_min = p_radius_min * p_radius_min;
	real_t r = Math::sqrt(randf_range(0.0, 1.0) * (r2_max - r2_min) + r2_min);
	real_t t = randf_range(0.0, 1.0) * Math_TAU;
	return Vector2(r * Math::cos(t), r * Math::sin(t));
}

void Random2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_direction"), &Random2D::get_direction);
	ClassDB::bind_method(D_METHOD("get_rotation"), &Random2D::get_rotation);

	ClassDB::bind_method(D_METHOD("point_in_region", "position_start", "position_end"), &Random2D::point_in_region);
	ClassDB::bind_method(D_METHOD("point_in_circle", "radius_min", "radius_max"), &Random2D::point_in_circle, DEFVAL(0.0), DEFVAL(1.0));

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "direction"), "", "get_direction");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "rotation"), "", "get_rotation");
}
