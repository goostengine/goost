#include "image_loader_indexed_png.h"

#include "core/os/os.h"
#include "core/print_string.h"

#include <string.h>

void ImageLoaderIndexedPNG::_read_png_data(png_structp png_ptr, png_bytep data, png_size_t p_length) {
	FileAccess *f = (FileAccess *)png_get_io_ptr(png_ptr);
	f->get_buffer((uint8_t *)data, p_length);
}

static png_voidp _png_malloc_fn(png_structp png_ptr, png_size_t size) {
	return memalloc(size);
}

static void _png_free_fn(png_structp png_ptr, png_voidp ptr) {
	memfree(ptr);
}

static void _png_error_function(png_structp, png_const_charp text) {
	ERR_PRINT(text);
}

static void _png_warn_function(png_structp, png_const_charp text) {
#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint()) {
		if (String(text).begins_with("iCCP"))
			return;
	}
#endif
	WARN_PRINT(text);
}

typedef void(PNGAPI *png_error_ptr) PNGARG((png_structp, png_const_charp));

Error ImageLoaderIndexedPNG::_load_image(void *rf_up, png_rw_ptr p_func, Ref<ImageIndexed> p_image) {
	png_structp png;
	png_infop info;

	png = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, _png_error_function, _png_warn_function, (png_voidp)NULL,
			_png_malloc_fn, _png_free_fn);

	ERR_FAIL_COND_V(!png, ERR_OUT_OF_MEMORY);

	info = png_create_info_struct(png);
	if (!info) {
		png_destroy_read_struct(&png, NULL, NULL);
		ERR_PRINT("Out of Memory");
		return ERR_OUT_OF_MEMORY;
	}

	if (setjmp(png_jmpbuf(png))) {
		png_destroy_read_struct(&png, NULL, NULL);
		ERR_PRINT("PNG Corrupted");
		return ERR_FILE_CORRUPT;
	}

	png_set_read_fn(png, (void *)rf_up, p_func);

	png_uint_32 width, height;
	int depth, color;

	png_read_info(png, info);
	png_get_IHDR(png, info, &width, &height, &depth, &color, NULL, NULL, NULL);

	bool update_info = false;

	png_colorp png_palette = NULL;
	int palette_size = 0;

	if (png_get_valid(png, info, PNG_INFO_PLTE)) {
		png_get_PLTE(png, info, &png_palette, &palette_size);
		update_info = true;
	}

	if (depth < 8) { //only bit dept 8 per channel is handled
		png_set_packing(png);
		update_info = true;
	}

	if (depth > 8) {
		png_set_strip_16(png);
		update_info = true;
	}

	png_bytep png_palette_alpha = NULL;
	int palette_alpha_size = 0;

	if (png_get_valid(png, info, PNG_INFO_tRNS)) {
		png_get_tRNS(png, info, &png_palette_alpha, &palette_alpha_size, NULL);
		update_info = true;
	}

	if (update_info) {
		png_read_update_info(png, info);
		png_get_IHDR(png, info, &width, &height, &depth, &color, NULL, NULL, NULL);
	}

	int components = 0;

	Image::Format fmt;
	switch (color) {
		case PNG_COLOR_TYPE_PALETTE: {
			fmt = Image::FORMAT_L8;
			components = 1;
		} break;
		case PNG_COLOR_TYPE_GRAY: {
			fmt = Image::FORMAT_L8;
			components = 1;
		} break;
		case PNG_COLOR_TYPE_GRAY_ALPHA: {
			fmt = Image::FORMAT_LA8;
			components = 2;
		} break;
		case PNG_COLOR_TYPE_RGB: {
			fmt = Image::FORMAT_RGB8;
			components = 3;
		} break;
		case PNG_COLOR_TYPE_RGB_ALPHA: {
			fmt = Image::FORMAT_RGBA8;
			components = 4;
		} break;
		default: {
			ERR_PRINT("INVALID PNG TYPE");
			png_destroy_read_struct(&png, &info, NULL);
			return ERR_UNAVAILABLE;
		} break;
	}

	//int rowsize = png_get_rowbytes(png, info);
	int rowsize = components * width;

	PoolVector<uint8_t> dstbuff;

	dstbuff.resize(rowsize * height);

	PoolVector<uint8_t>::Write dstbuff_write = dstbuff.write();

	uint8_t *data = dstbuff_write.ptr();

	uint8_t **row_p = memnew_arr(uint8_t *, height);

	for (unsigned int i = 0; i < height; i++) {
		row_p[i] = &data[components * width * i];
	}

	png_read_image(png, (png_bytep *)row_p);

	memdelete_arr(row_p);

	if (color == PNG_COLOR_TYPE_PALETTE) {
		// Loaded data are indices
		ERR_FAIL_COND_V_MSG(png_palette == NULL, ERR_BUG, "Expected to extract PNG palette, got none.");

		PoolVector<uint8_t> palette_data;
		palette_data.resize(palette_size * 4);
		PoolVector<uint8_t>::Write w = palette_data.write();

		if (png_palette_alpha && palette_alpha_size == palette_size) {
			for (int i = 0; i < palette_size; i++) {
				png_colorp c = &png_palette[i];
				w[i * 4 + 0] = c->red;
				w[i * 4 + 1] = c->green;
				w[i * 4 + 2] = c->blue;

				png_bytep a = &png_palette_alpha[i];
				w[i * 4 + 3] = *a;
			}
		} else {
			for (int i = 0; i < palette_size; i++) {
				png_colorp c = &png_palette[i];
				w[i * 4 + 0] = c->red;
				w[i * 4 + 1] = c->green;
				w[i * 4 + 2] = c->blue;
				w[i * 4 + 3] = 255;
			}
		}
		// Create image with palette and extend it
		p_image->create(width, height, 0, Image::FORMAT_RGBA8);
		p_image->create_indexed_from_data(palette_data, dstbuff);
		p_image->apply_palette(); // (png_palette_to_rgb)

	} else {
		ERR_FAIL_COND_V_MSG(png_palette, ERR_BUG, "Attempt to create a regular image with a palette.");
		// Loaded data are pixels
		p_image->create(width, height, 0, fmt, dstbuff);
	}

	png_destroy_read_struct(&png, &info, NULL);

	return OK;
}

