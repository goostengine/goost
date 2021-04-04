#include "light_texture.h"

LightTexture::LightTexture() {
	fill = GradientTexture2D::FILL_RADIAL;
	fill_from = Vector2(0.5, 0.5); // Center!
	fill_to = Vector2(0.5, 0.0);
	gradient = Ref<Gradient>(memnew(Gradient));
	// Invert default black and white colors.
	gradient->set_color(0, Color(1, 1, 1, 1));
	gradient->set_color(1, Color(0, 0, 0, 0));
	set_gradient(gradient); // Make sure it's updated upon gradient changes.
}
