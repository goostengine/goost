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
	static Random* get_singleton() { return singleton; }

	Ref<Random> new_instance() const { return memnew(Random); }

	Random() {
		if (!singleton) {
			singleton = this;
		}
	}
};

#endif // GOOST_RANDOM_H
