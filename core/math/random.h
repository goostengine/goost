#pragma once

#include "core/math/random_number_generator.h"

class Random : public RandomNumberGenerator {
	GDCLASS(Random, RandomNumberGenerator);

private:
	static Random *singleton;

protected:
	static void _bind_methods();

public:
	static Random *get_singleton() { return singleton; }
	virtual Ref<Reference> new_instance() const { return memnew(Random); }

	uint32_t get_number();
	real_t get_value();
	Color get_color();
	bool get_condition();

	Color color_hsv(float h_min = 0.0, float h_max = 1.0, float s_min = 0.0, float s_max = 1.0, float v_min = 0.0, float v_max = 1.0, float a_min = 1.0, float a_max = 1.0);
	Color color_rgb(float r_min = 0.0, float r_max = 1.0, float g_min = 0.0, float g_max = 1.0, float b_min = 0.0, float b_max = 1.0, float a_min = 1.0, float a_max = 1.0);

	Variant range(const Variant &p_from, const Variant &p_to);
	Variant choice(const Variant &p_sequence);
	Array choices(const Variant &p_sequence, int p_count = 1, const PoolIntArray &p_weights = Variant(), bool p_is_cumulative = false);
	void shuffle(Array p_array);
	bool decision(float probability);

	Random() {
		if (!singleton) {
			randomize(); // Only the global one is randomized automatically.
			singleton = this;
		}
	}
};
