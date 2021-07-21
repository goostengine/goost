#include "goost_image.h"

#include "modules/modules_enabled.gen.h"
#ifdef MODULE_SVG_ENABLED
#include "modules/svg/image_loader_svg.h"
#endif

#include "goost/thirdparty/hqx/HQ2x.hh"
#include "goost/thirdparty/hqx/HQ3x.hh"
#include "goost/thirdparty/leptonica/allheaders.h"

#include "core/local_vector.h"

void GoostImage::replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color) {
	if (p_color == p_with_color) {
		return;
	}

	p_image->lock();

	for (int y = 0; y < p_image->get_height(); ++y) {
		for (int x = 0; x < p_image->get_width(); ++x) {
			if (p_image->get_pixel(x, y) == p_color) {
				p_image->set_pixel(x, y, p_with_color);
			}
		}
	}
	p_image->unlock();
}

struct BucketFillQueue {
	LocalVector<Vector2> array;
	uint32_t front = 0;
	uint32_t back = 0;

	_FORCE_INLINE_ void push_back(const Vector2 &p_point) {
		array.push_back(p_point);
		++back;
	}
	_FORCE_INLINE_ Vector2 pop_front() {
		return array[front++];
	}
	_FORCE_INLINE_ bool is_empty() {
		return front == back;
	}
};

// Based on flood-fill algorithm.
// Scanline could perform better for 4-connected case, but this runs up to
// x35 faster compared to equivalent GDScript implementation in any case.
Ref<Image> GoostImage::bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image, Connectivity p_con) {
	if (!has_pixelv(p_image, p_at)) {
		return Ref<Image>();
	}
	p_image->lock();

	Color filling_color = p_image->get_pixelv(p_at);

	int width = p_image->get_width();
	int height = p_image->get_height();
	bool mipmaps = p_image->has_mipmaps();
	Image::Format format = p_image->get_format();

	Ref<Image> fill_image = memnew(Image);
	fill_image->create(width, height, mipmaps, format);
	fill_image->lock();

	Vector2 at;
	Vector2 pos = p_at;
	Color pixel;

	Vector<Vector2> kernel;
	switch (p_con) {
		case FOUR_CONNECTED: {
			kernel.push_back(Vector2(1, 0));
			kernel.push_back(Vector2(0, -1));
			kernel.push_back(Vector2(-1, 0));
			kernel.push_back(Vector2(0, 1));
		} break;

		case EIGHT_CONNECTED: {
			kernel.push_back(Vector2(1, 0));
			kernel.push_back(Vector2(1, -1));
			kernel.push_back(Vector2(0, -1));
			kernel.push_back(Vector2(-1, -1));
			kernel.push_back(Vector2(-1, 0));
			kernel.push_back(Vector2(-1, 1));
			kernel.push_back(Vector2(0, 1));
			kernel.push_back(Vector2(1, 1));
		} break;
	}

	BucketFillQueue to_fill;
	// Reserve some memory for typical images such as textures, will grow as needed.
	to_fill.array.reserve(MIN(512 * 512, width * height));
	to_fill.push_back(pos);

	while (!to_fill.is_empty()) {
		pos = to_fill.pop_front();

		for (int i = 0; i < kernel.size(); ++i) {
			const Vector2 &dir = kernel[i];
			at = pos + dir;

			if (has_pixelv(fill_image, at)) {
				pixel = fill_image->get_pixelv(at);
				if (pixel.a > 0.0) {
					continue; // Already filled.
				}
			}
			if (has_pixelv(fill_image, at)) {
				pixel = p_image->get_pixelv(at);
			} else {
				continue;
			}
			if (pixel == filling_color) {
				fill_image->set_pixelv(at, p_fill_color);
				to_fill.push_back(at);
			}
		}
	}
	if (p_fill_image) {
		// Fill the actual image (no undo),
		// else just return filled area as a new image.
		Rect2 fill_rect(0, 0, width, height);
		p_image->blend_rect(fill_image, fill_rect, Point2());
	}

	fill_image->unlock();
	p_image->unlock();

	return fill_image;
}

