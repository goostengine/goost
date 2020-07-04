#include "register_editor_types.h"

#include "2d/visual_shape_2d_editor_plugin.h"

namespace goost {

void register_editor_types() {
#ifdef TOOLS_ENABLED
	EditorPlugins::add_by_type<VisualShape2DEditorPlugin>();
#endif
}

void unregister_editor_types() {
	// Nothing to do.
}

} // namespace goost
