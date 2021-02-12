#include "register_image_types.h"
#include "goost/register_types.h"
#include "goost/classes_enabled.gen.h"

#include "core/engine.h"

#include "drivers/png/image_loader_indexed_png.h"
#include "drivers/png/resource_saver_indexed_png.h"
#include "goost_image.h"
#include "goost_image_bind.h"
#include "image_blender.h"
#include "image_indexed.h"

static _GoostImage *_goost_image = nullptr;
static ImageLoaderIndexedPNG *image_loader_indexed_png;
static Ref<ResourceSaverIndexedPNG> resource_saver_indexed_png;

namespace goost {

void register_image_types() {
#ifdef GOOST_CLASS_GOOSTIMAGE_ENABLED
	_goost_image = memnew(_GoostImage);
	ClassDB::register_class<_GoostImage>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("GoostImage", _GoostImage::get_singleton()));
#endif
#ifdef GOOST_CLASS_IMAGEINDEXED_ENABLED
	ClassDB::register_class<ImageIndexed>();

	image_loader_indexed_png = memnew(ImageLoaderIndexedPNG);
	ImageLoader::add_image_format_loader(image_loader_indexed_png);

	resource_saver_indexed_png.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_indexed_png);
#endif

	GOOST_REGISTER_CLASS(ImageBlender);
}

void unregister_image_types() {
#ifdef GOOST_CLASS_GOOSTIMAGE_ENABLED
	memdelete(_goost_image);
#endif
#ifdef GOOST_CLASS_IMAGEINDEXED_ENABLED
	if (image_loader_indexed_png) {
		memdelete(image_loader_indexed_png);
	}
	ResourceSaver::remove_resource_format_saver(resource_saver_indexed_png);
	resource_saver_indexed_png.unref();
#endif
}

} // namespace goost