void GoostImage::resize_hqx(Ref<Image> p_image, int p_scale) {
	ERR_FAIL_COND(p_scale < 2);
	ERR_FAIL_COND(p_scale > 3);

	bool used_mipmaps = p_image->has_mipmaps();

	Image::Format current = p_image->get_format();
	if (current != Image::FORMAT_RGBA8) {
		p_image->convert(Image::FORMAT_RGBA8);
	}
	PoolVector<uint8_t> dest;
	PoolVector<uint8_t> src = p_image->get_data();

	const int new_width = p_image->get_width() * p_scale;
	const int new_height = p_image->get_height() * p_scale;
	dest.resize(new_width * new_height * 4);
	{
		PoolVector<uint8_t>::Read r = src.read();
		PoolVector<uint8_t>::Write w = dest.write();

		ERR_FAIL_COND(!r.ptr());

		HQx *hqx;
		if (p_scale == 2) {
			hqx = memnew(HQ2x);
		} else if (p_scale == 3) {
			hqx = memnew(HQ3x);
		} else {
			hqx = memnew(HQ2x); // Fallback to HQ2x in all cases.
		}
		hqx->resize((const uint32_t *)r.ptr(), p_image->get_width(), p_image->get_height(), (uint32_t *)w.ptr());
		memdelete(hqx);
	}
	p_image->create(new_width, new_height, false, Image::FORMAT_RGBA8, dest);

	if (used_mipmaps) {
		p_image->generate_mipmaps();
	}
}

// PIX <-> Godot Image conversion.
PIX *pix_create_from_image(Ref<Image> p_image);
Ref<Image> image_create_from_pix(PIX *p_pix, bool p_include_alpha = true);
void image_copy_from_pix(Ref<Image> p_image, PIX *p_pix, bool p_include_alpha = true);

void GoostImage::rotate(Ref<Image> p_image, real_t p_angle, bool p_expand) {
	PIX *pix_in = pix_create_from_image(p_image);

	const int w = p_expand ? p_image->get_width() : 0;
	const int h = p_expand ? p_image->get_height() : 0;
	const int type = L_ROTATE_SHEAR;
	const bool hq = type == L_ROTATE_AREA_MAP;

	PIX *pix_out = pixRotate(pix_in, p_angle, type, L_BRING_IN_BLACK, w, h);
	pixDestroy(&pix_in);

	image_copy_from_pix(p_image, pix_out, hq ? false : true);
	pixDestroy(&pix_out);
}

void GoostImage::rotate_90(Ref<Image> p_image, Direction p_direction) {
	PIX *pix_in = pix_create_from_image(p_image);

	PIX *pix_out = pixRotate90(pix_in, static_cast<int>(p_direction));
	pixDestroy(&pix_in);

	image_copy_from_pix(p_image, pix_out);
	pixDestroy(&pix_out);
}

void GoostImage::rotate_180(Ref<Image> p_image) {
	PIX *pix_in = pix_create_from_image(p_image);

	PIX *pix_out = pixRotate180(nullptr, pix_in);
	pixDestroy(&pix_in);

	image_copy_from_pix(p_image, pix_out);
	pixDestroy(&pix_out);
}

void GoostImage::binarize(Ref<Image> p_image, real_t p_threshold, bool p_invert) {
	PIX *pix_in = pix_create_from_image(p_image);
	
	PIX *pix_bin = nullptr;
	if (p_threshold < 0) {
		pix_bin = pixConvertTo1Adaptive(pix_in);
	} else {
		pix_bin = pixConvertTo1(pix_in, uint8_t(CLAMP(p_threshold * 255.0, 0, 255)));
	}
	pixDestroy(&pix_in);

	const l_uint32 val0 = p_invert ? 0 : 0xffffffff;
	const l_uint32 val1 = p_invert ? 0xffffffff : 0;

	PIX *pix_grayscale = pixConvert1To8(nullptr, pix_bin, val0, val1);
	pixDestroy(&pix_bin);

	image_copy_from_pix(p_image, pix_grayscale);
	pixDestroy(&pix_grayscale);
}

