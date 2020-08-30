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
	p_extensions->push_back("atex");
	p_extensions->push_back("gif");
}

#define HEADER_SIZE 6

RES ImageFramesLoaderGIF::load(const String &p_path, const String &p_original_path, Error *r_error) const {
	Error err;
	FileAccess *f = FileAccess::open(p_path, FileAccess::READ, &err);
	if (!f) {
		if (r_error)
			*r_error = err;
		return RES();
	}
	uint8_t header[HEADER_SIZE] = { 0 };
	int header_size = f->get_buffer(header, HEADER_SIZE);

	if (header_size != HEADER_SIZE) {
		if (r_error) {
			*r_error = ERR_FILE_CORRUPT;
		}
		return RES();
	}
	int tex_flags = 0;
	int max_frames = 0;

	uint8_t imported_header[HEADER_SIZE] = { 'G', 'D', 'A', 'T', 'E', 'X' };
	if (memcmp(&header[0], &imported_header[0], HEADER_SIZE) == 0) { // The file is imported.
		tex_flags = f->get_32();
		max_frames = f->get_32();
	} else {
		if (header[0] != 'G') {
			f->close();
			memdelete(f);
			if (r_error)
				*r_error = ERR_FILE_CORRUPT;
			return RES();
		}
		f->seek(0); // Reset the cursor to the beginning of the file.
	}
	Ref<ImageFrames> image_frames = memnew(ImageFrames);
	load_image_frames(image_frames, f, max_frames);

	f->close();
	memdelete(f);

	if (err != OK) {
		if (r_error) {
			*r_error = err;
		}
		return RES();
	}
	if (max_frames <= 0 || max_frames > 256) {
		max_frames = 256;
	}
	int frames_size = MIN(image_frames->get_frame_count(), max_frames);

	Ref<AnimatedTexture> animated_texture = memnew(AnimatedTexture);
	animated_texture->set_fps(0); // Allow each frame to have a custom delay.

	for (int i = 0; i < frames_size; i++) {
		Ref<ImageTexture> texture = memnew(ImageTexture);
		texture->create_from_image(image_frames->get_image(i), tex_flags);
		animated_texture->set_frame_texture(i, texture);
		animated_texture->set_frame_delay(i, image_frames->get_delay(i));
	}
	animated_texture->set_frames(frames_size);

	if (r_error) {
		*r_error = OK;
	}
	return animated_texture;
}

ImageFramesLoaderGIF::ImageFramesLoaderGIF() {
	ImageFrames::_load_gif = _load_gif;
}
