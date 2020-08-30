#include "resource_importer_animated_texture.h"

#include "core/os/file_access.h"
#include "scene/resources/texture.h"

String ResourceImporterAnimatedTexture::get_resource_type() const {
	return "AnimatedTexture";
}

String ResourceImporterAnimatedTexture::get_importer_name() const {
	return "animated_texture";
}

String ResourceImporterAnimatedTexture::get_visible_name() const {
	return "AnimatedTexture";
}

void ResourceImporterAnimatedTexture::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("gif");
}

String ResourceImporterAnimatedTexture::get_save_extension() const {
	return "atex";
}

bool ResourceImporterAnimatedTexture::get_option_visibility(const String &p_option, const Map<StringName, Variant> &p_options) const {
	return true;
}

int ResourceImporterAnimatedTexture::get_preset_count() const {
	return 0;
}

String ResourceImporterAnimatedTexture::get_preset_name(int p_idx) const {
	return "";
}

void ResourceImporterAnimatedTexture::get_import_options(List<ImportOption> *r_options, int p_preset) const {
	r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "flags/repeat", PROPERTY_HINT_ENUM, "Disabled,Enabled,Mirrored"), 0));
	r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "flags/filter"), false));
	r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "flags/mipmaps"), false));
	r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "flags/anisotropic"), false));
	r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "flags/srgb", PROPERTY_HINT_ENUM, "Disable,Enable,Detect"), 0));
	r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "max_frames", PROPERTY_HINT_RANGE, "0, 256, 1"), 0));
}

Error ResourceImporterAnimatedTexture::import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	int repeat = p_options["flags/repeat"];
	bool filter = p_options["flags/filter"];
	bool mipmaps = p_options["flags/mipmaps"];
	bool anisotropic = p_options["flags/anisotropic"];
	int srgb = p_options["flags/srgb"];
	int max_frames = p_options["max_frames"];

	int tex_flags = 0;
	if (repeat > 0)
		tex_flags |= Texture::FLAG_REPEAT;
	if (repeat == 2)
		tex_flags |= Texture::FLAG_MIRRORED_REPEAT;
	if (filter)
		tex_flags |= Texture::FLAG_FILTER;
	if (mipmaps)
		tex_flags |= Texture::FLAG_MIPMAPS;
	if (anisotropic)
		tex_flags |= Texture::FLAG_ANISOTROPIC_FILTER;
	if (srgb == 1)
		tex_flags |= Texture::FLAG_CONVERT_TO_LINEAR;

	FileAccess *f = FileAccess::open(p_source_file, FileAccess::READ);
	ERR_FAIL_COND_V(!f, ERR_CANT_OPEN);
	size_t len = f->get_len();

	Vector<uint8_t> data;
	data.resize(len);
	f->get_buffer(data.ptrw(), len);
	f->close();
	memdelete(f);

	f = FileAccess::open(p_save_path + ".atex", FileAccess::WRITE);
	ERR_FAIL_COND_V(!f, ERR_CANT_OPEN);

	const uint8_t header[6] = { 'G', 'D', 'A', 'T', 'E', 'X' };
	f->store_buffer(header, 6);
	f->store_32(tex_flags);
	f->store_32(max_frames);
	f->store_buffer(data.ptr(), len);
	f->close();
	memdelete(f);

	return OK;
}
