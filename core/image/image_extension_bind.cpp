#include "image_extension_bind.h"
#include "image_extension.h"

_ImageExtension *_ImageExtension::singleton = NULL;

void _ImageExtension::replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color) {

	ImageExtension::replace_color(p_image, p_color, p_with_color);
}

Ref<Image> _ImageExtension::bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image, KernelConnectivity p_kc) {

	return ImageExtension::bucket_fill(p_image, p_at, p_fill_color, p_fill_image, ImageExtension::KernelConnectivity(p_kc));
}

void _ImageExtension::resize_hqx(Ref<Image> p_image, int p_scale) {
	return ImageExtension::resize_hqx(p_image, p_scale);
}

void _ImageExtension::rotate(Ref<Image> p_image, real_t p_angle) {
	return ImageExtension::rotate(p_image, p_angle);
}

bool _ImageExtension::has_pixel(Ref<Image> p_image, int x, int y) {

	return ImageExtension::has_pixel(p_image, x, y);
}

bool _ImageExtension::has_pixelv(Ref<Image> p_image, const Vector2 &p_pos) {

	return ImageExtension::has_pixelv(p_image, p_pos);
}

Variant _ImageExtension::get_pixel_or_null(Ref<Image> p_image, int x, int y) {
	Variant ret;
	Color pixel;
	if(ImageExtension::get_pixel_or_null(p_image, x, y, &pixel)) {
		ret = pixel;
	}
	return ret;
}

Variant _ImageExtension::get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos) {
	
	return _ImageExtension::get_pixel_or_null(p_image, p_pos.x, p_pos.y);
}

void _ImageExtension::_bind_methods() {

	// Image methods
	ClassDB::bind_method(D_METHOD("replace_color", "image", "color", "with_color"), &_ImageExtension::replace_color);
	ClassDB::bind_method(D_METHOD("bucket_fill", "image", "at", "fill_color", "fill_image", "connectivity"), &_ImageExtension::bucket_fill, DEFVAL(true), DEFVAL(KERNEL_FOUR_WAY));
	ClassDB::bind_method(D_METHOD("resize_hqx", "image", "scale"), &_ImageExtension::resize_hqx, DEFVAL(2));
	ClassDB::bind_method(D_METHOD("rotate", "image", "angle"), &_ImageExtension::rotate);

	// Pixel methods
	ClassDB::bind_method(D_METHOD("get_pixel_or_null", "image", "x", "y"), &_ImageExtension::get_pixel_or_null);
	ClassDB::bind_method(D_METHOD("get_pixelv_or_null", "image", "pos"), &_ImageExtension::get_pixelv_or_null);
	ClassDB::bind_method(D_METHOD("has_pixel", "image", "x", "y"), &_ImageExtension::has_pixel);
	ClassDB::bind_method(D_METHOD("has_pixelv", "image", "pos"), &_ImageExtension::has_pixelv);

	BIND_ENUM_CONSTANT(KERNEL_FOUR_WAY);
	BIND_ENUM_CONSTANT(KERNEL_EIGHT_WAY);
}

_ImageExtension::_ImageExtension() {
	singleton = this;
}
