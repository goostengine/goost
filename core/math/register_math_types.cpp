#include "register_math_types.h"
#include "goost/register_types.h"
#include "goost/classes_enabled.gen.h"

#include "core/engine.h"

#include "2d/random_2d.h"
#include "random.h"

#include "2d/geometry/goost_geometry_2d.h"
#include "2d/geometry/goost_geometry_2d_bind.h"
#include "2d/geometry/poly/boolean/poly_boolean.h"
#include "2d/geometry/poly/decomp/poly_decomp.h"
#include "2d/geometry/poly/offset/poly_offset.h"
#include "2d/geometry/poly/poly_backends.h"

static Ref<Random> _random;
static Ref<Random2D> _random_2d;

static _GoostGeometry2D *_goost_geometry_2d = nullptr;
static Ref<_PolyBoolean2D> _poly_boolean_2d;
static Ref<_PolyOffset2D> _poly_offset_2d;
static Ref<_PolyDecomp2D> _poly_decomp_2d;

namespace goost {

void register_math_types() {
#ifdef GOOST_CLASS_RANDOM_ENABLED
	_random.instance();
	ClassDB::register_class<Random>();
	Object *random = Object::cast_to<Object>(Random::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("Random", random));
#endif
#ifdef GOOST_CLASS_RANDOM2D_ENABLED
	_random_2d.instance();
	ClassDB::register_class<Random2D>();
	Object *random_2d = Object::cast_to<Object>(Random2D::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("Random2D", random_2d));
#endif
#ifdef GOOST_CLASS_GOOSTGEOMETRY2D_ENABLED
	_goost_geometry_2d = memnew(_GoostGeometry2D);
	ClassDB::register_class<_GoostGeometry2D>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("GoostGeometry2D", _GoostGeometry2D::get_singleton()));
#endif
	// Can still be used in C++ alone.
	PolyBackends2D::initialize();

#ifdef GOOST_CLASS_POLYBOOLEAN2D_ENABLED
	_poly_boolean_2d.instance();
	ClassDB::register_class<_PolyBoolean2D>();
	Object *poly_boolean_2d = Object::cast_to<Object>(_PolyBoolean2D::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("PolyBoolean2D", poly_boolean_2d));
#endif
	GOOST_REGISTER_CLASS(PolyBooleanParameters2D);
	GOOST_REGISTER_CLASS(PolyNode2D);

#ifdef GOOST_CLASS_POLYOFFSET2D_ENABLED
	_poly_offset_2d.instance();
	ClassDB::register_class<_PolyOffset2D>();
	Object *poly_offset_2d = Object::cast_to<Object>(_PolyOffset2D::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("PolyOffset2D", poly_offset_2d));
#endif
	GOOST_REGISTER_CLASS(PolyOffsetParameters2D);

#ifdef GOOST_CLASS_POLYDECOMP2D_ENABLED
	_poly_decomp_2d.instance();
	ClassDB::register_class<_PolyDecomp2D>();
	Object *poly_decomp_2d = Object::cast_to<Object>(_PolyDecomp2D::get_singleton());
	Engine::get_singleton()->add_singleton(Engine::Singleton("PolyDecomp2D", poly_decomp_2d));
#endif
	GOOST_REGISTER_CLASS(PolyDecompParameters2D);
}

void unregister_math_types() {
#ifdef GOOST_CLASS_RANDOM_ENABLED
	_random.unref();
#endif
#ifdef GOOST_CLASS_RANDOM2D_ENABLED
	_random_2d.unref();
#endif

#ifdef GOOST_CLASS_GOOSTGEOMETRY2D_ENABLED
	memdelete(_goost_geometry_2d);
#endif

	PolyBackends2D::finalize();

#ifdef GOOST_CLASS_POLYBOOLEAN2D_ENABLED
	_poly_boolean_2d.unref();
#endif
#ifdef GOOST_CLASS_POLYOFFSET2D_ENABLED
	_poly_offset_2d.unref();
#endif
#ifdef GOOST_CLASS_POLYDECOMP2D_ENABLED
	_poly_decomp_2d.unref();
#endif
}

} // namespace goost
