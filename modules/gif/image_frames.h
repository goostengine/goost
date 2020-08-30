#ifndef IMAGE_FRAMES_H
#define IMAGE_FRAMES_H

#include "core/image.h"

class ImageFrames;

typedef Error (*LoadImageFramesFunction)(Ref<ImageFrames> &r_image_frames, const Variant &source, int max_frames);

class ImageFrames : public Resource {
	GDCLASS(ImageFrames, Resource);

	struct Frame {
		Ref<Image> image;
		float delay;
	};
	Vector<Frame> frames;

protected:
	static void _bind_methods();

public:
	static LoadImageFramesFunction _load_gif;

	Error load(const String &p_path, int max_frames = 0);
	Error load_gif_from_buffer(const PoolByteArray &p_data, int max_frames = 0);

	void add_frame(const Ref<Image> &p_image, float p_delay, int p_idx = -1);
	void remove_frame(int p_idx);

	void set_image(int p_idx, const Ref<Image> &p_image);
	Ref<Image> get_image(int p_idx) const;

	void set_delay(int p_idx, float p_delay);
	float get_delay(int p_idx) const;

	int get_frame_count() const;

	void clear();
};

#endif // IMAGE_FRAMES_H
