#include "data_container.h"

void DataContainer::set_type(int p_type) {
	const Variant::Type prev_type = type;
	type = static_cast<Variant::Type>(p_type);
	// Convert previous value to a new type, if possible.
	if (prev_type != Variant::NIL) {
		value = convert(value, type);
	} else {
		value = create(type);
	}
	emit_changed();
	_change_notify();
}

Variant DataContainer::create(const Variant::Type &p_type) {
	Variant::CallError error;
	return Variant::construct(p_type, nullptr, 0, error);
}

Variant DataContainer::convert(const Variant &p_value, const Variant::Type &p_to_type) {
	Variant::CallError error;
	const Variant *args[1];
	args[0] = &p_value;
	return Variant::construct(p_to_type, args, 1, error, false); // Non-strict.
}

bool DataContainer::_set(const StringName &p_name, const Variant &p_value) {
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

bool DataContainer::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name.operator String();
	if (name == pi.name) {
		r_ret = value;
	} else {
		return false;
	}
	return true;
}

void DataContainer::set_property_name(const String &p_property_name) {
	pi.name = p_property_name;
	_change_notify();
}

void DataContainer::set_property_hint(PropertyHint p_property_hint) {
	pi.hint = p_property_hint;
	_change_notify();
}

void DataContainer::set_property_hint_string(const String &p_property_hint_string) {
	pi.hint_string = p_property_hint_string;
	_change_notify();
}

void DataContainer::set_property_usage(PropertyUsageFlags p_property_usage) {
	pi.usage = p_property_usage;
	_change_notify();
}

void DataContainer::_get_property_list(List<PropertyInfo> *p_list) const {
	// This property is changed dynamically from other properties.
	p_list->push_back(PropertyInfo(type, pi.name, pi.hint, pi.hint_string, pi.usage));
}

String DataContainer::get_type_hints() {
	String type_hints;
	for (int i = 0; i < Variant::VARIANT_MAX; ++i) {
		type_hints += Variant::get_type_name(Variant::Type(i));
		if (i < Variant::VARIANT_MAX - 1) {
			type_hints += ",";
		}
	}
	return type_hints;
}

