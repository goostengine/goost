#include "goost_image_bind.h"
#include "goost_image.h"

_GoostImage *_GoostImage::singleton = NULL;

void _GoostImage::replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color) {
	GoostImage::replace_color(p_image, p_color, p_with_color);
}

Ref<Image> _GoostImage::bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image, Connectivity p_con) {
	return GoostImage::bucket_fill(p_image, p_at, p_fill_color, p_fill_image, GoostImage::Connectivity(p_con));
}

void _GoostImage::resize_hqx(Ref<Image> p_image, int p_scale) {
	GoostImage::resize_hqx(p_image, p_scale);
}

void _GoostImage::rotate(Ref<Image> p_image, real_t p_angle, bool p_expand) {
	GoostImage::rotate(p_image, p_angle, p_expand);
}

void _GoostImage::rotate_90(Ref<Image> p_image, Direction p_direction) {
	GoostImage::rotate_90(p_image, GoostImage::Direction(p_direction));
}

void _GoostImage::rotate_180(Ref<Image> p_image) {
	GoostImage::rotate_180(p_image);
}

Ref<Image> _GoostImage::render_polygon(Vector<Point2> p_polygon, bool p_fill, const Color &p_color, const Color &p_bg_color) {
	return GoostImage::render_polygon(p_polygon, p_fill, p_color, p_bg_color);
}

bool _GoostImage::has_pixel(Ref<Image> p_image, int x, int y) {
	return GoostImage::has_pixel(p_image, x, y);
}

bool _GoostImage::has_pixelv(Ref<Image> p_image, const Vector2 &p_pos) {
	return GoostImage::has_pixelv(p_image, p_pos);
}

Variant _GoostImage::get_pixel_or_null(Ref<Image> p_image, int x, int y) {
	Variant ret;
	Color pixel;
	if (GoostImage::get_pixel_or_null(p_image, x, y, &pixel)) {
		ret = pixel;
	}
	return ret;
}

Variant _GoostImage::get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos) {
	return _GoostImage::get_pixel_or_null(p_image, p_pos.x, p_pos.y);
}

void _GoostImage::_bind_methods() {
	// Image methods
	ClassDB::bind_method(D_METHOD("replace_color", "image", "color", "with_color"), &_GoostImage::replace_color);
	ClassDB::bind_method(D_METHOD("bucket_fill", "image", "at", "fill_color", "fill_image", "connectivity"), &_GoostImage::bucket_fill, DEFVAL(true), DEFVAL(FOUR_CONNECTED));
	ClassDB::bind_method(D_METHOD("resize_hqx", "image", "scale"), &_GoostImage::resize_hqx, DEFVAL(2));
	ClassDB::bind_method(D_METHOD("rotate", "image", "angle", "expand"), &_GoostImage::rotate, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("rotate_90", "image", "direction"), &_GoostImage::rotate_90);
	ClassDB::bind_method(D_METHOD("rotate_180", "image"), &_GoostImage::rotate_180);
	ClassDB::bind_method(D_METHOD("render_polygon", "polygon", "fill", "foreground_color", "background_color"), &_GoostImage::render_polygon, DEFVAL(false), DEFVAL(Color(1, 1, 1, 1)), DEFVAL(Color(0, 0, 0, 0)));

	// Pixel methods
	ClassDB::bind_method(D_METHOD("get_pixel_or_null", "image", "x", "y"), &_GoostImage::get_pixel_or_null);
	ClassDB::bind_method(D_METHOD("get_pixelv_or_null", "image", "pos"), &_GoostImage::get_pixelv_or_null);
	ClassDB::bind_method(D_METHOD("has_pixel", "image", "x", "y"), &_GoostImage::has_pixel);
	ClassDB::bind_method(D_METHOD("has_pixelv", "image", "pos"), &_GoostImage::has_pixelv);

	BIND_ENUM_CONSTANT(FOUR_CONNECTED);
	BIND_ENUM_CONSTANT(EIGHT_CONNECTED);

	BIND_ENUM_CONSTANT(CW);
	BIND_ENUM_CONSTANT(CCW);
}

_GoostImage::_GoostImage() {
	singleton = this;
}
