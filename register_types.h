void register_goost_types();
void unregister_goost_types();

// Register a class only if enabled.
//
// Use this macro over `ClassDB::register_class` in Goost,
// unless the process of registering the class involes more complex steps
// such as instantiating a singleton of the class, or adding editor plugins.
// If that's the case, you'll have to check whether coresponding
// `GOOST_CLASS_*_ENABLED` macros are defined manually in code which are
// generated in "classes_enabled.gen.h" file included above.
//
#ifndef GOOST_REGISTER_CLASS
#define GOOST_REGISTER_CLASS(m_name)                         \
	for (int i = 0; i < _goost_classes_enabled_count; ++i) { \
		String name = _goost_classes_enabled[i];             \
		if (name == #m_name) {                               \
			ClassDB::register_class<m_name>();               \
			break;                                           \
		}                                                    \
	}
#endif
