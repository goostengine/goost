#include "register_geometry_types.h"

#include "goost/classes_enabled.gen.h"

static _GoostGeometry2D *_goost_geometry_2d = nullptr;
static Ref<_PolyBoolean2D> _poly_boolean_2d;
static Ref<_PolyOffset2D> _poly_offset_2d;
static Ref<_PolyDecomp2D> _poly_decomp_2d;

static Ref<Random2D> _random_2d;

namespace goost {

void register_geometry_types() {
#ifdef GOOST_GoostGeometry2D
	_goost_geometry_2d = memnew(_GoostGeometry2D);
	ClassDB::register_class<_GoostGeometry2D>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("GoostGeometry2D", _GoostGeometry2D::get_singleton()));
#endif
#ifdef GOOST_PolyNode2D
	PolyBackends2D::initialize();
#endif
#ifdef GOOST_PolyBoolean2D
	_poly_boolean_2d.instance();
	ClassDB::register_class<_PolyBoolean2D>();
	Object *poly_boolean_2d = Object::cast_to<Object>(_PolyBoolean2D::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("PolyBoolean2D", poly_boolean_2d));
#endif
	ClassDB::register_class<PolyBooleanParameters2D>();
	ClassDB::register_class<PolyNode2D>();

#ifdef GOOST_PolyOffset2D
	_poly_offset_2d.instance();
	ClassDB::register_class<_PolyOffset2D>();
	Object *poly_offset_2d = Object::cast_to<Object>(_PolyOffset2D::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("PolyOffset2D", poly_offset_2d));
#endif
	ClassDB::register_class<PolyOffsetParameters2D>();

#ifdef GOOST_PolyDecomp2D
	_poly_decomp_2d.instance();
	ClassDB::register_class<_PolyDecomp2D>();
	Object *poly_decomp_2d = Object::cast_to<Object>(_PolyDecomp2D::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("PolyDecomp2D", poly_decomp_2d));
#endif
	ClassDB::register_class<PolyDecompParameters2D>();

#ifdef GOOST_Random2D
	_random_2d.instance();
	ClassDB::register_class<Random2D>();
	Object *random_2d = Object::cast_to<Object>(Random2D::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("Random2D", random_2d));
#endif
}

void unregister_geometry_types() {
#ifdef GOOST_GoostGeometry2D
	memdelete(_goost_geometry_2d);
#endif
#ifdef GOOST_PolyNode2D
	PolyBackends2D::finalize();
#endif
#ifdef GOOST_PolyBoolean2D
	_poly_boolean_2d.unref();
#endif
#ifdef GOOST_PolyOffset2D
	_poly_offset_2d.unref();
#endif
#ifdef GOOST_PolyDecomp2D
	_poly_decomp_2d.unref();
#endif

#ifdef GOOST_Random2D
	_random_2d.unref();
#endif
}

} // namespace goost
