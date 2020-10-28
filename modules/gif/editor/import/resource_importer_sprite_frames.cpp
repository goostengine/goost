#include "resource_importer_sprite_frames.h"

#include "image_frames.h"
#include "image_frames_loader.h"

#include "core/os/file_access.h"
#include "scene/resources/texture.h"

String ResourceImporterSpriteFrames::get_resource_type() const {
	return "SpriteFrames";
}

String ResourceImporterSpriteFrames::get_importer_name() const {
	return "sprite_frames";
}

String ResourceImporterSpriteFrames::get_visible_name() const {
	return "SpriteFrames";
}

void ResourceImporterSpriteFrames::get_recognized_extensions(List<String> *p_extensions) const {
	ImageFramesLoader::get_recognized_extensions(p_extensions);
}

String ResourceImporterSpriteFrames::get_save_extension() const {
	return "sframes";
}

bool ResourceImporterSpriteFrames::get_option_visibility(const String &p_option, const Map<StringName, Variant> &p_options) const {
	return true;
}

int ResourceImporterSpriteFrames::get_preset_count() const {
	return 0;
}

String ResourceImporterSpriteFrames::get_preset_name(int p_idx) const {
	return "";
}

void ResourceImporterSpriteFrames::get_import_options(List<ImportOption> *r_options, int p_preset) const {
	r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "flags/repeat", PROPERTY_HINT_ENUM, "Disabled,Enabled,Mirrored"), 0));
	r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "flags/filter"), false));
	r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "flags/mipmaps"), false));
	r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "flags/anisotropic"), false));
	r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "flags/srgb", PROPERTY_HINT_ENUM, "Disable,Enable,Detect"), 0));
	r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "max_frames", PROPERTY_HINT_RANGE, "0, 4096, 1"), 0));
}

Error ResourceImporterSpriteFrames::import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	int max_frames = p_options["max_frames"];
	int repeat = p_options["flags/repeat"];
	bool filter = p_options["flags/filter"];
	bool mipmaps = p_options["flags/mipmaps"];
	bool anisotropic = p_options["flags/anisotropic"];
	int srgb = p_options["flags/srgb"];

	int tex_flags = 0;
	if (repeat > 0) {
		tex_flags |= Texture::FLAG_REPEAT;
	}
	if (repeat == 2) {
		tex_flags |= Texture::FLAG_MIRRORED_REPEAT;
	}
	if (filter) {
		tex_flags |= Texture::FLAG_FILTER;
	}
	if (mipmaps) {
		tex_flags |= Texture::FLAG_MIPMAPS;
	}
	if (anisotropic) {
		tex_flags |= Texture::FLAG_ANISOTROPIC_FILTER;
	}
	if (srgb == 1) {
		tex_flags |= Texture::FLAG_CONVERT_TO_LINEAR;
	}

	Ref<ImageFrames> image_frames;
	image_frames.instance();
	Error err = ImageFramesLoader::load_image_frames(p_source_file, image_frames, nullptr, max_frames);
	if (err != OK) {
		return err;
	}
	if (max_frames <= 0 || max_frames > 4096) {
		max_frames = 4096;
	}
	const int frame_count = MIN(image_frames->get_frame_count(), max_frames);

	FileAccess *f = FileAccess::open(p_save_path + ".sframes", FileAccess::WRITE);
	ERR_FAIL_COND_V(!f, ERR_CANT_OPEN);

	const uint8_t header[4] = { 'G', 'D', 'S', 'F' };
	f->store_buffer(header, 4); // GoDot Sprite Frames.
	f->store_32(tex_flags);
	f->store_32(frame_count);

	// We already assume image frames already contains at least one frame,
	// and that all frames have the same size.
	f->store_32(image_frames->get_frame_image(0)->get_width());
	f->store_32(image_frames->get_frame_image(0)->get_height());

	// Use the average time of all the frames.
	real_t total_time = 0.0;
	for (int i = 0; i < frame_count; i++) {
		total_time += image_frames->get_frame_delay(i);
	}
	real_t average_time = total_time / frame_count;

	for (int i = 0; i < frame_count; ++i) {
		// Frame image data.
		PoolVector<uint8_t> data = image_frames->get_frame_image(i)->get_data();
		PoolVector<uint8_t>::Read r = data.read();
		f->store_32(data.size());
		f->store_buffer(r.ptr(), data.size());
	}
	// Animation speed.
	real_t speed = 1.0 / average_time;
	f->store_real(speed);

	f->close();
	memdelete(f);

	return OK;
}
