#include "image_frames.h"

#include "goost/classes_enabled.gen.h"

#include "core/image/image_indexed.h"
#include "core/os/file_access.h"

#include <gif_lib.h>

LoadImageFramesFunction ImageFrames::load_gif_func = nullptr;

Error ImageFrames::load(const String &p_path, int max_frames) {
	clear();
	String ext = p_path.get_extension().to_lower();
	if (ext == "gif") {
		Ref<ImageFrames> image_frames = Ref<ImageFrames>(this);
		return load_gif_func(image_frames, p_path, max_frames);
	} else {
		ERR_PRINT("Unrecognized image: " + p_path);
		return ERR_FILE_UNRECOGNIZED;
	}
}

Error ImageFrames::load_gif_from_buffer(const PoolByteArray &p_data, int max_frames) {
	clear();
	if (p_data[0] == 'G') {
		Ref<ImageFrames> image_frames = Ref<ImageFrames>(this);
		return load_gif_func(image_frames, p_data, max_frames);
	} else {
		ERR_PRINT("Unrecognized image.");
		return ERR_FILE_UNRECOGNIZED;
	}
}

#ifdef GOOST_ImageIndexed

static int save_gif_func(GifFileType *gif, const GifByteType *data, int length) {
	// gif->UserData is the first parameter passed to EGifOpen.
	FileAccess *f = (FileAccess *)(gif->UserData); 
	f->store_buffer((const uint8_t*)data, length);
	return length;
}

Error ImageFrames::save_gif(const String &p_filepath) {
	ERR_FAIL_COND_V_MSG(get_frame_count() == 0, ERR_CANT_CREATE,
			"ImageFrames must have at least one frame added.");

	int error;

	FileAccess *f = FileAccess::open(p_filepath, FileAccess::WRITE);
	ERR_FAIL_COND_V(!f, ERR_CANT_OPEN);

	GifFileType *gif = EGifOpen(f, save_gif_func, &error);
	if (!gif) {
		ERR_PRINT(vformat("EGifOpen() failed - %s", error));
		return ERR_CANT_CREATE;
	}

	// Using dimensions of the first image as the base.
	const Ref<Image> &first = get_frame_image(0);

	gif->SWidth = first->get_width();
	gif->SHeight = first->get_height();
	gif->SColorResolution = 8;
	gif->SBackGroundColor = 0;
	gif->SColorMap = nullptr; // No global color map, using local.

	if (EGifPutScreenDesc(gif,
				gif->SWidth,
				gif->SHeight,
				gif->SColorResolution,
				gif->SBackGroundColor,
				gif->SColorMap) == GIF_ERROR) {
		return ERR_CANT_CREATE;
	}

	for (int i = 0; i < get_frame_count(); ++i) {
		Ref<Image> frame = get_frame_image(i)->duplicate();
		frame->convert(Image::FORMAT_RGBA8);
		const float delay = get_frame_delay(i); // Seconds.

		// Generate color map.
		Ref<ImageIndexed> indexed = frame;
		if (indexed.is_null()) {
			indexed.instance();
			indexed->create(frame->get_width(), frame->get_height(), false, Image::FORMAT_RGBA8, frame->get_data());
		}
		indexed->generate_palette(256, ImageIndexed::DITHER_ORDERED, false, true);

		PoolVector<uint8_t> color_map = indexed->get_palette_data();
		ColorMapObject *cmap = nullptr;
		{
			PoolVector<uint8_t>::Read r = color_map.read();
			GifColorType *gif_colors = (GifColorType *)malloc(sizeof(GifColorType) * indexed->get_palette_size());
			for (int j = 0; j < indexed->get_palette_size(); ++j) {
				gif_colors[j].Red = r[j * 4 + 0];
				gif_colors[j].Green = r[j * 4 + 1];
				gif_colors[j].Blue = r[j * 4 + 2];
			}
			cmap = GifMakeMapObject(indexed->get_palette_size(), gif_colors);
		}
		// Create raster.
		PoolVector<uint8_t> index_data = indexed->get_index_data();
		GifByteType *raster = nullptr;
		{
			PoolVector<uint8_t>::Read r = index_data.read();
			raster = (GifByteType *)malloc(index_data.size());
			memcpy(raster, r.ptr(), index_data.size());
		}
		// Add delay.
		if (get_frame_count() > 1) {
			EGifPutExtensionLeader(gif, GRAPHICS_EXT_FUNC_CODE);
			static unsigned char gfx_ext_data[4] = {
				0x04,
				CLAMP(static_cast<uint8_t>(100 * delay), 0u, 255u) % 0xFF,
				CLAMP(static_cast<uint8_t>(100 * delay), 0u, 255u) / 0xFF,
				0x00
			};
			EGifPutExtensionBlock(gif, 4, gfx_ext_data);
			EGifPutExtensionTrailer(gif);
		}

		// Write!
		if (EGifPutImageDesc(gif, 0, 0, frame->get_width(), frame->get_height(), false, cmap) == GIF_ERROR) {
			return ERR_CANT_CREATE;
		}

		for (int j = 0; j < frame->get_height(); j++) {
			if (EGifPutLine(gif, raster + j * frame->get_width(), frame->get_width()) == GIF_ERROR) {
				return ERR_CANT_CREATE;
			}
		}
	}

	// Add loop.
	if (get_frame_count() > 1) {
		EGifPutExtensionLeader(gif, APPLICATION_EXT_FUNC_CODE);
		char ns[12] = "NETSCAPE2.0";
		EGifPutExtensionBlock(gif, 11, ns);
		unsigned char sub[3] = {
			0x01,
			0x00, // Infinite.
			0x00,
		};
		EGifPutExtensionBlock(gif, 3, sub);
		EGifPutExtensionTrailer(gif);
	}

	EGifCloseFile(gif, &error);

	return OK;
}
#endif // GOOST_ImageIndexed

