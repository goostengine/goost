#include "variant_resource_preview.h"

bool VariantResourcePreviewGenerator::handles(const String &p_type) const {
	return p_type == "VariantResource";
}

Ref<Texture> VariantResourcePreviewGenerator::generate(const Ref<Resource> &p_from, const Size2 &p_size) const {
	Ref<VariantResource> var = p_from;
	ERR_FAIL_COND_V_MSG(var.is_null(), Ref<VariantResource>(), "Invalid reference to a VariantResource object.");

    const Variant &value = var->get(var->get_property_name());

    if (value.get_type() == Variant::NIL) {
        return Ref<Texture>();
    }
	Ref<Image> image;
	image.instance();
	image->create(p_size.x, p_size.y, false, Image::FORMAT_RGBA8);

    switch (value.get_type()) {
        case Variant::COLOR: {
            Color color = value;
            image->fill(color);
        } break;
        default: {
            return Ref<Texture>();
        };
    }
	Ref<ImageTexture> tex;
    tex.instance();
	tex->create_from_image(image, 0);

	return tex;
}
