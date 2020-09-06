#ifndef IMAGE_FRAMES_LOADER_GIF_H
#define IMAGE_FRAMES_LOADER_GIF_H

#include "image_frames_loader.h"

class ImageFramesLoaderGIF : public ImageFramesFormatLoader {
public:
	virtual Error load_image_frames(Ref<ImageFrames> &r_image_frames, FileAccess *f, int max_frames = 0) const;
	virtual void get_recognized_extensions(List<String> *p_extensions) const;

	ImageFramesLoaderGIF();
};

#endif // IMAGE_FRAMES_LOADER_GIF_H
