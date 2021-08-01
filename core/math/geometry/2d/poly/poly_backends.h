#pragma once

#include "core/project_settings.h"

#include "boolean/poly_boolean.h"
#include "decomp/poly_decomp.h"
#include "offset/poly_offset.h"

#include "boolean/clipper10/poly_boolean_clipper10.h"
#include "boolean/clipper6/poly_boolean_clipper6.h"
#include "decomp/clipper10/poly_decomp_clipper10.h"
#include "decomp/polypartition/poly_decomp_polypartition.h"
#include "offset/clipper10/poly_offset_clipper10.h"
#include "offset/clipper6/poly_offset_clipper6.h"

template <class T>
class PolyBackend2DManager {
	struct Backend {
		String name = "";
		T instance = nullptr;

		Backend() :
				name(""),
				instance(nullptr) {}

		Backend(String p_name, T p_instance) :
				name(p_name),
				instance(p_instance) {}
	};

	Vector<Backend> backends;
	int default_backend_id = -1;
	String default_backend_name;

public:
	String setting_name;

public:
	void register_backend(const String &p_name, T p_instance, bool p_default = false) {
		backends.push_back(Backend(p_name, p_instance));
		if (p_default) {
			set_default_backend(p_name);
		}
	}
	void set_default_backend(const String &p_name) {
		default_backend_name = p_name;
		default_backend_id = find_backend_id(p_name);
	}

	T get_default_backend_instance() const {
		if (default_backend_id != -1) {
			return backends[default_backend_id].instance;
		}
		return nullptr;
	}

	int find_backend_id(const String &p_name) const {
		for (int i = 0; i < backends.size(); ++i) {
			if (p_name == backends[i].name) {
				return i;
			}
		}
		return -1;
	}

	T get_backend_instance(const String &p_name) const {
		int id = find_backend_id(p_name);
		if (id != -1) {
			return backends[id].instance;
		}
		return nullptr;
	}

	int get_backends_count() const { return backends.size(); }

	String get_backend_name(int p_id) const {
		CRASH_BAD_INDEX(p_id, backends.size());
		return backends[p_id].name;
	}

	String update() {
		String backends_list;

		for (int i = 0; i < get_backends_count(); ++i) {
			backends_list += get_backend_name(i);
			if (i < get_backends_count() - 1) {
				backends_list += ",";
			}
		}
		T default_backend = get_default_backend_instance();
		if (default_backend) {
			// Suggest restart because the singleton can also be used in extension mode.
			GLOBAL_DEF_RST(setting_name, default_backend_name);
		}
		if (!backends_list.empty()) {
			ProjectSettings::get_singleton()->set_custom_property_info(
					setting_name,
					PropertyInfo(Variant::STRING, setting_name, PROPERTY_HINT_ENUM, backends_list));
		}
		return ProjectSettings::get_singleton()->has_setting(setting_name) ? GLOBAL_GET(setting_name) : "";
	}

	void finalize() {
		for (int i = 0; i < backends.size(); ++i) {
			if (backends[i].instance) {
				memdelete(backends[i].instance);
			}
		}
	}
};

class PolyBackends2D {
public:
	static PolyBackend2DManager<PolyBoolean2DBackend *> poly_boolean;
	static PolyBackend2DManager<PolyOffset2DBackend *> poly_offset;
	static PolyBackend2DManager<PolyDecomp2DBackend *> poly_decomp;

	static void initialize() {
		poly_boolean.setting_name = "goost/geometry/2d/backends/poly_boolean";
		poly_boolean.register_backend("clipper6", memnew(PolyBoolean2DClipper6), true);
		poly_boolean.register_backend("clipper10", memnew(PolyBoolean2DClipper10));

		poly_offset.setting_name = "goost/geometry/2d/backends/poly_offset";
		poly_offset.register_backend("clipper6", memnew(PolyOffset2DClipper6), true);
		poly_offset.register_backend("clipper10", memnew(PolyOffset2DClipper10));

		poly_decomp.setting_name = "goost/geometry/2d/backends/poly_decomp";
		poly_decomp.register_backend("polypartition", memnew(PolyDecomp2DPolyPartition));
		poly_decomp.register_backend("clipper10:polypartition", memnew(PolyDecomp2DClipper10), true);
		
		update();
	}

	static void update() {
		String selected;
		selected = poly_boolean.update();
		PolyBoolean2D::set_backend(poly_boolean.get_backend_instance(selected));
		selected = poly_offset.update();
		PolyOffset2D::set_backend(poly_offset.get_backend_instance(selected));
		selected = poly_decomp.update();
		PolyDecomp2D::set_backend(poly_decomp.get_backend_instance(selected));
	}
	static void finalize() {
		poly_boolean.finalize();
		poly_offset.finalize();
		poly_decomp.finalize();
	}
};

