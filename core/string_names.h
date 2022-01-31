#pragma once

#include "core/string_name.h"

namespace goost {
    void register_core_types();
	void unregister_core_types();
}

class StringNames {
	friend void goost::register_core_types();
	friend void goost::unregister_core_types();

	static void create() { singleton = memnew(StringNames); }
	static void free() {
		memdelete(singleton);
		singleton = nullptr;
	}
	StringNames();

public:
	_FORCE_INLINE_ static StringNames *get_singleton() { return singleton; }
	static StringNames *singleton;

    StringName _create_vertex;
    StringName _create_edge;
};
