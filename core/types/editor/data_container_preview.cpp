#include "data_container_preview.h"
#include "editor/editor_node.h"

bool DataContainerPreviewGenerator::handles(const String &p_type) const {
	return p_type == "DataContainer";
}

Ref<Texture> DataContainerPreviewGenerator::generate(const Ref<Resource> &p_from, const Size2 &p_size) const {
	Ref<DataContainer> container = p_from;
	ERR_FAIL_COND_V_MSG(container.is_null(), Ref<DataContainer>(), "Invalid reference to a DataContainer object.");

	const Variant &value = container->get_value();

	Ref<Texture> tex;

	switch (value.get_type()) {
		case Variant::NIL: {
			tex = EditorNode::get_singleton()->get_class_icon("Variant");
		} break;
		case Variant::BOOL: {
			bool b = value;
			String builtin_name = Variant::get_type_name(value.get_type());
			String name = b ? "GuiChecked" : "GuiUnchecked";
			tex = EditorNode::get_singleton()->get_class_icon(name, builtin_name);
		} break;
		case Variant::COLOR: {
			Color color = value;
			Ref<Image> image;
			image.instance();
			// The icon is stretched in grid mode so it's possible to speed up
			// generation in those cases, but won't work in tree mode (default).
			// Therefore, we must still use `p_size` for the preview...
			image->create(p_size.x, p_size.y, false, Image::FORMAT_RGBA8);
			image->fill(color);
			Ref<ImageTexture> img_tex;
			img_tex.instance();
			img_tex->create_from_image(image, 0);
			tex = img_tex;
		} break;
		case Variant::OBJECT: {
			Object *obj = value;
			Ref<Texture> obj_tex = Ref<Texture>(Object::cast_to<Texture>(obj));
			if (obj_tex.is_valid()) {
				Ref<Image> image = obj_tex->get_data();
				image->resize(p_size.x, p_size.y);
				Ref<ImageTexture> img_tex;
				img_tex.instance();
				img_tex->create_from_image(image, 0);
				tex = img_tex;
			}
		} break;
		default: {
			String name = Variant::get_type_name(value.get_type());
			tex = EditorNode::get_singleton()->get_class_icon(name, "Resource");
		};
	}
	return tex;
}
