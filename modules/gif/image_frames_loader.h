#pragma once

#include "image_frames.h"

#include "core/io/resource_loader.h"
#include "core/os/file_access.h"

class ImageFramesFormatLoader {
	friend class ImageFramesLoader;
	friend class ResourceFormatLoaderImageFrames;
public:
	virtual Error load_image_frames(Ref<ImageFrames> &r_image_frames, FileAccess *p_f, int p_max_frames = 0) const = 0;
	virtual void get_recognized_extensions(List<String> *p_extensions) const = 0;
	bool recognize(String p_extension);

	virtual ~ImageFramesFormatLoader() {}
};

class ImageFramesLoader {
	static Vector<ImageFramesFormatLoader *> loader;
	friend class ResourceFormatLoaderImageFrames;
public:
	static Error load_image_frames(String p_file, Ref<ImageFrames> p_image, FileAccess *p_custom = nullptr, int p_max_frames = 0);
	static void get_recognized_extensions(List<String> *p_extensions);
	static ImageFramesFormatLoader *recognize(const String &p_extension);

	static void add_image_frames_format_loader(ImageFramesFormatLoader *p_loader);
	static void remove_image_frames_format_loader(ImageFramesFormatLoader *p_loader);

	static void cleanup();
};

class ResourceFormatLoaderImageFrames : public ResourceFormatLoader {
public:
	virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class ResourceFormatLoaderAnimatedTexture : public ResourceFormatLoader {
public:
	virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class ResourceFormatLoaderSpriteFrames : public ResourceFormatLoader {
public:
	virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

