#ifndef GIF_H
#define GIF_H

#include "image_frames_loader.h"

struct GifFileType;

class Gif {
	GifFileType *gif;

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

	Gif();
};

#endif // GIF_H
