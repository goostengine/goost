#ifndef GOOST_RANDOM_H
#define GOOST_RANDOM_H

#include "core/math/random_number_generator.h"

class Random : public RandomNumberGenerator {
	GDCLASS(Random, RandomNumberGenerator);

private:
	static Random *singleton;

protected:
	static void _bind_methods();

public:
	static Random *get_singleton() { return singleton; }
	Ref<Random> new_instance() const { return memnew(Random); }

	uint32_t get_number();
	real_t get_value();
	Color get_color();
	bool get_condition();

	Variant range(const Variant &p_from, const Variant &p_to);

	Color color_hsv(real_t h_min = 0.0, real_t h_max = 1.0, real_t s_min = 0.0, real_t s_max = 1.0, real_t v_min = 0.0, real_t v_max = 1.0, real_t a_min = 1.0, real_t a_max = 1.0);
	Color color_rgb(real_t r_min = 0.0, real_t r_max = 1.0, real_t g_min = 0.0, real_t g_max = 1.0, real_t b_min = 0.0, real_t b_max = 1.0, real_t a_min = 1.0, real_t a_max = 1.0);

	Vector2 circle_point(real_t p_min_radius = 1.0, real_t p_max_radius = 1.0); // Unit length by default.

	Variant choice(const Variant &p_sequence);

	Random() {
		if (!singleton) {
			singleton = this;
		}
	}
};

#endif // GOOST_RANDOM_H
