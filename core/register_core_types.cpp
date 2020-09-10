#include "register_core_types.h"

#include "core/bind/list.h"

#include "image/register_image_types.h"
#include "math/register_math_types.h"

namespace goost {

void register_core_types() {
	ClassDB::register_class<VariantList>();
	ClassDB::register_class<VariantListElement>();
#ifdef GOOST_IMAGE_ENABLED
	register_image_types();
#endif
#ifdef GOOST_MATH_ENABLED
	register_math_types();
#endif
}

void unregister_core_types() {
#ifdef GOOST_IMAGE_ENABLED
	unregister_image_types();
#endif
#ifdef GOOST_MATH_ENABLED
	unregister_math_types();
#endif
}

} // namespace goost
