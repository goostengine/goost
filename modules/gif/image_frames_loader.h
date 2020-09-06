#ifndef IMAGE_FRAMES_LOADER_H
#define IMAGE_FRAMES_LOADER_H

#include "image_frames.h"

#include "core/io/resource_loader.h"
#include "core/os/file_access.h"

class ImageFramesFormatLoader {
public:
	virtual Error load_image_frames(Ref<ImageFrames> &r_image_frames, FileAccess *f, int max_frames = 0) const = 0;
	virtual void get_recognized_extensions(List<String> *p_extensions) const = 0;
	virtual bool recognize(String p_extension);
	virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = NULL) const = 0;

	virtual ~ImageFramesFormatLoader() {}
};

class ResourceFormatLoaderImageFrames : public ResourceFormatLoader {
	static Vector<ImageFramesFormatLoader *> loaders;
public:
	static void add_image_frames_format_loader(ImageFramesFormatLoader *p_loader);
	static void remove_image_frames_format_loader(ImageFramesFormatLoader *p_loader);

	virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = NULL);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

#endif // IMAGE_FRAMES_LOADER_H  
