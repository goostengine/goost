#pragma once

#include "core/object.h"

#define GOOST_CMP_EPSILON 0.000001

class GoostMath : public Object {
	GDCLASS(GoostMath, Object);

private:
	static GoostMath *singleton;

protected:
	static void _bind_methods();

public:
	static GoostMath *get_singleton() { return singleton; }

	bool is_equal_approx(real_t a, real_t b, real_t tolerance = GOOST_CMP_EPSILON);
	bool is_zero_approx(real_t s, real_t tolerance = GOOST_CMP_EPSILON);

	bool is_between(real_t s, real_t a, real_t b);
	bool is_in_range(real_t s, real_t min, real_t max);

	real_t log(real_t x, real_t base = Math_E);
	real_t log2(real_t x) { return ::log2(x); }
	real_t log10(real_t x) { return ::log10(x); }

	Variant catmull_rom(const Variant &p0, const Variant &p1, const Variant &p2, const Variant &p3, float t);
	Variant bezier(const Variant &p0, const Variant &p1, const Variant &p2, const Variant &p3, float t);

	GoostMath() {
		ERR_FAIL_COND_MSG(singleton != nullptr, "Singleton already exists");
		singleton = this;
	}
};

namespace goost {

namespace math {

template <class T>
T catmull_rom(const T &p0, const T &p1, const T &p2, const T &p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4 * p2 - p3) * t2 + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

template <class T>
T bezier(T start, T control_1, T control_2, T end, float t) {
	real_t it = (1.0 - t);
	real_t it2 = it * it;
	real_t it3 = it2 * it;
	real_t t2 = t * t;
	real_t t3 = t2 * t;

	return start * it3 + control_1 * it2 * t * 3.0 + control_2 * it * t2 * 3.0 + end * t3;
}

} // namespace math

} // namespace goost
