#pragma once

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
	void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);
	Ref<Image> bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image = true, Connectivity p_con = FOUR_CONNECTED);
	void resize_hqx(Ref<Image> p_image, int p_scale);

	void rotate(Ref<Image> p_image, real_t p_angle, bool p_expand);
	void rotate_90(Ref<Image> p_image, Direction p_direction);
	void rotate_180(Ref<Image> p_image);

	void binarize(Ref<Image> p_image, real_t p_threshold = -1, bool p_invert = false);

	void dilate(Ref<Image> p_image, int p_kernel_size = 3);
	void erode(Ref<Image> p_image, int p_kernel_size = 3);
	void morph(Ref<Image> p_image, MorphOperation p_op, const Vector2 &p_kernel_size = Size2i(3, 3));

	Ref<Image> tile(const Ref<Image> &p_image, const Vector2 &p_size, WrapMode p_mode);
	Ref<Image> repeat(const Ref<Image> &p_image, const Vector2 &p_count, WrapMode p_mode, const Vector2 &p_max_size);

	Vector2 get_centroid(const Ref<Image> &p_image);

	Ref<Image> render_polygon(Vector<Point2> p_polygon, bool p_fill, const Color &p_color, const Color &p_bg_color);
	Ref<Image> render_svg(const String &p_svg, real_t p_scale = 1.0);

	bool has_pixel(Ref<Image> p_image, int x, int y);
	bool has_pixelv(Ref<Image> p_image, const Vector2 &p_pos);
	Variant get_pixel_or_null(Ref<Image> p_image, int x, int y);
	Variant get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos);

	Color get_pixel_average(const Ref<Image> &p_image, const Rect2 &p_rect = Rect2(), const Ref<Image> &p_mask = nullptr);

	_GoostImage();
};

VARIANT_ENUM_CAST(_GoostImage::Connectivity);
VARIANT_ENUM_CAST(_GoostImage::MorphOperation);
VARIANT_ENUM_CAST(_GoostImage::Direction);
VARIANT_ENUM_CAST(_GoostImage::WrapMode);

