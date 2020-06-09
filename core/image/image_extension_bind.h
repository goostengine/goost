#ifndef GOOST_IMAGE_BIND_H
#define GOOST_IMAGE_BIND_H

#include "core/image.h"

class _ImageExtension : public Object {
	GDCLASS(_ImageExtension, Object);

private:
	static _ImageExtension *singleton;

protected:
	static void _bind_methods();

public:
	static _ImageExtension *get_singleton() { return singleton; }

public:
	enum KernelConnectivity {
		KERNEL_FOUR_WAY,
		KERNEL_EIGHT_WAY,
	};
	enum Direction {
		CW = 1,
		CCW = -1,
	};
public:
	// Image methods
	void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);
	Ref<Image> bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image = true, KernelConnectivity p_kc = KERNEL_FOUR_WAY);
	void resize_hqx(Ref<Image> p_image, int p_scale);
	void rotate(Ref<Image> p_image, real_t p_angle);
	void rotate_90(Ref<Image> p_image, Direction p_direction);
	void rotate_180(Ref<Image> p_image);
	Ref<Image> render_polygon(Vector<Point2> p_polygon);

	// Pixel methods
	bool has_pixel(Ref<Image> p_image, int x, int y);
	bool has_pixelv(Ref<Image> p_image, const Vector2 &p_pos);
	Variant get_pixel_or_null(Ref<Image> p_image, int x, int y);
	Variant get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos);

	_ImageExtension();
};

VARIANT_ENUM_CAST(_ImageExtension::KernelConnectivity);
VARIANT_ENUM_CAST(_ImageExtension::Direction);

#endif
