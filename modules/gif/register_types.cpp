#include "register_types.h"

#include "image_frames.h"
#include "image_frames_loader.h"
#include "image_frames_loader_gif.h"

#ifdef TOOLS_ENABLED
#include "editor/import/resource_importer_animated_texture.h"
#include "editor/import/resource_importer_sprite_frames.h"
#endif

static ImageFramesLoaderGIF *image_frames_loader_gif = nullptr;

static Ref<ResourceFormatLoaderImageFrames> resource_format_image_frames;
static Ref<ResourceFormatLoaderAnimatedTexture> resource_format_animated_texture;
static Ref<ResourceFormatLoaderSpriteFrames> resource_format_sprite_frames;

void register_gif_types() {
	image_frames_loader_gif = memnew(ImageFramesLoaderGIF);
	ImageFramesLoader::add_image_frames_format_loader(image_frames_loader_gif);

	resource_format_image_frames.instance();
	ResourceLoader::add_resource_format_loader(resource_format_image_frames);

	resource_format_animated_texture.instance();
	ResourceLoader::add_resource_format_loader(resource_format_animated_texture);

	resource_format_sprite_frames.instance();
	ResourceLoader::add_resource_format_loader(resource_format_sprite_frames);

	ClassDB::register_class<ImageFrames>();

#ifdef TOOLS_ENABLED
	Ref<ResourceImporterAnimatedTexture> import_animated_texture;
	import_animated_texture.instance();
	ResourceFormatImporter::get_singleton()->add_importer(import_animated_texture);

	Ref<ResourceImporterSpriteFrames> import_sprite_frames;
	import_sprite_frames.instance();
	ResourceFormatImporter::get_singleton()->add_importer(import_sprite_frames);
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

	ResourceLoader::remove_resource_format_loader(resource_format_sprite_frames);
	resource_format_sprite_frames.unref();
}
