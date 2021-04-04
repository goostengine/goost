#ifndef GOOST_GRADIENT_TEXTURE_2D
#define GOOST_GRADIENT_TEXTURE_2D

#include "scene/resources/texture.h"

class GradientTexture2D : public Texture {
	GDCLASS(GradientTexture2D, Texture);

public:
	enum Fill {
		FILL_LINEAR,
		FILL_RADIAL,
	};
	enum Repeat {
		REPEAT_NONE,
		REPEAT,
		REPEAT_MIRROR,
	};

private:
	RID texture;
	float _get_gradient_offset_at(int x, int y) const;
	bool update_pending = false;

protected:
	Ref<Gradient> gradient;

	int width = 64;
	int height = 64;

	Vector2 fill_from;
	Vector2 fill_to = Vector2(1, 0);

	Fill fill = FILL_LINEAR;
	Repeat repeat = REPEAT_NONE;

	static void _bind_methods();

	void _queue_update();
	void _update();

public:
	void set_gradient(Ref<Gradient> p_gradient);
	Ref<Gradient> get_gradient() const { return gradient; }

	void set_width(int p_width);
	virtual int get_width() const { return width; }
	void set_height(int p_height);
	virtual int get_height() const { return height; };

	void set_fill(Fill p_fill);
	Fill get_fill() const { return fill; }
	void set_fill_from(Vector2 p_fill_from);
	Vector2 get_fill_from() const { return fill_from; }
	void set_fill_to(Vector2 p_fill_to);
	Vector2 get_fill_to() const { return fill_to; }

	void set_repeat(Repeat p_repeat);
	Repeat get_repeat() const { return repeat; }

	virtual void set_flags(uint32_t p_flags) {}
	virtual uint32_t get_flags() const { return FLAG_FILTER; }

	virtual RID get_rid() const { return texture; }
	virtual bool has_alpha() const { return true; }
	virtual Ref<Image> get_data() const;

	GradientTexture2D();
	virtual ~GradientTexture2D();
};

VARIANT_ENUM_CAST(GradientTexture2D::Fill);
VARIANT_ENUM_CAST(GradientTexture2D::Repeat);

#endif
