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

void _GoostImage::binarize(Ref<Image> p_image, real_t p_threshold, bool p_invert) {
	GoostImage::binarize(p_image, p_threshold, p_invert);
}

void _GoostImage::dilate(Ref<Image> p_image, int p_kernel_size) {
	GoostImage::dilate(p_image, p_kernel_size);
}

void _GoostImage::erode(Ref<Image> p_image, int p_kernel_size) {
	GoostImage::erode(p_image, p_kernel_size);
}

void _GoostImage::morph(Ref<Image> p_image, MorphOperation p_op, const Vector2 &p_kernel_size) {
	GoostImage::morph(p_image, GoostImage::MorphOperation(p_op), p_kernel_size);
}

Ref<Image> _GoostImage::repeat(const Ref<Image> &p_image, const Vector2 &p_count, WrapMode p_mode, const Vector2 &p_max_size) {
	return GoostImage::repeat(p_image, p_count, GoostImage::WrapMode(p_mode), p_max_size);
}

Ref<Image> _GoostImage::tile(const Ref<Image> &p_image, const Vector2 &p_size, WrapMode p_mode) {
	return GoostImage::tile(p_image, p_size, GoostImage::WrapMode(p_mode));
}

Vector2 _GoostImage::get_centroid(const Ref<Image> &p_image) {
	return GoostImage::get_centroid(p_image);
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

Color _GoostImage::get_pixel_average(const Ref<Image> &p_image, const Rect2 &p_rect, const Ref<Image> &p_mask) {
	return GoostImage::get_pixel_average(p_image, p_rect, p_mask);
}

void _GoostImage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("replace_color", "image", "color", "with_color"), &_GoostImage::replace_color);
	ClassDB::bind_method(D_METHOD("bucket_fill", "image", "at", "fill_color", "fill_image", "connectivity"), &_GoostImage::bucket_fill, DEFVAL(true), DEFVAL(FOUR_CONNECTED));
	ClassDB::bind_method(D_METHOD("resize_hqx", "image", "scale"), &_GoostImage::resize_hqx, DEFVAL(2));

	ClassDB::bind_method(D_METHOD("rotate", "image", "angle", "expand"), &_GoostImage::rotate, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("rotate_90", "image", "direction"), &_GoostImage::rotate_90);
	ClassDB::bind_method(D_METHOD("rotate_180", "image"), &_GoostImage::rotate_180);

	ClassDB::bind_method(D_METHOD("binarize", "image", "threshold", "invert"), &_GoostImage::binarize, DEFVAL(-1), DEFVAL(false));

	ClassDB::bind_method(D_METHOD("dilate", "image", "kernel_size"), &_GoostImage::dilate, DEFVAL(3));
	ClassDB::bind_method(D_METHOD("erode", "image", "kernel_size"), &_GoostImage::erode, DEFVAL(3));
	ClassDB::bind_method(D_METHOD("morph", "image", "operation", "kernel_size"), &_GoostImage::morph, DEFVAL(Vector2(3, 3)));

	ClassDB::bind_method(D_METHOD("tile", "image", "size", "wrap_mode"), &_GoostImage::tile, DEFVAL(TILE));
	ClassDB::bind_method(D_METHOD("repeat", "image", "count", "wrap_mode", "max_size"), &_GoostImage::repeat, DEFVAL(TILE), DEFVAL(Vector2(Image::MAX_WIDTH, Image::MAX_HEIGHT)));

	ClassDB::bind_method(D_METHOD("get_centroid", "image"), &_GoostImage::get_centroid);

	ClassDB::bind_method(D_METHOD("render_polygon", "polygon", "fill", "foreground_color", "background_color"), &_GoostImage::render_polygon, DEFVAL(false), DEFVAL(Color(1, 1, 1, 1)), DEFVAL(Color(0, 0, 0, 0)));

	ClassDB::bind_method(D_METHOD("get_pixel_or_null", "image", "x", "y"), &_GoostImage::get_pixel_or_null);
	ClassDB::bind_method(D_METHOD("get_pixelv_or_null", "image", "pos"), &_GoostImage::get_pixelv_or_null);
	ClassDB::bind_method(D_METHOD("has_pixel", "image", "x", "y"), &_GoostImage::has_pixel);
	ClassDB::bind_method(D_METHOD("has_pixelv", "image", "pos"), &_GoostImage::has_pixelv);

	ClassDB::bind_method(D_METHOD("get_pixel_average", "image", "rect", "mask"), &_GoostImage::get_pixel_average, DEFVAL(Rect2()), DEFVAL(Variant()));

	BIND_ENUM_CONSTANT(FOUR_CONNECTED);
	BIND_ENUM_CONSTANT(EIGHT_CONNECTED);

	BIND_ENUM_CONSTANT(MORPH_DILATE);
	BIND_ENUM_CONSTANT(MORPH_ERODE);
	BIND_ENUM_CONSTANT(MORPH_OPEN);
	BIND_ENUM_CONSTANT(MORPH_CLOSE);

	BIND_ENUM_CONSTANT(CW);
	BIND_ENUM_CONSTANT(CCW);

	BIND_ENUM_CONSTANT(TILE);
	BIND_ENUM_CONSTANT(TILE_FLIP_X);
	BIND_ENUM_CONSTANT(TILE_FLIP_Y);
	BIND_ENUM_CONSTANT(TILE_FLIP_XY);
}

_GoostImage::_GoostImage() {
	singleton = this;
}
