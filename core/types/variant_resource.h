#pragma once

#include "core/resource.h"
#include "core/variant.h"

class VariantResource : public Resource {
	GDCLASS(VariantResource, Resource);

protected:
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	static void _bind_methods();

	Variant value;
	Variant::Type type = Variant::NIL;
	PropertyInfo pi;

public:
	void set_type(int p_type);
	int get_type() const { return type; }

	void set_value(const Variant &p_value) { set(pi.name, p_value); }
	Variant get_value() const { return get(pi.name); }

	void set_property_name(const String &p_property_name);
	String get_property_name() const { return pi.name; };

	void set_property_hint(PropertyHint p_property_hint);
	PropertyHint get_property_hint() const { return pi.hint; };

	void set_property_hint_string(const String &p_property_hint_string);
	String get_property_hint_string() const { return pi.hint_string; };
	
	void set_property_usage(PropertyUsageFlags p_property_usage);
	PropertyUsageFlags get_property_usage() const { return PropertyUsageFlags(pi.usage); };

	static Variant create(const Variant::Type &p_type);
	static Variant convert(const Variant &p_value, const Variant::Type &p_to_type);

	static String get_type_hints();
	// This should be in Godot, just like `Variant::get_type_name()` method.
	static String get_property_hint_name(const PropertyHint &p_hint);
	static String get_property_hint_types();

	virtual String to_string() { return String(value); }

	VariantResource() {
		// Default, but can be configured with `property_name` property.
		pi.name = "value"; 
	}
};

