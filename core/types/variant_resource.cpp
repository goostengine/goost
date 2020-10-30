#include "variant_resource.h"

void VariantResource::set_type(Variant::Type p_type) {
	const Variant::Type prev_type = type;
	type = p_type;
	// Convert previous value to a new type, if possible.
	if (prev_type != Variant::NIL) {
		value = convert(value, type);
	} else {
		value = create(type);
	}
	emit_changed();
	_change_notify();
}

Variant VariantResource::create(const Variant::Type &p_type) {
	Variant::CallError error;
	return Variant::construct(p_type, nullptr, 0, error);
}

Variant VariantResource::convert(const Variant &p_value, const Variant::Type &p_to_type) {
	Variant::CallError error;
	const Variant *args[1];
	args[0] = &p_value;
	return Variant::construct(p_to_type, args, 1, error, false); // Non-strict.
}

bool VariantResource::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name.operator String();
	if (name == "value") {
		value = p_value;
		type = p_value.get_type();
		emit_changed();
	} else {
		return false;
	}
	return true;
}

bool VariantResource::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name.operator String();
	if (name == "value") {
		r_ret = value;
	} else {
		return false;
	}
	return true;
}

void VariantResource::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(type, "value"));
}

void VariantResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_type", "type"), &VariantResource::set_type);
	ClassDB::bind_method(D_METHOD("get_type"), &VariantResource::get_type);

	String hint_types;
	for (int i = 0; i < Variant::VARIANT_MAX; ++i) {
		hint_types += Variant::get_type_name(Variant::Type(i));
		if (i < Variant::VARIANT_MAX - 1) {
			hint_types += ",";
		}
	}
	ADD_PROPERTY(PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM, hint_types), "set_type", "get_type");
}