void GoostImage::dilate(Ref<Image> p_image, int p_kernel_size) {
	morph(p_image, MORPH_DILATE, Size2i(p_kernel_size, p_kernel_size));
}

void GoostImage::erode(Ref<Image> p_image, int p_kernel_size) {
	morph(p_image, MORPH_ERODE, Size2i(p_kernel_size, p_kernel_size));
}

void GoostImage::morph(Ref<Image> p_image, MorphOperation p_op, Size2i p_kernel_size) {
	const int hs = p_kernel_size.x;
	const int vs = p_kernel_size.y;
#ifdef DEBUG_ENABLED
	ERR_FAIL_COND_MSG(hs % 2 == 0, "Kernel X size must be an odd number.");
	ERR_FAIL_COND_MSG(vs % 2 == 0, "Kernel Y size must be an odd number.");
	ERR_FAIL_COND_MSG(hs <= 1 || vs <= 1, "Kernel size must be greater than 1.");
#endif
	p_image->convert(Image::FORMAT_RGBA8);

	PIX *pix_in = pix_create_from_image(p_image);
	l_int32 type = -1;
	switch (p_op) {
		case MORPH_DILATE: {
			type = L_MORPH_DILATE;
		} break;
		case MORPH_ERODE: {
			type = L_MORPH_ERODE;
		} break;
		case MORPH_OPEN: {
			type = L_MORPH_OPEN;
		} break;
		case MORPH_CLOSE: {
			type = L_MORPH_CLOSE;
		} break;
		default: {
			ERR_FAIL_MSG("Invalid morph type");
		}
	}
	PIX *pix_morph = pixColorMorph(pix_in, type, hs, vs);
	pixDestroy(&pix_in);

	image_copy_from_pix(p_image, pix_morph, false);
	pixDestroy(&pix_morph);
}

Ref<Image> GoostImage::tile(const Ref<Image> &p_image, const Size2i &p_size, WrapMode p_mode) {
	ERR_FAIL_COND_V(p_image.is_null(), Variant());
	ERR_FAIL_COND_V(p_image->empty(), Variant());

	const int sw = p_image->get_width();
	const int sh = p_image->get_height();
	const int dw = p_size.x;
	const int dh = p_size.y;

	ERR_FAIL_COND_V(dw <= 0, Variant());
	ERR_FAIL_COND_V(dh <= 0, Variant());

	const int cols = (dw + sw - 1) / sw;
	const int rows = (dh + sh - 1) / sh;

	return repeat(p_image, Size2i(cols, rows), p_mode, p_size);
}

