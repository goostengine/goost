#include "gif.h"

#include "scene/resources/texture.h"

#include "image_frames.h"
#include "image_frames_loader_gif.h"

static Error _load_gif(Ref<ImageFrames> &r_image_frames, const Variant &source, int max_frames) {
	Gif gif;
	if (source.get_type() == Variant::STRING) {
		Error err;
		FileAccess *f = FileAccess::open(source, FileAccess::READ, &err);
		if (!f) {
			ERR_PRINTS("Error opening file '" + String(source) + "'.");
			return err;
		}
		err = gif.load_from_file_access(r_image_frames, f, max_frames);
		f->close();
		memdelete(f);
		return err;
	} else {
		return gif.load_from_buffer(r_image_frames, source, max_frames);
	}
}

Error ImageFramesLoaderGIF::load_image_frames(Ref<ImageFrames> &r_image_frames, FileAccess *f, int max_frames) const {
	Gif gif;
	return gif.load_from_file_access(r_image_frames, f, max_frames);
}

void ImageFramesLoaderGIF::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("gif");
}

ImageFramesLoaderGIF::ImageFramesLoaderGIF() {
	ImageFrames::_load_gif = _load_gif;
}
