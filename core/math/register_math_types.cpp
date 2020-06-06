#include "register_math_types.h"

#include "core/engine.h"

#include "2d/geometry/geometry_extension.h"
#include "2d/geometry/geometry_extension_bind.h"

static _GeometryExtension2D *_geometry_extension = nullptr;

namespace goost {

void register_math_types() {
	_geometry_extension = memnew(_GeometryExtension2D);
	GeometryExtension2D::initialize();
	
	ClassDB::register_class<_GeometryExtension2D>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("GeometryExtension2D", _GeometryExtension2D::get_singleton()));
	
	ClassDB::register_class<PolyBooleanParameters2D>();
	ClassDB::register_class<PolyOffsetParameters2D>();
	ClassDB::register_class<PolyDecompParameters2D>();
	
	ClassDB::register_class<PolyNode2D>();
}

void unregister_math_types() {
	memdelete(_geometry_extension);
	GeometryExtension2D::finalize();
}

} // namespace goost
