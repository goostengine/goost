#ifndef GOOST_IMAGE_H
#define GOOST_IMAGE_H

#include "core/image.h"

class ImageExtension {

public:
	enum KernelConnectivity {
		KERNEL_FOUR_WAY,
		KERNEL_EIGHT_WAY,
	};

public:
	// Image methods
	static void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);
	static Ref<Image> bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image = true, KernelConnectivity p_kc = KERNEL_FOUR_WAY);
	static void resize_hqx(Ref<Image> p_image, int p_scale = 2);
	static void rotate(Ref<Image> p_image, real_t p_angle);

	// Pixel methods
	static bool has_pixel(Ref<Image> p_image, int x, int y);
	static bool has_pixelv(Ref<Image> p_image, const Vector2 &p_pos);
	static bool get_pixel_or_null(Ref<Image> p_image, int x, int y, Color* r_pixel = NULL);
	static bool get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos, Color* r_pixel = NULL);
};

#endif
