#include "gradient_texture_2d.h"

#include "core/core_string_names.h"
#include "core/math/geometry.h"

GradientTexture2D::GradientTexture2D() {
	texture = VS::get_singleton()->texture_create();
	gradient = Ref<Gradient>(memnew(Gradient));
	set_gradient(gradient);
}

GradientTexture2D::~GradientTexture2D() {
	if (texture.is_valid()) {
		VS::get_singleton()->free(texture);
	}
}

void GradientTexture2D::set_gradient(Ref<Gradient> p_gradient) {
	if (gradient.is_valid()) {
		if (gradient->is_connected(CoreStringNames::get_singleton()->changed, this, "_queue_update")) {
			gradient->disconnect(CoreStringNames::get_singleton()->changed, this, "_queue_update");
		}
	}
	gradient = p_gradient;
	if (gradient.is_valid()) {
		gradient->connect(CoreStringNames::get_singleton()->changed, this, "_queue_update");
	}
	_queue_update();
}

void GradientTexture2D::_queue_update() {
	if (update_pending) {
		return;
	}
	update_pending = true;
	call_deferred("_update");
}

void GradientTexture2D::_update() {
	update_pending = false;

	if (gradient.is_null()) {
		return;
	}
	Ref<Image> image;
	image.instance();

	if (gradient->get_points_count() <= 1) { // No need to interpolate.
		image->create(width, height, false, Image::FORMAT_RGBA8);
		Color color;
		if (gradient->get_points_count() == 1) {
			color = gradient->get_color(0);
		} else {
			color = Color(0, 0, 0, 1);
		}
		image->fill(color);
	} else {
		PoolVector<uint8_t> data;
		data.resize(width * height * 4);
		{
			PoolVector<uint8_t>::Write wd8 = data.write();
			Gradient &g = **gradient;
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					float ofs = _get_gradient_offset_at(x, y);
					const Color &c = g.get_color_at_offset(ofs);
					wd8[(x + (y * width)) * 4 + 0] = uint8_t(CLAMP(c.r * 255.0, 0, 255));
					wd8[(x + (y * width)) * 4 + 1] = uint8_t(CLAMP(c.g * 255.0, 0, 255));
					wd8[(x + (y * width)) * 4 + 2] = uint8_t(CLAMP(c.b * 255.0, 0, 255));
					wd8[(x + (y * width)) * 4 + 3] = uint8_t(CLAMP(c.a * 255.0, 0, 255));
				}
			}
		}
		image->create(width, height, false, Image::FORMAT_RGBA8, data);
	}
	VS::get_singleton()->texture_allocate(texture, width, height, 0, Image::FORMAT_RGBA8, VS::TEXTURE_TYPE_2D, VS::TEXTURE_FLAG_FILTER);
	VS::get_singleton()->texture_set_data(texture, image);

	emit_changed();
}

float GradientTexture2D::_get_gradient_offset_at(int x, int y) const {
	if (fill_to == fill_from) {
		return 0;
	}
	float ofs = 0;
	Vector2 pos;
	if (width > 1) {
		pos.x = static_cast<float>(x) / (width - 1);
	}
	if (height > 1) {
		pos.y = static_cast<float>(y) / (height - 1);
	}
	if (fill == Fill::FILL_LINEAR) {
		Vector2 segment[2];
		segment[0] = fill_from;
		segment[1] = fill_to;
		Vector2 closest = Geometry::get_closest_point_to_segment_uncapped_2d(pos, &segment[0]);
		ofs = (closest - fill_from).length() / (fill_to - fill_from).length();
		if ((closest - fill_from).dot(fill_to - fill_from) < 0) {
			ofs *= -1;
		}
	} else if (fill == Fill::FILL_RADIAL) {
		ofs = (pos - fill_from).length() / (fill_to - fill_from).length();
	}
	if (repeat == Repeat::REPEAT_NONE) {
		ofs = CLAMP(ofs, 0.0, 1.0);
	} else if (repeat == Repeat::REPEAT) {
		ofs = Math::fmod(ofs, 1.0f);
		if (ofs < 0) {
			ofs = 1 + ofs;
		}
	} else if (repeat == Repeat::REPEAT_MIRROR) {
		ofs = Math::abs(ofs);
		ofs = Math::fmod(ofs, 2.0f);
		if (ofs > 1.0) {
			ofs = 2.0 - ofs;
		}
	}
	return ofs;
}