Ref<Image> GoostImage::repeat(const Ref<Image> &p_image, const Size2i &p_count, WrapMode p_mode, const Size2i &p_max_size) {
	ERR_FAIL_COND_V(p_image.is_null(), Variant());
	ERR_FAIL_COND_V(p_image->empty(), Variant());

	const int cols = p_count.x;
	const int rows = p_count.y;

	ERR_FAIL_COND_V(cols <= 0, Variant());
	ERR_FAIL_COND_V(rows <= 0, Variant());

	Ref<Image> src = p_image;
	Ref<Image> src_fx;
	Ref<Image> src_fy;
	Ref<Image> src_fxy;

	Rect2i src_rect = Rect2i(Size2i(0, 0), src->get_size());
	const int w = src->get_width();
	const int h = src->get_height();

	if (p_mode == TILE_FLIP_XY || p_mode == TILE_FLIP_X) {
		src_fx.instance();
		src_fx->copy_internals_from(src);
		src_fx->flip_x();
	}
	if (p_mode == TILE_FLIP_XY || p_mode == TILE_FLIP_Y) {
		src_fy.instance();
		src_fy->copy_internals_from(src);
		src_fy->flip_y();
	}
	if (p_mode == TILE_FLIP_XY) {
		src_fxy.instance();
		src_fxy->copy_internals_from(src_fx);
		src_fxy->flip_y();
	}
	Ref<Image> dest;
	dest.instance();

	int dw = CLAMP(w * cols, 0, p_max_size.x);
	int dh = CLAMP(h * rows, 0, p_max_size.y);

	if (cols == 1 && p_max_size.x < src->get_size().x) {
		dw = p_max_size.x;
	}
	if (rows == 1 && p_max_size.y < src->get_size().y) {
		dh = p_max_size.y;
	}
	dest->create(dw, dh, false, src->get_format());

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			src = p_image;
			switch (p_mode) {
				case TILE_FLIP_Y: {
					if (i & 1) {
						src = src_fy;
					}
				} break;
				case TILE_FLIP_X: {
					if (j & 1) {
						src = src_fx;
					}
				} break;
				case TILE_FLIP_XY: {
					if ((i & 1) && !(j & 1)) {
						src = src_fy;
					} else if (!(i & 1) && (j & 1)) {
						src = src_fx;
					} else if ((i & 1) && (j & 1)) {
						src = src_fxy;
					}
				} break;
				default: {
				}
			}
			dest->blit_rect(src, src_rect, Size2i(j * w, i * h));
		}
	}
	return dest;
}

Point2 GoostImage::get_centroid(const Ref<Image> &p_image) {
	PIX *pix_in = pix_create_from_image(p_image);

	PIX *pix_bin = pixConvertTo8(pix_in, 0);
	pixDestroy(&pix_in);

	l_float32 x, y;
	pixCentroid(pix_bin, nullptr, nullptr, &x, &y);
	pixDestroy(&pix_bin);

	return Point2(static_cast<real_t>(x), static_cast<real_t>(y));
}

Color GoostImage::get_pixel_average(const Ref<Image> &p_image, const Rect2 &p_rect, const Ref<Image> &p_mask) {
	bool using_rect = !p_rect.has_no_area();
	bool using_mask = p_mask.is_valid();
	if (using_mask) {
		ERR_FAIL_COND_V(p_mask->empty(), Color());
	}
#ifdef DEBUG_ENABLED
	if (using_mask) {
		ERR_FAIL_COND_V(p_mask->is_invisible(), Color());
	}
#endif
	PIX *pix = pix_create_from_image(p_image);
	PIX *pix_mask = nullptr;
	if (using_mask) {
		PIX *pix_tmp = pix_create_from_image(p_mask);
		pix_mask = pixConvertTo1(pix_tmp, 0);
		pixDestroy(&pix_tmp);
	}
	BOX *box = nullptr;
	if (using_rect) {
		box = memnew(Box);
		box->x = p_rect.position.x;
		box->y = p_rect.position.y;
		box->w = p_rect.size.x;
		box->h = p_rect.size.y;
	}
	l_uint32 pixel_rgb = 0;
	Color average;
	l_ok ret = 0;

	if (pix->d == 8) {
		l_float32 c = 0;
		l_int32 max = 255;
		ret = pixAverageInRect(pix, pix_mask, box, 0, max, 1, &c);
		average = Color(c, c, c) / max;
		average.a = 1.0f;
	} else if (pix->d == 32) {
		ret = pixAverageInRectRGB(pix, pix_mask, box, 1, &pixel_rgb);
		average = Color::hex(pixel_rgb);
		average.a = 1.0f;
	}
	if (pix_mask) {
		pixDestroy(&pix_mask);
	}
	if (box) {
		memdelete(box);
	}
	pixDestroy(&pix);

	ERR_FAIL_COND_V_MSG(ret == 1, Color(), "Invalid input data.");
	// If this happens, it's an internal bug (should be handled above).
	ERR_FAIL_COND_V_MSG(ret == 2, Color(), "All pixels are filtered out.");

	return average;
}

