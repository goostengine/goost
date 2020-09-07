#ifndef IMAGE_FRAMES_LOADER_GIF_H
#define IMAGE_FRAMES_LOADER_GIF_H

#include "image_frames_loader.h"

class ImageFramesLoaderGIF : public ImageFramesFormatLoader {
public:
	virtual Error load_image_frames(Ref<ImageFrames> &r_image_frames, FileAccess *f, int max_frames = 0) const;
	virtual void get_recognized_extensions(List<String> *p_extensions) const;

	ImageFramesLoaderGIF();
};

struct GifFileType;

class Gif {
	GifFileType *gif = nullptr;

	enum SourceType {
		FILE,
		BUFFER
	};
	Error parse_error(Error parse_error, const String &message);
	Error gif_error(int gif_error);

	Error _open(void *source, SourceType source_type);
	Error _load_frames(Ref<ImageFrames> &r_image_frames, int max_frames = 0);
	Error _close();

public:
	Error load_from_file_access(Ref<ImageFrames> &r_image_frames, FileAccess *f, int max_frames = 0);
	Error load_from_buffer(Ref<ImageFrames> &r_image_frames, const PoolByteArray &p_data, int max_frames = 0);
};

#endif // IMAGE_FRAMES_LOADER_GIF_H
