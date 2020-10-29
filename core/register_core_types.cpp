#include "register_core_types.h"

#include "image/register_image_types.h"
#include "math/register_math_types.h"
#include "types/list.h"
#include "types/variant_resource.h"

namespace goost {

void register_core_types() {
	ClassDB::register_class<ListNode>();
	ClassDB::register_class<LinkedList>();

	ClassDB::register_class<VariantResource>();
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