Ref<Image> GoostImage::render_polygon(Vector<Point2> p_polygon, bool p_fill, const Color &p_color, const Color &p_bg_color) {
	ERR_FAIL_COND_V_MSG(p_polygon.size() < 3, Variant(), "Bad polygon!")

	PTA *pta_in = ptaCreate(p_polygon.size());
	for (int i = 0; i < p_polygon.size(); ++i) {
		ptaAddPt(pta_in, p_polygon[i].x, p_polygon[i].y);
	}
	// Outline width is not exposed, width > 1 produces squarish results.
	static constexpr int width = 1;
	int xmin, ymin;

	PIX *pix_poly = pixRenderPolygon(pta_in, width, &xmin, &ymin);
	PIX *pix_fill = nullptr;
	if (p_fill) {
		pix_fill = pixFillPolygon(pix_poly, pta_in, xmin, ymin);
	}
	ptaDestroy(&pta_in);

#ifdef L_LITTLE_ENDIAN
	const uint32_t bg_color = p_bg_color.to_rgba32();
	const uint32_t color = p_color.to_rgba32();
#else
	const uint32_t bg_color = p_bg_color.to_abgr32();
	const uint32_t color = p_color.to_abgr32();
#endif
	PIX *pix_out = pixConvert1To32(nullptr, p_fill ? pix_fill : pix_poly, bg_color, color);
	pixDestroy(&pix_poly);
	if (pix_fill) {
		pixDestroy(&pix_fill);
	}
	Ref<Image> image = image_create_from_pix(pix_out);
	pixDestroy(&pix_out);

	return image;
}

Ref<Image> GoostImage::render_svg(const String &p_svg, real_t p_scale) {
	Ref<Image> image;
#ifdef MODULE_SVG_ENABLED
	ERR_FAIL_COND_V_MSG(p_svg.empty(), Ref<Image>(), "Empty SVG document.");
	ERR_FAIL_COND_V_MSG(p_scale <= 0, Ref<Image>(), "Scale must be positive.");
	image.instance();
	CharString svg = p_svg.utf8();
	const char *svg_str = svg.get_data();
	ImageLoaderSVG::create_image_from_string(image, svg_str, p_scale, false, false);
#else
	WARN_PRINT_ONCE("Cannot render, the SVG module is disabled.");
#endif
	return image;
}

bool GoostImage::has_pixel(Ref<Image> p_image, int x, int y) {
	return get_pixel_or_null(p_image, x, y);
}

bool GoostImage::has_pixelv(Ref<Image> p_image, const Vector2 &p_pos) {
	return get_pixelv_or_null(p_image, p_pos);
}

bool GoostImage::get_pixel_or_null(Ref<Image> p_image, int x, int y, Color *r_pixel) {
	if (x >= 0 && x < p_image->get_width() && y >= 0 && y < p_image->get_height()) {
		if (r_pixel) {
			*r_pixel = p_image->get_pixel(x, y);
		}
		return true;
	}
	return false;
}

bool GoostImage::get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos, Color *r_pixel) {
	return get_pixel_or_null(p_image, p_pos.x, p_pos.y, r_pixel);
}

// PIX to Image conversion

