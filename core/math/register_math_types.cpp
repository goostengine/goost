#include "register_math_types.h"

#include "core/engine.h"

#include "2d/geometry/goost_geometry_2d.h"
#include "2d/geometry/goost_geometry_2d_bind.h"

static _GoostGeometry2D *_goost_geometry_2d = nullptr;

namespace goost {

void register_math_types() {
	_goost_geometry_2d = memnew(_GoostGeometry2D);
	GoostGeometry2D::initialize();

	ClassDB::register_class<_GoostGeometry2D>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("GoostGeometry2D", _GoostGeometry2D::get_singleton()));

	ClassDB::register_class<PolyBooleanParameters2D>();
	ClassDB::register_class<PolyOffsetParameters2D>();
	ClassDB::register_class<PolyDecompParameters2D>();

	ClassDB::register_class<PolyNode2D>();
}

void unregister_math_types() {
	memdelete(_goost_geometry_2d);
	GoostGeometry2D::finalize();
}

} // namespace goost