Error ImageLoaderIndexedPNG::load_image(Ref<Image> p_image, FileAccess *f, bool p_force_linear, float p_scale) {
	Ref<ImageIndexed> img_i = p_image;
	ERR_FAIL_COND_V(img_i.is_null(), ERR_BUG);

	Error err = _load_image(f, _read_png_data, img_i);
	f->close();

	return err;
}

void ImageLoaderIndexedPNG::get_recognized_extensions(List<String> *p_extensions) const {
	// There's no way to know whether a PNG image is indexed only by extension, sorry.
}

struct PNGReadStatus {
	uint32_t offset;
	uint32_t size;
	const unsigned char *image;
};

static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t p_length) {
	PNGReadStatus *rstatus;
	rstatus = (PNGReadStatus *)png_get_io_ptr(png_ptr);

	png_size_t to_read = MIN(p_length, rstatus->size - rstatus->offset);
	memcpy(data, &rstatus->image[rstatus->offset], to_read);
	rstatus->offset += to_read;

	if (to_read < p_length) {
		memset(&data[to_read], 0, p_length - to_read);
	}
}

static Ref<ImageIndexed> _load_mem_indexed_png(const uint8_t *p_png, int p_size) {
	PNGReadStatus prs;
	prs.image = p_png;
	prs.offset = 0;
	prs.size = p_size;

	Ref<ImageIndexed> img;
	img.instance();
	Error err = ImageLoaderIndexedPNG::_load_image(&prs, user_read_data, img);
	ERR_FAIL_COND_V(err, Ref<ImageIndexed>());

	return img;
}

ImageLoaderIndexedPNG::ImageLoaderIndexedPNG() {
	ImageIndexed::_indexed_png_mem_loader_func = _load_mem_indexed_png;
}
