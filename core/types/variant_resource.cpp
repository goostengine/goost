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
	if (name == pi.name) {
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
	if (name == pi.name) {
		r_ret = value;
	} else {
		return false;
	}
	return true;
}

void VariantResource::set_property_name(const String &p_property_name) {
	pi.name = p_property_name;
	_change_notify();
}

void VariantResource::set_property_hint(PropertyHint p_property_hint) {
	pi.hint = p_property_hint;
	_change_notify();
}

void VariantResource::set_property_hint_string(const String &p_property_hint_string) {
	pi.hint_string = p_property_hint_string;
	_change_notify();
}

void VariantResource::set_property_usage(PropertyUsageFlags p_property_usage) {
	pi.usage = p_property_usage;
	_change_notify();
}

void VariantResource::_get_property_list(List<PropertyInfo> *p_list) const {
	// This property is changed dynamically from other properties.
	p_list->push_back(PropertyInfo(type, pi.name, pi.hint, pi.hint_string, pi.usage));
}

String VariantResource::get_type_hints() {
	String type_hints;
	for (int i = 0; i < Variant::VARIANT_MAX; ++i) {
		type_hints += Variant::get_type_name(Variant::Type(i));
		if (i < Variant::VARIANT_MAX - 1) {
			type_hints += ",";
		}
	}
	return type_hints;
}

void VariantResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_type", "type"), &VariantResource::set_type);
	ClassDB::bind_method(D_METHOD("get_type"), &VariantResource::get_type);

	ClassDB::bind_method(D_METHOD("set_property_name", "name"), &VariantResource::set_property_name);
	ClassDB::bind_method(D_METHOD("get_property_name"), &VariantResource::get_property_name);

	ClassDB::bind_method(D_METHOD("set_property_hint", "hint"), &VariantResource::set_property_hint);
	ClassDB::bind_method(D_METHOD("get_property_hint"), &VariantResource::get_property_hint);

	ClassDB::bind_method(D_METHOD("set_property_hint_string", "hint_string"), &VariantResource::set_property_hint_string);
	ClassDB::bind_method(D_METHOD("get_property_hint_string"), &VariantResource::get_property_hint_string);

	ClassDB::bind_method(D_METHOD("set_property_usage", "usage"), &VariantResource::set_property_usage);
	ClassDB::bind_method(D_METHOD("get_property_usage"), &VariantResource::get_property_usage);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM, get_type_hints()), "set_type", "get_type");

	ADD_GROUP("Property", "property_");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "property_name"), "set_property_name", "get_property_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "property_hint"), "set_property_hint", "get_property_hint");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "property_hint_string"), "set_property_hint_string", "get_property_hint_string");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "property_usage"), "set_property_usage", "get_property_usage");
}
