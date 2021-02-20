#ifndef GOOST_REGISTER_TYPES
#define GOOST_REGISTER_TYPES

#include "core/engine.h"

#include "scene/2d/editor/poly_node_2d_editor_plugin.h"
#include "scene/2d/editor/visual_shape_2d_editor_plugin.h"
#include "scene/2d/poly_collision_shape_2d.h"
#include "scene/2d/poly_generators_2d.h"
#include "scene/2d/poly_shape_2d.h"
#include "scene/2d/visual_shape_2d.h"
#include "scene/physics/2d/shape_cast_2d.h"
#include "scene/resources/gradient_texture_2d.h"

#include "core/goost_engine.h"
#include "core/image/goost_image.h"
#include "core/image/goost_image_bind.h"
#include "core/image/image_blender.h"
#include "core/image/image_indexed.h"
#include "core/invoke_state.h"
#include "core/math/2d/geometry/goost_geometry_2d.h"
#include "core/math/2d/geometry/goost_geometry_2d_bind.h"
#include "core/math/2d/geometry/poly/boolean/poly_boolean.h"
#include "core/math/2d/geometry/poly/decomp/poly_decomp.h"
#include "core/math/2d/geometry/poly/offset/poly_offset.h"
#include "core/math/2d/geometry/poly/poly_backends.h"
#include "core/math/2d/random_2d.h"
#include "core/math/random.h"
#include "core/types/grid_2d.h"
#include "core/types/linked_list.h"
#include "core/types/variant_resource.h"

void register_goost_types();
void unregister_goost_types();

namespace goost {
template <typename T>
void register_class() {
	ClassDB::register_class<T>();
}
} // namespace goost

#endif // GOOST_REGISTER_TYPES
