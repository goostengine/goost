#ifndef GOOST_IMAGE_BLENDER_H
#define GOOST_IMAGE_BLENDER_H

#include "core/image.h"

class ImageBlender : public Reference {
	GDCLASS(ImageBlender, Reference);

protected:
	static void _bind_methods();

public:
	enum BlendEquation {
		FUNC_ADD,
		FUNC_SUBTRACT,
		FUNC_REVERSE_SUBTRACT,
		FUNC_MIN,
		FUNC_MAX
	};

	enum BlendFactor {
		FACTOR_ZERO,
		FACTOR_ONE,
		FACTOR_SRC_COLOR,
		FACTOR_ONE_MINUS_SRC_COLOR,
		FACTOR_DST_COLOR,
		FACTOR_ONE_MINUS_DST_COLOR,
		FACTOR_SRC_ALPHA,
		FACTOR_ONE_MINUS_SRC_ALPHA,
		FACTOR_DST_ALPHA,
		FACTOR_ONE_MINUS_DST_ALPHA,
		FACTOR_DST_ALPHA_TIMES_ONE_MINUS_SRC_ALPHA
	};

private:
	BlendEquation rgb_equation = FUNC_ADD;
	BlendEquation alpha_equation = FUNC_ADD;

	BlendFactor rgb_src_factor = FACTOR_SRC_ALPHA;
	BlendFactor rgb_dst_factor = FACTOR_DST_ALPHA_TIMES_ONE_MINUS_SRC_ALPHA;
	BlendFactor alpha_src_factor = FACTOR_ONE;
	BlendFactor alpha_dst_factor = FACTOR_ONE_MINUS_SRC_ALPHA;

	Color calculate_factor(const Color &p_src, const Color &p_dst, BlendFactor p_factor) const;

	Color calculate_rgb_src_factor(const Color &p_src, const Color &p_dst) const { return calculate_factor(p_src, p_dst, rgb_src_factor); };
	Color calculate_rgb_dst_factor(const Color &p_src, const Color &p_dst) const { return calculate_factor(p_src, p_dst, rgb_dst_factor); };
	Color calculate_alpha_src_factor(const Color &p_src, const Color &p_dst) const { return calculate_factor(p_src, p_dst, alpha_src_factor); };
	Color calculate_alpha_dst_factor(const Color &p_src, const Color &p_dst) const { return calculate_factor(p_src, p_dst, alpha_dst_factor); };

	Color blend_colors(const Color &p_src, const Color &p_dst) const;

public:
	void blend_rect(const Ref<Image> p_src, const Rect2 &p_src_rect, Ref<Image> p_dst, const Point2 &p_dst_pos) const;

	void set_rgb_equation(BlendEquation p_equation) { rgb_equation = p_equation; };
	void set_alpha_equation(BlendEquation p_equation) { alpha_equation = p_equation; };

	void set_rgb_src_factor(BlendFactor p_factor) { rgb_src_factor = p_factor; };
	void set_rgb_dst_factor(BlendFactor p_factor) { rgb_dst_factor = p_factor; };
	void set_alpha_src_factor(BlendFactor p_factor) { alpha_src_factor = p_factor; };
	void set_alpha_dst_factor(BlendFactor p_factor) { alpha_dst_factor = p_factor; };

	BlendEquation get_rgb_equation() const { return rgb_equation; };
	BlendEquation get_alpha_equation() const { return alpha_equation; };

	BlendFactor get_rgb_src_factor() const { return rgb_src_factor; };
	BlendFactor get_rgb_dst_factor() const { return rgb_dst_factor; };
	BlendFactor get_alpha_src_factor() const { return alpha_src_factor; };
	BlendFactor get_alpha_dst_factor() const { return alpha_dst_factor; };
};

VARIANT_ENUM_CAST(ImageBlender::BlendEquation);
VARIANT_ENUM_CAST(ImageBlender::BlendFactor);

#endif // GOOST_IMAGE_BLENDER_H
