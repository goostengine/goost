#include "image_blender.h"

Color ImageBlender::calculate_factor(const Color &p_src, const Color &p_dst, BlendFactor p_factor) const {
	Color color_factor;

	switch (p_factor) {
		case FACTOR_ZERO: {
			color_factor = Color(0, 0, 0, 0);
		} break;
		case FACTOR_ONE: {
			color_factor = Color(1, 1, 1, 1);
		} break;
		case FACTOR_SRC_COLOR: {
			color_factor = p_src;
		} break;
		case FACTOR_ONE_MINUS_SRC_COLOR: {
			color_factor = Color(1, 1, 1, 1) - p_src;
		} break;
		case FACTOR_DST_COLOR: {
			color_factor = p_dst;
		} break;
		case FACTOR_ONE_MINUS_DST_COLOR: {
			color_factor = Color(1, 1, 1, 1) - p_dst;
		} break;
		case FACTOR_SRC_ALPHA: {
			color_factor = Color(p_src.a, p_src.a, p_src.a, p_src.a);
		} break;
		case FACTOR_ONE_MINUS_SRC_ALPHA: {
			color_factor = Color(1, 1, 1, 1) - Color(p_src.a, p_src.a, p_src.a, p_src.a);
		} break;
		case FACTOR_DST_ALPHA: {
			color_factor = Color(p_dst.a, p_dst.a, p_dst.a, p_dst.a);
		} break;
		case FACTOR_ONE_MINUS_DST_ALPHA: {
			color_factor = Color(1, 1, 1, 1) - Color(p_dst.a, p_dst.a, p_dst.a, p_dst.a);
		} break;
		case FACTOR_DST_ALPHA_TIMES_ONE_MINUS_SRC_ALPHA: {
			color_factor = Color(p_dst.a, p_dst.a, p_dst.a, p_dst.a) * (Color(1, 1, 1, 1) - Color(p_src.a, p_src.a, p_src.a, p_src.a));
		} break;
	}
	return color_factor;
}

Color ImageBlender::blend_colors(const Color &p_src, const Color &p_dst) const {
	Color color;

	switch (rgb_equation) {
		case FUNC_ADD: {
			color = p_src * calculate_rgb_src_factor(p_src, p_dst) + p_dst * calculate_rgb_dst_factor(p_src, p_dst);
		} break;
		case FUNC_SUBTRACT: {
			color = p_src * calculate_rgb_src_factor(p_src, p_dst) - p_dst * calculate_rgb_dst_factor(p_src, p_dst);
		} break;
		case FUNC_REVERSE_SUBTRACT: {
			color = p_dst * calculate_rgb_dst_factor(p_src, p_dst) - p_src * calculate_rgb_src_factor(p_src, p_dst);
		} break;
		case FUNC_MAX: {
			color.r = MAX(p_src.r, p_dst.r);
			color.g = MAX(p_src.g, p_dst.g);
			color.b = MAX(p_src.b, p_dst.b);
		} break;
		case FUNC_MIN: {
			color.r = MIN(p_src.r, p_dst.r);
			color.g = MIN(p_src.g, p_dst.g);
			color.b = MIN(p_src.b, p_dst.b);
		} break;
	}
	switch (alpha_equation) {
		case FUNC_ADD: {
			color.a = (p_src * calculate_alpha_src_factor(p_src, p_dst)).a + (p_dst * calculate_alpha_dst_factor(p_src, p_dst)).a;
		} break;
		case FUNC_SUBTRACT: {
			color.a = (p_src * calculate_alpha_src_factor(p_src, p_dst)).a - (p_dst * calculate_alpha_dst_factor(p_src, p_dst)).a;
		} break;
		case FUNC_REVERSE_SUBTRACT: {
			color.a = (p_dst * calculate_alpha_dst_factor(p_src, p_dst)).a - (p_src * calculate_alpha_src_factor(p_src, p_dst)).a;
		} break;
		case FUNC_MAX: {
			color.a = MAX(p_src.a, p_dst.a);
		} break;
		case FUNC_MIN: {
			color.a = MIN(p_src.a, p_dst.a);
		} break;
	}
	return color;
}

