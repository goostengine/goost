#pragma once

#include "core/io/image_loader.h"
#include "goost/core/image/image_indexed.h"

#include <png.h>

class ImageLoaderIndexedPNG : public ImageFormatLoader {
	static void _read_png_data(png_structp png_ptr, png_bytep data, png_size_t p_length);

public:
	static Error _load_image(void *rf_up, png_rw_ptr p_func, Ref<ImageIndexed> p_image);
	virtual Error load_image(Ref<Image> p_image, FileAccess *f, bool p_force_linear, float p_scale);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	ImageLoaderIndexedPNG();
};

