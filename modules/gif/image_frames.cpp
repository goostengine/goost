#include "image_frames.h"

LoadImageFramesFunction ImageFrames::_load_gif = nullptr;

Error ImageFrames::load(const String &p_path, int max_frames) {
	clear();
	String ext = p_path.get_extension().to_lower();
	if (ext == "gif") {
		Ref<ImageFrames> image_frames = Ref<ImageFrames>(this);
		return _load_gif(image_frames, p_path, max_frames);
	} else {
		ERR_PRINTS("Unrecognized image: " + p_path);
		return ERR_FILE_UNRECOGNIZED;
	}
}

Error ImageFrames::load_gif_from_buffer(const PoolByteArray &p_data, int max_frames) {
	clear();
	if (p_data[0] == 'G') {
		Ref<ImageFrames> image_frames = Ref<ImageFrames>(this);
		return _load_gif(image_frames, p_data, max_frames);
	} else {
		ERR_PRINTS("Unrecognized image.");
		return ERR_FILE_UNRECOGNIZED;
	}
}

void ImageFrames::add_frame(const Ref<Image> &p_image, float p_delay, int p_idx) {
	ERR_FAIL_COND(p_idx > get_frame_count() - 1);
	Frame frame;
	frame.image = p_image;
	frame.delay = p_delay;
	if (p_idx < 0) {
		frames.push_back(frame);
	}
	else {
		frames.set(p_idx, frame);
	}
}

void ImageFrames::remove_frame(int p_idx) {
	ERR_FAIL_COND(p_idx < 0);
	ERR_FAIL_COND(p_idx > get_frame_count() - 1);
	frames.remove(p_idx);
}

void ImageFrames::set_image(int p_idx, const Ref<Image> &p_image) {
	ERR_FAIL_COND(p_idx < 0);
	ERR_FAIL_COND(p_idx > get_frame_count() - 1);
	frames.write[p_idx].image = p_image;
}

Ref<Image> ImageFrames::get_image(int p_idx) const {
	ERR_FAIL_COND_V(p_idx < 0, Ref<Image>());
	ERR_FAIL_COND_V(p_idx > get_frame_count() - 1, Ref<Image>());
	return frames[p_idx].image;
}

void ImageFrames::set_delay(int p_idx, float p_delay) {
	ERR_FAIL_COND(p_idx < 0);
	ERR_FAIL_COND(p_idx > get_frame_count() - 1);
	frames.write[p_idx].delay = p_delay;
}

float ImageFrames::get_delay(int p_idx) const {
	ERR_FAIL_COND_V(p_idx < 0, 0);
	ERR_FAIL_COND_V(p_idx > get_frame_count() - 1, 0);
	return frames[p_idx].delay;
}

int ImageFrames::get_frame_count() const {
	return frames.size();
}

void ImageFrames::clear() {
	frames.clear();
}

void ImageFrames::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load", "path", "max_frames"), &ImageFrames::load, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("load_gif_from_buffer", "data", "max_frames"), &ImageFrames::load_gif_from_buffer, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("add_frame", "image", "delay", "idx"), &ImageFrames::add_frame, DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("remove_frame", "idx"), &ImageFrames::remove_frame);

	ClassDB::bind_method(D_METHOD("set_image", "idx", "image"), &ImageFrames::set_image);
	ClassDB::bind_method(D_METHOD("get_image", "idx"), &ImageFrames::get_image);

	ClassDB::bind_method(D_METHOD("set_delay", "idx", "delay"), &ImageFrames::set_delay);
	ClassDB::bind_method(D_METHOD("get_delay", "idx"), &ImageFrames::get_delay);

	ClassDB::bind_method(D_METHOD("get_frame_count"), &ImageFrames::get_frame_count);

	ClassDB::bind_method(D_METHOD("clear"), &ImageFrames::clear);
}
