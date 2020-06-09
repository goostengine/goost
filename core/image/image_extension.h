#ifndef GOOST_IMAGE_H
#define GOOST_IMAGE_H

#include "core/image.h"

class ImageExtension {

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
	static void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);
	static Ref<Image> bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image = true, Connectivity p_con = FOUR_CONNECTED);
	static void resize_hqx(Ref<Image> p_image, int p_scale = 2);
	static void rotate(Ref<Image> p_image, real_t p_angle);
	static void rotate_90(Ref<Image> p_image, Direction p_direction);
	static void rotate_180(Ref<Image> p_image);
	static Ref<Image> render_polygon(Vector<Point2> p_polygon, bool p_fill = false, const Color &p_color = Color(1,1,1,1), const Color &p_bg_color = Color(0,0,0,0));

	// Pixel methods
	static bool has_pixel(Ref<Image> p_image, int x, int y);
	static bool has_pixelv(Ref<Image> p_image, const Vector2 &p_pos);
	static bool get_pixel_or_null(Ref<Image> p_image, int x, int y, Color* r_pixel = NULL);
	static bool get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos, Color* r_pixel = NULL);
};

#endif
