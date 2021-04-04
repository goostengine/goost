#ifndef GOOST_LIGHT_TEXTURE
#define GOOST_LIGHT_TEXTURE

#include "gradient_texture_2d.h"

class LightTexture : public GradientTexture2D {
	GDCLASS(LightTexture, GradientTexture2D);

	LightTexture();
};

#endif // GOOST_LIGHT_TEXTURE
