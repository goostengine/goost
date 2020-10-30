#ifndef GOOST_VARIANT_RESOURCE_H
#define GOOST_VARIANT_RESOURCE_H

#include "core/resource.h"
#include "core/variant.h"

class VariantResource : public Resource {
	GDCLASS(VariantResource, Resource);

protected:
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	static void _bind_methods();

private:
	Variant::Type type = Variant::NIL;
	Variant value;

public:
	void set_type(Variant::Type p_type);
	int get_type() const { return type; }

	static Variant create(const Variant::Type &p_type);
	static Variant convert(const Variant &p_value, const Variant::Type &p_to_type);

	virtual String to_string() { return String(value); }
};

#endif // GOOST_VARIANT_RESOURCE_H