PIX *pix_create_from_image(Ref<Image> p_image) {
	if (p_image->get_format() == Image::FORMAT_RGB8) {
		// 24 bpp is not a valid image type in Leptonica.
		p_image->convert(Image::FORMAT_RGBA8);
	}
	const Image::Format format = p_image->get_format();

	PoolVector<uint8_t> src = p_image->get_data();
	PoolVector<uint8_t>::Read read = src.read();
	ERR_FAIL_COND_V(!read.ptr(), nullptr);
	const uint8_t *r = read.ptr();

	const int w = p_image->get_width();
	const int h = p_image->get_height();

	const int ds = Image::get_image_data_size(w, h, format);
	const int ps = Image::get_format_pixel_size(format);
	const uint8_t d = ps * 8;

	PIX *pix = pixCreateNoInit(w, h, d);
	l_uint32 *pix_data = pixGetData(pix);
	l_int32 wpl = pixGetWpl(pix);
	l_uint32 *line = nullptr;
	size_t index = 0;

	switch (d) {
		case 8: {
			for (int i = 0; i < h; i++) {
				line = pix_data + i * wpl;
				for (int j = 0; j < w; j++) {
					SET_DATA_BYTE(line, j, r[index++]);
				}
			}
		} break;
		case 32: {
			for (int i = 0; i < ds; ++i) {
				SET_DATA_BYTE(pix_data, i, r[i]);
			}
		} break;
		default: {
			ERR_FAIL_V_MSG(nullptr, "Unsupported image format.");
		}
	}
	return pix;
}

void _image_from_pix(Ref<Image> p_image, PIX *p_pix, bool p_include_alpha) {
	ERR_FAIL_COND_MSG(!p_pix, "Invalid image input data.");

	const int width = pixGetWidth(p_pix);
	const int height = pixGetHeight(p_pix);

	PIX *pix = p_pix;

	Image::Format format;
	switch (pix->d) {
		case 8: {
			format = Image::FORMAT_L8;
		} break;
		case 32: {
			format = Image::FORMAT_RGBA8;
		} break;
		default: {
			ERR_FAIL_MSG(vformat("Image depth %s not supported.", pix->d));
		}
	}
	l_uint32 *pix_data = pixGetData(pix);
	ERR_FAIL_COND_MSG(!pix_data, "Could not fetch image data.");
	PoolVector<uint8_t> image_data;
	{
		const int data_size = Image::get_image_data_size(width, height, format);
		image_data.resize(data_size);
		PoolVector<uint8_t>::Write write = image_data.write();
		uint8_t *w = write.ptr();

		const int pixel_count = width * height;
		const int pixel_size = Image::get_format_pixel_size(format);

		size_t index = 0;
		l_uint32 val = 0;
		l_int32 wpl = pixGetWpl(pix);
		l_uint32 *line = nullptr;

		switch (pix->d) {
			case 8: {
				for (int i = 0; i < height; i++) {
					line = pix_data + i * wpl;
					for (int j = 0; j < width; j++) {
						w[index++] = GET_DATA_BYTE(line, j);
					}
				}
			} break;
			case 32: {
				if (p_include_alpha) {
					for (int i = 0; i < pixel_count; ++i) {
						val = pix_data[i];
						w[index + 0] = GET_DATA_BYTE(&val, COLOR_RED);
						w[index + 1] = GET_DATA_BYTE(&val, COLOR_GREEN);
						w[index + 2] = GET_DATA_BYTE(&val, COLOR_BLUE);
						w[index + 3] = GET_DATA_BYTE(&val, L_ALPHA_CHANNEL);
						index += pixel_size;
					}
				} else {
					for (int i = 0; i < pixel_count; ++i) {
						val = pix_data[i];
						w[index + 0] = GET_DATA_BYTE(&val, COLOR_RED);
						w[index + 1] = GET_DATA_BYTE(&val, COLOR_GREEN);
						w[index + 2] = GET_DATA_BYTE(&val, COLOR_BLUE);
						w[index + 3] = 0xff;
						index += pixel_size;
					}
				}
			} break;
		}
	}
	p_image->create(width, height, false, format, image_data);
}

Ref<Image> image_create_from_pix(PIX *p_pix, bool p_include_alpha) {
	Ref<Image> image;
	image.instance();
	_image_from_pix(image, p_pix, p_include_alpha);
	return image;
}

void image_copy_from_pix(Ref<Image> p_image, PIX *p_pix, bool p_include_alpha) {
	_image_from_pix(p_image, p_pix, p_include_alpha);
}
