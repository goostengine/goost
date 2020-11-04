#ifndef GOOST_RANDOM_2D_H
#define GOOST_RANDOM_2D_H

#include "../random.h"

class Random2D : public Random {
	GDCLASS(Random2D, Random);

private:
	static Random2D *singleton;

protected:
	static void _bind_methods();

public:
	static Random2D *get_singleton() { return singleton; }
	virtual Ref<Random> new_instance() const override { return memnew(Random2D); }

	real_t get_rotation();
	Vector2 get_direction(); // Unit vector.

	Vector2 point_in_region(const Vector2 &p_pos, const Vector2 &p_end);
	Vector2 point_in_circle(real_t p_min_radius = 0.0, real_t p_max_radius = 1.0);
	Vector2 point_in_triangle(const Vector<Point2> &p_triangle);
	Variant point_in_polygon(const Variant &p_polygon, int p_point_count = 1);

	Random2D() {
		if (!singleton) {
			randomize(); // Only the global one is randomized automatically.
			singleton = this;
		}
	}
};

#endif // GOOST_RANDOM_2D_H
