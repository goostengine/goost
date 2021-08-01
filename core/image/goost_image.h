#pragma once

#include "core/image.h"

class GoostImage {
public:
	enum Connectivity {
		FOUR_CONNECTED,
		EIGHT_CONNECTED,
	};
	enum MorphOperation {
		MORPH_DILATE,
		MORPH_ERODE,
		MORPH_OPEN,
		MORPH_CLOSE,
	};
	enum Direction {
		CW = 1,
		CCW = -1,
	};
	enum WrapMode {
		TILE,
		TILE_FLIP_X,
		TILE_FLIP_Y,
		TILE_FLIP_XY,
	};

public:
	// Image processing methods.
	static void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);
	static Ref<Image> bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image = true, Connectivity p_con = FOUR_CONNECTED);
	static void resize_hqx(Ref<Image> p_image, int p_scale = 2);

	static void rotate(Ref<Image> p_image, real_t p_angle, bool p_expand = true);
	static void rotate_90(Ref<Image> p_image, Direction p_direction);
	static void rotate_180(Ref<Image> p_image);

	static void binarize(Ref<Image> p_image, real_t p_threshold = -1, bool p_invert = false);

	static void dilate(Ref<Image> p_image, int p_kernel_size = 3);
	static void erode(Ref<Image> p_image, int p_kernel_size = 3);
	static void morph(Ref<Image> p_image, MorphOperation p_op, Size2i p_kernel_size = Size2i(3, 3));

	static Ref<Image> tile(const Ref<Image> &p_image, const Size2i &p_size, WrapMode p_mode);
	static Ref<Image> repeat(const Ref<Image> &p_image, const Size2i &p_count, WrapMode p_mode, const Size2i &p_max_size);

	// Image analysis methods.
	static Point2 get_centroid(const Ref<Image> &p_image);

	// Image generation methods.
	static Ref<Image> render_polygon(Vector<Point2> p_polygon, bool p_fill = false, const Color &p_color = Color(1, 1, 1, 1), const Color &p_bg_color = Color(0, 0, 0, 0));
	static Ref<Image> render_svg(const String &p_svg, real_t p_scale = 1.0);

	// Pixel methods.
	static bool has_pixel(Ref<Image> p_image, int x, int y);
	static bool has_pixelv(Ref<Image> p_image, const Vector2 &p_pos);
	static bool get_pixel_or_null(Ref<Image> p_image, int x, int y, Color *r_pixel = nullptr);
	static bool get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos, Color *r_pixel = nullptr);

	static Color get_pixel_average(const Ref<Image> &p_image, const Rect2 &p_rect = Rect2(), const Ref<Image> &p_mask = nullptr);
};

