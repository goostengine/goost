#include "register_image_types.h"

#include "core/engine.h"

#include "image/image_extension.h"
#include "image/image_extension_bind.h"
#include "image/image_indexed.h"
#include "image/image_blender.h"
#include "image/drivers/png/image_loader_indexed_png.h"
#include "image/drivers/png/resource_saver_indexed_png.h"

static _ImageExtension *_image_extension = nullptr;
static ImageLoaderIndexedPNG *image_loader_indexed_png;
static Ref<ResourceSaverIndexedPNG> resource_saver_indexed_png;

namespace goost {

void register_image_types() {
	_image_extension = memnew(_ImageExtension);

	ClassDB::register_class<_ImageExtension>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("ImageExtension", _ImageExtension::get_singleton()));

	ClassDB::register_class<ImageIndexed>();

	image_loader_indexed_png = memnew(ImageLoaderIndexedPNG);
	ImageLoader::add_image_format_loader(image_loader_indexed_png);

	resource_saver_indexed_png.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_indexed_png);

	ClassDB::register_class<ImageBlender>();
}

void unregister_image_types() {
	memdelete(_image_extension);

	if (image_loader_indexed_png)
		memdelete(image_loader_indexed_png);

	ResourceSaver::remove_resource_format_saver(resource_saver_indexed_png);
	resource_saver_indexed_png.unref();
}

} // namespace goost
