#include "light_texture.h"

LightTexture::LightTexture() {
	set_fill(GradientTexture2D::FILL_RADIAL);
	set_fill_from(Vector2(0.5, 0.5)); // Center!
	set_fill_to(Vector2(0.5, 0.0));

	Ref<Gradient> g;
	g.instance();	
	// Invert default black and white colors.
	g->set_color(0, Color(1, 1, 1, 1));
	g->set_color(1, Color(0, 0, 0, 0));
	// Make sure it's updated upon gradient changes.
	set_gradient(g); 
}
