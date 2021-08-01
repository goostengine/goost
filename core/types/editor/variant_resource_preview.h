#pragma once

#include "editor/editor_resource_preview.h"
#include "../variant_resource.h"

class VariantResourcePreviewGenerator : public EditorResourcePreviewGenerator {
	GDCLASS(VariantResourcePreviewGenerator, EditorResourcePreviewGenerator);

public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;
    
	virtual bool generate_small_preview_automatically() const { return true; };
	virtual bool can_generate_small_preview() const { return true; };
};

