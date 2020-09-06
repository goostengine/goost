#include "image_frames_loader.h"

Vector<ImageFramesFormatLoader *> ResourceFormatLoaderImageFrames::loaders;

void ResourceFormatLoaderImageFrames::add_image_frames_format_loader(ImageFramesFormatLoader *p_loader) {
	loaders.push_back(p_loader);
}

void ResourceFormatLoaderImageFrames::remove_image_frames_format_loader(ImageFramesFormatLoader *p_loader) {
	loaders.erase(p_loader);
}

RES ResourceFormatLoaderImageFrames::load(const String &p_path, const String &p_original_path, Error *r_error) {
	RES result;
	for (int i = 0; i < loaders.size(); i++) {
		if (loaders[i]->recognize(p_path.get_extension())) {
			result = loaders[i]->load(p_path, p_original_path, r_error);
			break;
		}
	}
	if (r_error && *r_error != OK) {
		return RES();
	}
	return result;
}

void ResourceFormatLoaderImageFrames::get_recognized_extensions(List<String> *p_extensions) const {
	for (int i = 0; i < loaders.size(); i++) {
		loaders[i]->get_recognized_extensions(p_extensions);
	}
}

bool ImageFramesFormatLoader::recognize(String p_extension) {
	List<String> extensions;
	get_recognized_extensions(&extensions);
	for (List<String>::Element *E = extensions.front(); E; E = E->next()) {
		if (p_extension == E->get()) {
			return true;
		}
	}
	return false;
}

String ResourceFormatLoaderImageFrames::get_resource_type(const String &p_path) const {
	String ext = p_path.get_extension();
	List<String> extensions;
	get_recognized_extensions(&extensions);
	for (List<String>::Element *E = extensions.front(); E; E = E->next()) {
		if (E->get().nocasecmp_to(ext) == 0) {
			return "ImageFrames";
		}
	}
	return "";
}

bool ResourceFormatLoaderImageFrames::handles_type(const String &p_type) const {
	return p_type == "AnimatedTexture";
}
