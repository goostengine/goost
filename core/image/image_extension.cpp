#include "image_extension.h"

#include "goost/thirdparty/hqx/HQ2x.hh"
#include "goost/thirdparty/hqx/HQ3x.hh"
#include "goost/thirdparty/leptonica/allheaders.h"

void ImageExtension::replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color) {

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

Ref<Image> ImageExtension::bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image, Connectivity p_con) {

	// Based on flood-fill algorithm
	// Runs up to x35 faster compared to GDScript implementation

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

	List<Vector2> to_fill;
	to_fill.push_back(pos);

	while (!to_fill.empty()) {
		pos = to_fill.front()->get();
		to_fill.pop_front();

		for (int i = 0; i < kernel.size(); ++i) {

			const Vector2 &dir = kernel[i];
			at = pos + dir;

			if (has_pixelv(fill_image, at)) {
				pixel = fill_image->get_pixelv(at);
				if (pixel.a > 0.0)
					continue; // already filled
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
		// Fill the actual image (no undo)
		// else just return filled area as a new image
		Rect2 fill_rect(0, 0, width, height);
		p_image->blend_rect(fill_image, fill_rect, Point2());
	}

	fill_image->unlock();
	p_image->unlock();

	return fill_image;
}

void ImageExtension::resize_hqx(Ref<Image> p_image, int p_scale) {
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

PIX *pix_create_from_image(Ref<Image> p_image, Image::Format p_convert = Image::FORMAT_RGBA8);
Ref<Image> image_create_from_pix(PIX *p_pix);
void image_copy_from_pix(Ref<Image> p_image, PIX *p_pix);

void ImageExtension::rotate(Ref<Image> p_image, real_t p_angle) {
	PIX *pix_in = pix_create_from_image(p_image);
	PIX *pix_out = pixRotate(
			pix_in, p_angle, L_ROTATE_SHEAR, L_BRING_IN_BLACK, 
			p_image->get_width(), p_image->get_height()
	);
	image_copy_from_pix(p_image, pix_out);
	pixDestroy(&pix_out);
}

void ImageExtension::rotate_90(Ref<Image> p_image, Direction p_direction) {
	PIX *pix_in = pix_create_from_image(p_image);
	PIX *pix_out = pixRotate90(pix_in, static_cast<int>(p_direction));
	image_copy_from_pix(p_image, pix_out);
	pixDestroy(&pix_out);
}

void ImageExtension::rotate_180(Ref<Image> p_image) {
	PIX *pix_in = pix_create_from_image(p_image);
	PIX *pix_out = pixRotate180(nullptr, pix_in);
	image_copy_from_pix(p_image, pix_out);
	pixDestroy(&pix_out);
}

Ref<Image> ImageExtension::render_polygon(Vector<Point2> p_polygon, bool p_fill, const Color &p_color, const Color &p_bg_color) {
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

	PIX *pix_out = pixConvert1To32(
			nullptr, p_fill ? pix_fill : pix_poly, 
			p_bg_color.to_abgr32(), p_color.to_abgr32()
	);
	pixDestroy(&pix_poly);
	if (pix_fill) {
		pixDestroy(&pix_fill);
	}
	Ref<Image> image = image_create_from_pix(pix_out);
	pixDestroy(&pix_out);

	return image;
}

bool ImageExtension::has_pixel(Ref<Image> p_image, int x, int y) {
	return get_pixel_or_null(p_image, x, y);
}

bool ImageExtension::has_pixelv(Ref<Image> p_image, const Vector2 &p_pos) {
	return get_pixelv_or_null(p_image, p_pos);
}

bool ImageExtension::get_pixel_or_null(Ref<Image> p_image, int x, int y, Color* r_pixel) {
	if (x >= 0 && x < p_image->get_width() && y >= 0 && y < p_image->get_height()) {
		if (r_pixel) {
			*r_pixel = p_image->get_pixel(x, y);
		}
		return true;
	}
	return false;
}

bool ImageExtension::get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos, Color* r_pixel) {
	return get_pixel_or_null(p_image, p_pos.x, p_pos.y, r_pixel);
}

// PIX to Image conversion

PIX *pix_create_from_image(Ref<Image> p_image, Image::Format p_format) {
	p_image->convert(p_format); // Does nothing if the same format.
	PoolVector<uint8_t> src = p_image->get_data();
	PoolVector<uint8_t>::Read read = src.read();
	ERR_FAIL_COND_V(!read.ptr(), nullptr);

	const uint8_t bpp = Image::get_format_pixel_size(p_image->get_format()) * 8;
	PIX *pix_in = pixCreateNoInit(p_image->get_width(), p_image->get_height(), bpp);
	pixSetData(pix_in, (l_uint32 *)read.ptr());

	return pix_in;
}

void _image_from_pix(Ref<Image> p_image, PIX *p_pix) {
	ERR_FAIL_COND_MSG(!p_pix, "Invalid image input data.");

	l_uint32 * src_data = pixExtractData(p_pix);
	ERR_FAIL_COND_MSG(!src_data, "Could not extract image data.");

	const int width = p_pix->w;
	const int height = p_pix->h;
	
	Image::Format format = Image::FORMAT_RGBA8;
	switch (p_pix->d) {
		case 32: {
			format = Image::FORMAT_RGBA8;
		} break;
		case 24: {
			format = Image::FORMAT_RGB8;
		} break;
		case 8: {
			format = Image::FORMAT_L8;
		} break;
	}
	PoolVector<uint8_t> dest;
	{	
		const int data_size = Image::get_image_data_size(width, height, format);
		dest.resize(data_size);
		PoolVector<uint8_t>::Write w = dest.write();
		copymem((uint32_t *)w.ptr(), (uint32_t *)src_data, data_size);
	}
	p_image->create(width, height, false, format, dest);
}

Ref<Image> image_create_from_pix(PIX *p_pix) {
	Ref<Image> image;
	image.instance();
	_image_from_pix(image, p_pix);
	return image;
}

void image_copy_from_pix(Ref<Image> p_image, PIX *p_pix) {
	_image_from_pix(p_image, p_pix);
}
