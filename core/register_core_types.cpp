#include "register_core_types.h"

#include "core/engine.h"

#include "image/image_extension.h"
#include "image/image_extension_bind.h"
#include "image/image_indexed.h"
#include "image/image_blender.h"
#include "image/drivers/png/image_loader_indexed_png.h"
#include "image/drivers/png/resource_saver_indexed_png.h"

#include "math/2d/geometry/geometry_extension.h"
#include "math/2d//geometry/geometry_extension_bind.h"

static _ImageExtension *_image_extension = nullptr;
static ImageLoaderIndexedPNG *image_loader_indexed_png;
static Ref<ResourceSaverIndexedPNG> resource_saver_indexed_png;

static _GeometryExtension2D *_geometry_extension = nullptr;

namespace goost {

void register_core_types() {
	// Image ===================================================================
	_image_extension = memnew(_ImageExtension);

	ClassDB::register_class<_ImageExtension>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("ImageExtension", _ImageExtension::get_singleton()));

	ClassDB::register_class<ImageIndexed>();

	image_loader_indexed_png = memnew(ImageLoaderIndexedPNG);
	ImageLoader::add_image_format_loader(image_loader_indexed_png);

	resource_saver_indexed_png.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_indexed_png);

	ClassDB::register_class<ImageBlender>();

	// Geometry ================================================================
	_geometry_extension = memnew(_GeometryExtension2D);
	GeometryExtension2D::initialize();
	
	ClassDB::register_class<_GeometryExtension2D>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("GeometryExtension2D", _GeometryExtension2D::get_singleton()));
	
	ClassDB::register_class<PolyBooleanParameters2D>();
	ClassDB::register_class<PolyOffsetParameters2D>();
	ClassDB::register_class<PolyDecompParameters2D>();
	
	ClassDB::register_class<PolyNode2D>();
}

void unregister_core_types() {
	// Image ===================================================================
	memdelete(_image_extension);

	if (image_loader_indexed_png)
		memdelete(image_loader_indexed_png);

	ResourceSaver::remove_resource_format_saver(resource_saver_indexed_png);
	resource_saver_indexed_png.unref();
	
	// Geometry ================================================================
	memdelete(_geometry_extension);
	GeometryExtension2D::finalize();
}

} // namespace goost
