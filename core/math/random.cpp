#include "random.h"

#include "core/method_bind_ext.gen.inc"

Random *Random::singleton = nullptr;

uint32_t Random::get_number() {
	return randi();
}

real_t Random::get_value() {
	return randf();
}

Color Random::get_color() {
	Color color;
	// "Not too dark" version:
	// color.set_hsv(randf(), 1.0, randf_range(0.5, 1.0));
	color = Color(randf(), randf(), randf());
	return color;
}

bool Random::get_condition() {
	return randf() >= 0.5;
}

Variant Random::range(const Variant &p_from, const Variant &p_to) {
	ERR_FAIL_COND_V_MSG(p_from.get_type() != p_to.get_type(), Variant(), "Incompatible types.");

	switch (p_from.get_type()) {
		case Variant::INT: {
			int from = p_from;
			int to = p_to;
			return randi_range(from, to);
		} break;
		case Variant::REAL: {
			real_t from = p_from;
			real_t to = p_to;
			return randf_range(from, to);
		} break;
		default: {
			Variant ret;
			Variant::interpolate(p_from, p_to, randf(), ret);
			return ret;
		}
	}
	return Variant();
}

Color Random::color_hsv(real_t h_min, real_t h_max, real_t s_min, real_t s_max, real_t v_min, real_t v_max, real_t a_min, real_t a_max) {
	real_t h = randf_range(h_min, h_max);
	real_t s = randf_range(s_min, s_max);
	real_t v = randf_range(v_min, v_max);
	real_t a = randf_range(a_min, a_max);
	return Color(h, s, v, a);
}

Color Random::color_rgb(real_t r_min, real_t r_max, real_t g_min, real_t g_max, real_t b_min, real_t b_max, real_t a_min, real_t a_max) {
	real_t r = randf_range(r_min, r_max);
	real_t g = randf_range(g_min, g_max);
	real_t b = randf_range(b_min, b_max);
	real_t a = randf_range(a_min, a_max);
	return Color(r, g, b, a);
}

Vector2 Random::circle_point(real_t p_radius_min, real_t p_radius_max) {
	real_t r2_max = p_radius_max * p_radius_max;
	real_t r2_min = p_radius_min * p_radius_min;
	real_t r = Math::sqrt(randf_range(0.0, 1.0) * (r2_max - r2_min) + r2_min);
	real_t t = randf_range(0.0, 1.0) * Math_TAU;
	return Vector2(r * Math::cos(t), r * Math::sin(t));
}

Variant Random::choice(const Variant &p_sequence) {
	switch (p_sequence.get_type()) {
		case Variant::STRING: {
			String str = p_sequence;
			ERR_FAIL_COND_V_MSG(str.empty(), Variant(), "String is empty.");
			return str.substr(randi() % str.length(), 1); // Not size().
		} break;
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY:
		case Variant::ARRAY: {
			Array arr = p_sequence;
			ERR_FAIL_COND_V_MSG(arr.empty(), Variant(), "Array is empty.");
			return arr[randi() % arr.size()];
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported type: the sequence must be indexable.")
		}
	}
	return Variant();
}

void Random::_bind_methods() {
	ClassDB::bind_method(D_METHOD("new_instance"), &Random::new_instance);

	ClassDB::bind_method(D_METHOD("get_value"), &Random::get_value);
	ClassDB::bind_method(D_METHOD("get_number"), &Random::get_number);
	ClassDB::bind_method(D_METHOD("get_color"), &Random::get_color);
	ClassDB::bind_method(D_METHOD("get_condition"), &Random::get_condition);

	ClassDB::bind_method(D_METHOD("range", "from", "to"), &Random::range);

	ClassDB::bind_method(D_METHOD("color_hsv", "hue_min", "hue_max", "saturation_min", "saturation_max", "value_min", "value_max", "alpha_min", "alpha_max"),
			&Random::color_hsv, DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(1.0), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("color_rgb", "red_min", "red_max", "green_min", "green_max", "blue_min", "blue_max", "alpha_min", "alpha_max"),
			&Random::color_hsv, DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(1.0), DEFVAL(1.0));

	ClassDB::bind_method(D_METHOD("circle_point", "radius_min", "radius_max"), &Random::circle_point, DEFVAL(1.0), DEFVAL(1.0));

	ClassDB::bind_method(D_METHOD("choice", "from_sequence"), &Random::choice);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "number"), "", "get_number");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "value"), "", "get_value");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "", "get_color");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "condition"), "", "get_condition");
}