String DataContainer::get_property_hint_name(const PropertyHint &p_hint) {
	switch (p_hint) {
		case PROPERTY_HINT_NONE: {
			return "None";
		} break;
		case PROPERTY_HINT_RANGE: {
			return "Range";
		} break;
		case PROPERTY_HINT_EXP_RANGE: {
			return "Exponential Range";
		} break;
		case PROPERTY_HINT_ENUM: {
			return "Enum";
		} break;
		case PROPERTY_HINT_EXP_EASING: {
			return "Exponential Easing";
		} break;
		case PROPERTY_HINT_LENGTH: {
			return "Length";
		} break;
		case PROPERTY_HINT_SPRITE_FRAME: {
			return "SpriteFrame";
		} break;
		case PROPERTY_HINT_KEY_ACCEL: {
			return "Key Accel";
		} break;
		case PROPERTY_HINT_FLAGS: {
			return "Flags";
		} break;
		case PROPERTY_HINT_LAYERS_2D_RENDER: {
			return "Layers 2D Render";
		} break;
		case PROPERTY_HINT_LAYERS_2D_PHYSICS: {
			return "Layers 2D Physics";
		} break;
		case PROPERTY_HINT_LAYERS_3D_RENDER: {
			return "Layers 3D Render";
		} break;
		case PROPERTY_HINT_LAYERS_3D_PHYSICS: {
			return "Layers 3D Physics";
		} break;
		case PROPERTY_HINT_FILE: {
			return "File";
		} break;
		case PROPERTY_HINT_DIR: {
			return "Directory";
		} break;
		case PROPERTY_HINT_GLOBAL_FILE: {
			return "Global File";
		} break;
		case PROPERTY_HINT_GLOBAL_DIR: {
			return "Global Directory";
		} break;
		case PROPERTY_HINT_RESOURCE_TYPE: {
			return "Resource Type";
		} break;
		case PROPERTY_HINT_MULTILINE_TEXT: {
			return "Multiline Text";
		} break;
		case PROPERTY_HINT_PLACEHOLDER_TEXT: {
			return "Placeholder Text";
		} break;
		case PROPERTY_HINT_COLOR_NO_ALPHA: {
			return "Color No Alpha";
		} break;
		case PROPERTY_HINT_IMAGE_COMPRESS_LOSSY: {
			return "Image Compress Lossy";
		} break;
		case PROPERTY_HINT_IMAGE_COMPRESS_LOSSLESS: {
			return "Image Compress Lossless";
		} break;
		case PROPERTY_HINT_OBJECT_ID: {
			return "ObjectID";
		} break;
		case PROPERTY_HINT_TYPE_STRING: {
			return "String";
		} break;
		case PROPERTY_HINT_NODE_PATH_TO_EDITED_NODE: {
			return "NodePath To Edited Node";
		} break;
		case PROPERTY_HINT_METHOD_OF_VARIANT_TYPE: {
			return "Method Of Variant Type";
		} break;
		case PROPERTY_HINT_METHOD_OF_BASE_TYPE: {
			return "Method Of Base Type";
		} break;
		case PROPERTY_HINT_METHOD_OF_INSTANCE: {
			return "Method Of Instance";
		} break;
		case PROPERTY_HINT_METHOD_OF_SCRIPT: {
			return "Method Of Script";
		} break;
		case PROPERTY_HINT_PROPERTY_OF_VARIANT_TYPE: {
			return "Property Of Variant Type";
		} break;
		case PROPERTY_HINT_PROPERTY_OF_BASE_TYPE: {
			return "Property Of Base Type";
		} break;
		case PROPERTY_HINT_PROPERTY_OF_INSTANCE: {
			return "Property Of Instance";
		} break;
		case PROPERTY_HINT_PROPERTY_OF_SCRIPT: {
			return "Property Of Script";
		} break;
		case PROPERTY_HINT_OBJECT_TOO_BIG: {
			return "Object Too Big";
		} break;
		case PROPERTY_HINT_NODE_PATH_VALID_TYPES: {
			return "NodePath Valid Types";
		} break;
		case PROPERTY_HINT_SAVE_FILE: {
			return "Save File";
		} break;
		case PROPERTY_HINT_ENUM_SUGGESTION: {
			return "Enum Suggestion";
		} break;
		case PROPERTY_HINT_MAX: {
			ERR_FAIL_V_MSG("", "Invalid property hint type.");
		} break;
	}
	return "";
}

String DataContainer::get_property_hint_types() {
	String hint_types;
	for (int i = 0; i < PropertyHint::PROPERTY_HINT_MAX; ++i) {
		hint_types += DataContainer::get_property_hint_name(PropertyHint(i));
		if (i < PropertyHint::PROPERTY_HINT_MAX - 1) {
			hint_types += ",";
		}
	}
	return hint_types;
}

void DataContainer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_type", "type"), &DataContainer::set_type);
	ClassDB::bind_method(D_METHOD("get_type"), &DataContainer::get_type);

	ClassDB::bind_method(D_METHOD("set_value", "value"), &DataContainer::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &DataContainer::get_value);

	ClassDB::bind_method(D_METHOD("set_property_name", "name"), &DataContainer::set_property_name);
	ClassDB::bind_method(D_METHOD("get_property_name"), &DataContainer::get_property_name);

	ClassDB::bind_method(D_METHOD("set_property_hint", "hint"), &DataContainer::set_property_hint);
	ClassDB::bind_method(D_METHOD("get_property_hint"), &DataContainer::get_property_hint);

	ClassDB::bind_method(D_METHOD("set_property_hint_string", "hint_string"), &DataContainer::set_property_hint_string);
	ClassDB::bind_method(D_METHOD("get_property_hint_string"), &DataContainer::get_property_hint_string);

	ClassDB::bind_method(D_METHOD("set_property_usage", "usage"), &DataContainer::set_property_usage);
	ClassDB::bind_method(D_METHOD("get_property_usage"), &DataContainer::get_property_usage);

	// DO NOT expose `value` as a property here, this is handled by `_get_property_list()` instead.

	ADD_PROPERTY(PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM, get_type_hints()), "set_type", "get_type");

	ADD_GROUP("Property", "property_");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "property_name"), "set_property_name", "get_property_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "property_hint", PROPERTY_HINT_ENUM, get_property_hint_types()), "set_property_hint", "get_property_hint");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "property_hint_string"), "set_property_hint_string", "get_property_hint_string");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "property_usage"), "set_property_usage", "get_property_usage");
}
