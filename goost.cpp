#include "goost.h"
#include "core/color_names.inc"

GoostEngine *GoostEngine::singleton = nullptr;

Dictionary GoostEngine::get_color_constants() const {
	if (_named_colors.empty()) {
		_populate_named_colors(); // color_names.inc
	}
    Dictionary colors;
	for (Map<String, Color>::Element *E = _named_colors.front(); E; E = E->next()) {
		colors[E->key()] = E->get();
	}
	return colors;
}

void GoostEngine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_color_constants"), &GoostEngine::get_color_constants);
}