void ImageBlender::blend_rect(const Ref<Image> p_src, const Rect2 &p_src_rect, Ref<Image> p_dst, const Point2 &p_dst_pos) const {
	ERR_FAIL_COND_MSG(p_dst.is_null(), "It's not a reference to a valid Image object.");
	ERR_FAIL_COND_MSG(p_src.is_null(), "It's not a reference to a valid Image object.");
	int dsize = p_dst->get_data().size();
	int srcdsize = p_src->get_data().size();
	ERR_FAIL_COND(dsize == 0);
	ERR_FAIL_COND(srcdsize == 0);
	ERR_FAIL_COND(p_dst->get_format() != p_src->get_format());

	Rect2i clipped_src_rect = Rect2i(0, 0, p_src->get_width(), p_src->get_height()).clip(p_src_rect);

	if (p_dst_pos.x < 0)
		clipped_src_rect.position.x = ABS(p_dst_pos.x);
	if (p_dst_pos.y < 0)
		clipped_src_rect.position.y = ABS(p_dst_pos.y);

	if (clipped_src_rect.size.x <= 0 || clipped_src_rect.size.y <= 0)
		return;

	Point2 src_underscan = Point2(MIN(0, p_src_rect.position.x), MIN(0, p_src_rect.position.y));
	Rect2i dest_rect = Rect2i(0, 0, p_dst->get_width(), p_dst->get_height()).clip(Rect2i(p_dst_pos - src_underscan, clipped_src_rect.size));

	p_dst->lock();
	Ref<Image> img = p_src;
	img->lock();

	for (int i = 0; i < dest_rect.size.y; i++) {
		for (int j = 0; j < dest_rect.size.x; j++) {
			int src_x = clipped_src_rect.position.x + j;
			int src_y = clipped_src_rect.position.y + i;

			int dst_x = dest_rect.position.x + j;
			int dst_y = dest_rect.position.y + i;

			Color sc = img->get_pixel(src_x, src_y);
			Color dc = p_dst->get_pixel(dst_x, dst_y);

			p_dst->set_pixel(dst_x, dst_y, blend_colors(sc, dc));
		}
	}
	img->unlock();
	p_dst->unlock();
}

void ImageBlender::stamp_rect(const Ref<Image> p_src, const Rect2 &p_src_rect, Ref<Image> p_dst, const Point2 &p_dst_init_pos, const Point2 &p_dst_end_pos, float p_spacing) const {
	ERR_FAIL_COND_MSG(p_dst.is_null(), "It's not a reference to a valid Image object.");
	ERR_FAIL_COND_MSG(p_src.is_null(), "It's not a reference to a valid Image object.");
	int dsize = p_dst->get_data().size();
	int srcdsize = p_src->get_data().size();
	ERR_FAIL_COND(dsize == 0);
	ERR_FAIL_COND(srcdsize == 0);
	ERR_FAIL_COND(p_dst->get_format() != p_src->get_format());
	ERR_FAIL_COND(p_spacing <= 0);

	float semi_width = p_src_rect.get_size().width / 2;
	float semi_height = p_src_rect.get_size().height / 2;

	Point2 start_point(p_dst_init_pos.x - semi_width, p_dst_init_pos.y - semi_height);
	Point2 end_point(p_dst_end_pos.x - semi_width, p_dst_end_pos.y - semi_height);

	float delta_x = end_point.x - start_point.x;
	float delta_y = end_point.y - start_point.y;

	float distance = Math::sqrt(delta_x * delta_x + delta_y * delta_y);

	float step_x = 0.0;
	float step_y = 0.0;
	if (distance > 0.0) {
		float reciprocal_distance = 1.0 / distance;
		step_x = delta_x * reciprocal_distance;
		step_y = delta_y * reciprocal_distance;
	}

	float offset_x = 0.0;
	float offset_y = 0.0;

	do {
		blend_rect(p_src, p_src_rect, p_dst, Point2(start_point.x + offset_x, start_point.y + offset_y));

		offset_x += step_x * p_spacing;
		offset_y += step_y * p_spacing;

		distance -= p_spacing;
	} while (distance >= p_spacing);
}

