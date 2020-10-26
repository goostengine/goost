#include "random.h"

Random *Random::singleton = nullptr;

void Random::_bind_methods() {
	ClassDB::bind_method(D_METHOD("new_instance"), &Random::new_instance);
}
