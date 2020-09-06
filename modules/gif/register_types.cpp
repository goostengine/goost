#include "register_types.h"

#include "image_frames.h"
#include "image_frames_loader.h"
#include "image_frames_loader_gif.h"

#include "editor/import/resource_importer_animated_texture.h"

#include "gif.h"

static ImageFramesLoaderGIF *image_frames_loader_gif = nullptr;
static Ref<ResourceFormatLoaderImageFrames> resource_format_image_frames;
static Ref<ResourceFormatLoaderAnimatedTexture> resource_format_animated_texture;

void register_gif_types() {
	image_frames_loader_gif = memnew(ImageFramesLoaderGIF);
	ImageFramesLoader::add_image_frames_format_loader(image_frames_loader_gif);

	resource_format_image_frames.instance();
	ResourceLoader::add_resource_format_loader(resource_format_image_frames);

	resource_format_animated_texture.instance();
	ResourceLoader::add_resource_format_loader(resource_format_animated_texture);

	ClassDB::register_class<ImageFrames>();

#ifdef TOOLS_ENABLED
	Ref<ResourceImporterAnimatedTexture> import_animated_texture;
	import_animated_texture.instance();
	ResourceFormatImporter::get_singleton()->add_importer(import_animated_texture);
#endif
}

void unregister_gif_types() {
	ImageFramesLoader::remove_image_frames_format_loader(image_frames_loader_gif);
	if (image_frames_loader_gif) {
		memdelete(image_frames_loader_gif);
	}
	ResourceLoader::remove_resource_format_loader(resource_format_image_frames);
	resource_format_image_frames.unref();

	ResourceLoader::remove_resource_format_loader(resource_format_animated_texture);
	resource_format_animated_texture.unref();
}