void GradientTexture2D::set_width(int p_width) {
	width = p_width;
	_queue_update();
}

void GradientTexture2D::set_height(int p_height) {
	height = p_height;
	_queue_update();
}
void GradientTexture2D::set_fill_from(Vector2 p_fill_from) {
	fill_from = p_fill_from;
	_queue_update();
}

void GradientTexture2D::set_fill_to(Vector2 p_fill_to) {
	fill_to = p_fill_to;
	_queue_update();
}

void GradientTexture2D::set_fill(Fill p_fill) {
	fill = p_fill;
	_queue_update();
}

void GradientTexture2D::set_repeat(Repeat p_repeat) {
	repeat = p_repeat;
	_queue_update();
}

Ref<Image> GradientTexture2D::get_data() const {
	if (!texture.is_valid()) {
		return Ref<Image>();
	}
	return VS::get_singleton()->texture_get_data(texture);
}

void GradientTexture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_gradient", "gradient"), &GradientTexture2D::set_gradient);
	ClassDB::bind_method(D_METHOD("get_gradient"), &GradientTexture2D::get_gradient);

	ClassDB::bind_method(D_METHOD("set_width", "width"), &GradientTexture2D::set_width);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &GradientTexture2D::set_height);

	ClassDB::bind_method(D_METHOD("set_fill", "fill"), &GradientTexture2D::set_fill);
	ClassDB::bind_method(D_METHOD("get_fill"), &GradientTexture2D::get_fill);
	ClassDB::bind_method(D_METHOD("set_fill_from", "fill_from"), &GradientTexture2D::set_fill_from);
	ClassDB::bind_method(D_METHOD("get_fill_from"), &GradientTexture2D::get_fill_from);
	ClassDB::bind_method(D_METHOD("set_fill_to", "fill_to"), &GradientTexture2D::set_fill_to);
	ClassDB::bind_method(D_METHOD("get_fill_to"), &GradientTexture2D::get_fill_to);

	ClassDB::bind_method(D_METHOD("set_repeat", "repeat"), &GradientTexture2D::set_repeat);
	ClassDB::bind_method(D_METHOD("get_repeat"), &GradientTexture2D::get_repeat);

	ClassDB::bind_method(D_METHOD("_update"), &GradientTexture2D::_update);
	ClassDB::bind_method(D_METHOD("_queue_update"), &GradientTexture2D::_queue_update);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "gradient", PROPERTY_HINT_RESOURCE_TYPE, "Gradient"), "set_gradient", "get_gradient");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,2048,1,or_greater"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,2048,1,or_greater"), "set_height", "get_height");

	ADD_GROUP("Fill", "fill_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fill", PROPERTY_HINT_ENUM, "Linear,Radial"), "set_fill", "get_fill");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "fill_from"), "set_fill_from", "get_fill_from");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "fill_to"), "set_fill_to", "get_fill_to");

	ADD_GROUP("Repeat", "repeat_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "repeat", PROPERTY_HINT_ENUM, "No Repeat,Repeat,Mirror Repeat"), "set_repeat", "get_repeat");

	BIND_ENUM_CONSTANT(FILL_LINEAR);
	BIND_ENUM_CONSTANT(FILL_RADIAL);

	BIND_ENUM_CONSTANT(REPEAT_NONE);
	BIND_ENUM_CONSTANT(REPEAT);
	BIND_ENUM_CONSTANT(REPEAT_MIRROR);
}