void ImageBlender::_bind_methods() {
	ClassDB::bind_method(D_METHOD("blend_rect", "src", "src_rect", "dst", "dst_pos"), &ImageBlender::blend_rect);
	ClassDB::bind_method(D_METHOD("stamp_rect", "src", "src_rect", "dst", "dst_init_pos", "dst_end_pos", "spacing"), &ImageBlender::stamp_rect);

	ClassDB::bind_method(D_METHOD("set_rgb_equation", "equation"), &ImageBlender::set_rgb_equation);
	ClassDB::bind_method(D_METHOD("get_rgb_equation"), &ImageBlender::get_rgb_equation);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rgb_equation"), "set_rgb_equation", "get_rgb_equation");

	ClassDB::bind_method(D_METHOD("set_alpha_equation", "equation"), &ImageBlender::set_alpha_equation);
	ClassDB::bind_method(D_METHOD("get_alpha_equation"), &ImageBlender::get_alpha_equation);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "alpha_equation"), "set_alpha_equation", "get_alpha_equation");

	ClassDB::bind_method(D_METHOD("set_rgb_src_factor", "factor"), &ImageBlender::set_rgb_src_factor);
	ClassDB::bind_method(D_METHOD("get_rgb_src_factor"), &ImageBlender::get_rgb_src_factor);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rgb_src_factor"), "set_rgb_src_factor", "get_rgb_src_factor");

	ClassDB::bind_method(D_METHOD("set_rgb_dst_factor", "factor"), &ImageBlender::set_rgb_dst_factor);
	ClassDB::bind_method(D_METHOD("get_rgb_dst_factor"), &ImageBlender::get_rgb_dst_factor);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rgb_dst_factor"), "set_rgb_dst_factor", "get_rgb_dst_factor");

	ClassDB::bind_method(D_METHOD("set_alpha_src_factor", "factor"), &ImageBlender::set_alpha_src_factor);
	ClassDB::bind_method(D_METHOD("get_alpha_src_factor"), &ImageBlender::get_alpha_src_factor);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "alpha_src_factor"), "set_alpha_src_factor", "get_alpha_src_factor");

	ClassDB::bind_method(D_METHOD("set_alpha_dst_factor", "factor"), &ImageBlender::set_alpha_dst_factor);
	ClassDB::bind_method(D_METHOD("get_alpha_dst_factor"), &ImageBlender::get_alpha_dst_factor);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "alpha_dst_factor"), "set_alpha_dst_factor", "get_alpha_dst_factor");

	BIND_ENUM_CONSTANT(FUNC_ADD);
	BIND_ENUM_CONSTANT(FUNC_SUBTRACT);
	BIND_ENUM_CONSTANT(FUNC_REVERSE_SUBTRACT);
	BIND_ENUM_CONSTANT(FUNC_MIN);
	BIND_ENUM_CONSTANT(FUNC_MAX);
	BIND_ENUM_CONSTANT(FACTOR_ZERO);

	BIND_ENUM_CONSTANT(FACTOR_ONE);
	BIND_ENUM_CONSTANT(FACTOR_SRC_COLOR);
	BIND_ENUM_CONSTANT(FACTOR_ONE_MINUS_SRC_COLOR);
	BIND_ENUM_CONSTANT(FACTOR_DST_COLOR);
	BIND_ENUM_CONSTANT(FACTOR_ONE_MINUS_DST_COLOR);
	BIND_ENUM_CONSTANT(FACTOR_SRC_ALPHA);
	BIND_ENUM_CONSTANT(FACTOR_ONE_MINUS_SRC_ALPHA);
	BIND_ENUM_CONSTANT(FACTOR_DST_ALPHA);
	BIND_ENUM_CONSTANT(FACTOR_ONE_MINUS_DST_ALPHA);
	BIND_ENUM_CONSTANT(FACTOR_DST_ALPHA_TIMES_ONE_MINUS_SRC_ALPHA);
}
