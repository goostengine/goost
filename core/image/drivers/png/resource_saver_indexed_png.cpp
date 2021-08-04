#include "resource_saver_indexed_png.h"

#include "core/image.h"
#include "core/os/file_access.h"
#include "core/project_settings.h"
#include "scene/resources/texture.h"

#include <png.h>

static void _write_png_data(png_structp png_ptr, png_bytep data, png_size_t p_length) {
	FileAccess *f = (FileAccess *)png_get_io_ptr(png_ptr);
	f->store_buffer((const uint8_t *)data, p_length);
}

Error ResourceSaverIndexedPNG::save(const String &p_path, const RES &p_resource, uint32_t p_flags) {
	Ref<ImageTexture> texture = p_resource;

	ERR_FAIL_COND_V_MSG(texture.is_null(), ERR_INVALID_PARAMETER, "Invalid texture passed.");
	ERR_FAIL_COND_V_MSG(texture->get_width() == 0, ERR_INVALID_DATA, "Can't save empty texture as PNG.");
	ERR_FAIL_COND_V_MSG(texture->get_height() == 0, ERR_INVALID_DATA, "Can't save empty texture as PNG.");

	Ref<ImageIndexed> img = texture->get_data();
	Error err = save_image(p_path, img);

	return err;
};

Error ResourceSaverIndexedPNG::save_image(const String &p_path, const Ref<ImageIndexed> &p_img) {
	ERR_FAIL_COND_V(p_img.is_null(), ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(p_img->get_width() == 0, ERR_INVALID_DATA);
	ERR_FAIL_COND_V(p_img->get_height() == 0, ERR_INVALID_DATA);

	Ref<Image> img = p_img->duplicate();

	if (img->is_compressed()) {
		img->decompress();
	}
	ERR_FAIL_COND_V_MSG(img->is_compressed(), ERR_UNAVAILABLE, "Cannot save indexed PNG from compressed image.");

	const bool has_palette = p_img->has_palette();
	const bool has_alpha = p_img->detect_alpha() > Image::ALPHA_NONE;
	const int format = p_img->get_format();

	// Initialize.
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	ERR_FAIL_COND_V(!png_ptr, ERR_CANT_CREATE);

	png_infop info_ptr = png_create_info_struct(png_ptr);
	ERR_FAIL_COND_V(!info_ptr, ERR_CANT_CREATE);

	if (setjmp(png_jmpbuf(png_ptr))) {
		ERR_FAIL_V(ERR_CANT_OPEN);
	}
	Error err;
	FileAccess *f = FileAccess::open(p_path, FileAccess::WRITE, &err);
	if (err != OK) {
		ERR_FAIL_V_MSG(err, "Cannot open file.");
	}
	png_set_write_fn(png_ptr, f, _write_png_data, nullptr);

	// Write header.
	if (setjmp(png_jmpbuf(png_ptr))) {
		f->close();
		memdelete(f);
		ERR_FAIL_V(ERR_CANT_OPEN);
	}

	int pngf = 0;
	int cs = 0;

	if (has_palette) {
		pngf = PNG_COLOR_TYPE_PALETTE;
		cs = 1;
	} else {
		switch (img->get_format()) {
			case Image::FORMAT_L8: {
				pngf = PNG_COLOR_TYPE_GRAY;
				cs = 1;
			} break;
			case Image::FORMAT_LA8: {
				pngf = PNG_COLOR_TYPE_GRAY_ALPHA;
				cs = 2;
			} break;
			case Image::FORMAT_RGB8: {
				pngf = PNG_COLOR_TYPE_RGB;
				cs = 3;
			} break;
			case Image::FORMAT_RGBA8: {
				pngf = PNG_COLOR_TYPE_RGB_ALPHA;
				cs = 4;
			} break;
			default: {
				if (has_alpha) {
					img->convert(Image::FORMAT_RGBA8);
					pngf = PNG_COLOR_TYPE_RGB_ALPHA;
					cs = 4;
				} else {
					img->convert(Image::FORMAT_RGB8);
					pngf = PNG_COLOR_TYPE_RGB;
					cs = 3;
				}
			}
		}
	}
	const int w = img->get_width();
	const int h = img->get_height();

	const int bit_depth = 8; // 1-2-4-8 bit depths for PNG_COLOR_TYPE_PALETTE

	png_set_IHDR(png_ptr, info_ptr, w, h, bit_depth, pngf,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Prepare palette
	png_colorp png_palette = nullptr;
	png_bytep png_palette_alpha = nullptr;

	if (has_palette) {
		PoolVector<uint8_t>::Read r = p_img->get_palette_data().read();

		int ps = 4;

		switch (format) {
			case Image::FORMAT_RGB8:
				ps = 3;
				break;
			case Image::FORMAT_RGBA8:
				ps = 4;
				break;
			default: {
				ERR_FAIL_V_MSG(ERR_UNAVAILABLE, "Cannot save indexed PNG image, unsupported format.");
			}
		}
		// RGB channels.
		int palette_size = p_img->get_palette_size();
		png_palette = (png_color *)png_malloc(png_ptr, palette_size * sizeof(png_color));

		for (int i = 0; i < palette_size; ++i) {
			png_color *c = &png_palette[i];

			c->red = r[i * ps + 0];
			c->green = r[i * ps + 1];
			c->blue = r[i * ps + 2];
		}
		png_set_PLTE(png_ptr, info_ptr, png_palette, palette_size);

		// Alpha channel.
		if (has_alpha) {
			ERR_FAIL_COND_V(format != Image::FORMAT_RGBA8, ERR_BUG);

			png_palette_alpha = (png_byte *)png_malloc(png_ptr, palette_size * sizeof(png_byte));
			for (int i = 0; i < palette_size; ++i) {
				png_bytep a = &png_palette_alpha[i];
				*a = r[i * ps + 3];
			}
			png_set_tRNS(png_ptr, info_ptr, png_palette_alpha, palette_size, nullptr);
		}
	}
	png_write_info(png_ptr, info_ptr);

	// Write data.
	if (setjmp(png_jmpbuf(png_ptr))) {
		f->close();
		memdelete(f);
		ERR_FAIL_V(ERR_CANT_OPEN);
	}

	PoolVector<uint8_t>::Read r;
	if (has_palette) {
		r = p_img->get_index_data().read();
	} else {
		r = img->get_data().read();
	}
	png_bytep *row_pointers = (png_bytep *)memalloc(sizeof(png_bytep) * h);
	for (int i = 0; i < h; ++i) {
		row_pointers[i] = (png_bytep)&r[i * w * cs];
	}
	png_write_image(png_ptr, row_pointers);
	memfree(row_pointers);

	if (setjmp(png_jmpbuf(png_ptr))) {
		f->close();
		memdelete(f);
		ERR_FAIL_V(ERR_CANT_OPEN);
	}
	png_write_end(png_ptr, nullptr);

	// Cleanup.
	png_free(png_ptr, png_palette);
	png_free(png_ptr, png_palette_alpha);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	f->close();
	memdelete(f);

	return OK;
}

bool ResourceSaverIndexedPNG::recognize(const RES &p_resource) const {
	return (p_resource.is_valid() && p_resource->is_class("ImageTexture"));
}
void ResourceSaverIndexedPNG::get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const {
	if (Object::cast_to<Texture>(*p_resource)) {
		p_extensions->push_back("png");
	}
}

ResourceSaverIndexedPNG::ResourceSaverIndexedPNG() {
	ImageIndexed::save_indexed_png_func = &save_image;
};