void ImageFrames::add_frame(const Ref<Image> &p_image, float p_delay, int p_idx) {
	ERR_FAIL_COND(p_idx > get_frame_count() - 1);
	Frame frame;
	frame.image = p_image;
	frame.delay = p_delay;
	if (p_idx < 0) {
		frames.push_back(frame);
	} else {
		frames.set(p_idx, frame);
	}
}

void ImageFrames::remove_frame(int p_idx) {
	ERR_FAIL_COND(p_idx < 0);
	ERR_FAIL_COND(p_idx > get_frame_count() - 1);
	frames.remove(p_idx);
}

void ImageFrames::set_frame_image(int p_idx, const Ref<Image> &p_image) {
	ERR_FAIL_COND(p_idx < 0);
	ERR_FAIL_COND(p_idx > get_frame_count() - 1);
	frames.write[p_idx].image = p_image;
}

Ref<Image> ImageFrames::get_frame_image(int p_idx) const {
	ERR_FAIL_COND_V(p_idx < 0, Ref<Image>());
	ERR_FAIL_COND_V(p_idx > get_frame_count() - 1, Ref<Image>());
	return frames[p_idx].image;
}

void ImageFrames::set_frame_delay(int p_idx, float p_delay) {
	ERR_FAIL_COND(p_idx < 0);
	ERR_FAIL_COND(p_idx > get_frame_count() - 1);
	frames.write[p_idx].delay = p_delay;
}

float ImageFrames::get_frame_delay(int p_idx) const {
	ERR_FAIL_COND_V(p_idx < 0, 0);
	ERR_FAIL_COND_V(p_idx > get_frame_count() - 1, 0);
	return frames[p_idx].delay;
}

int ImageFrames::get_frame_count() const {
	return frames.size();
}

void ImageFrames::clear() {
	frames.clear();
}

void ImageFrames::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load", "path", "max_frames"), &ImageFrames::load, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("load_gif_from_buffer", "data", "max_frames"), &ImageFrames::load_gif_from_buffer, DEFVAL(0));
#ifdef GOOST_ImageIndexed
	ClassDB::bind_method(D_METHOD("save_gif", "filepath"), &ImageFrames::save_gif);
#endif
	ClassDB::bind_method(D_METHOD("add_frame", "image", "delay", "idx"), &ImageFrames::add_frame, DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("remove_frame", "idx"), &ImageFrames::remove_frame);

	ClassDB::bind_method(D_METHOD("set_frame_image", "idx", "image"), &ImageFrames::set_frame_image);
	ClassDB::bind_method(D_METHOD("get_frame_image", "idx"), &ImageFrames::get_frame_image);

	ClassDB::bind_method(D_METHOD("set_frame_delay", "idx", "delay"), &ImageFrames::set_frame_delay);
	ClassDB::bind_method(D_METHOD("get_frame_delay", "idx"), &ImageFrames::get_frame_delay);

	ClassDB::bind_method(D_METHOD("get_frame_count"), &ImageFrames::get_frame_count);

	ClassDB::bind_method(D_METHOD("clear"), &ImageFrames::clear);
}
