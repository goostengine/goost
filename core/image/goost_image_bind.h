#ifndef GOOST_IMAGE_BIND_H
#define GOOST_IMAGE_BIND_H

#include "core/image.h"

class _GoostImage : public Object {
	GDCLASS(_GoostImage, Object);

private:
	static _GoostImage *singleton;

protected:
	static void _bind_methods();

public:
	static _GoostImage *get_singleton() { return singleton; }

public:
	enum Connectivity {
		FOUR_CONNECTED,
		EIGHT_CONNECTED,
	};
	enum Direction {
		CW = 1,
		CCW = -1,
	};
public:
	// Image methods
	void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);
	Ref<Image> bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image = true, Connectivity p_con = FOUR_CONNECTED);
	void resize_hqx(Ref<Image> p_image, int p_scale);
	void rotate(Ref<Image> p_image, real_t p_angle);
	void rotate_90(Ref<Image> p_image, Direction p_direction);
	void rotate_180(Ref<Image> p_image);
	Ref<Image> render_polygon(Vector<Point2> p_polygon, bool p_fill, const Color &p_color, const Color &p_bg_color);

	// Pixel methods
	bool has_pixel(Ref<Image> p_image, int x, int y);
	bool has_pixelv(Ref<Image> p_image, const Vector2 &p_pos);
	Variant get_pixel_or_null(Ref<Image> p_image, int x, int y);
	Variant get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos);

	_GoostImage();
};

VARIANT_ENUM_CAST(_GoostImage::Connectivity);
VARIANT_ENUM_CAST(_GoostImage::Direction);

#endif
