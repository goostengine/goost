#include "variant_resource_preview.h"
#include "editor/editor_node.h"

bool VariantResourcePreviewGenerator::handles(const String &p_type) const {
	return p_type == "VariantResource";
}

Ref<Texture> VariantResourcePreviewGenerator::generate(const Ref<Resource> &p_from, const Size2 &p_size) const {
	Ref<VariantResource> var = p_from;
	ERR_FAIL_COND_V_MSG(var.is_null(), Ref<VariantResource>(), "Invalid reference to a VariantResource object.");

    const Variant &value = var->get_value();

	Ref<Image> image;
	image.instance();
	image->create(p_size.x, p_size.y, false, Image::FORMAT_RGBA8);

    switch (value.get_type()) {
        case Variant::NIL: {
            return EditorNode::get_singleton()->get_class_icon("Variant");
        } break;
        case Variant::COLOR: {
            Color color = value;
            image->fill(color);
        } break;
        default: {
            String name = Variant::get_type_name(value.get_type());
            return EditorNode::get_singleton()->get_class_icon(name, "Resource");
        };
    }
	Ref<ImageTexture> tex;
    tex.instance();
	tex->create_from_image(image, 0);

	return tex;
}
