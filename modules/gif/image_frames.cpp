#include "image_frames.h"

#include "core/os/file_access.h"

#include "modules/modules_enabled.gen.h"
#ifdef MODULE_GOOST_ENABLED
#include "goost/classes_enabled.gen.h"
#include "goost/core/image/image_indexed.h"
#endif

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
	ERR_FAIL_COND_V_MSG(p_data.empty(), ERR_INVALID_DATA, "Invalid GIF data");
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
	f->store_buffer((const uint8_t *)data, length);
	return length;
}

Error ImageFrames::save_gif(const String &p_filepath, int p_color_count) {
	ERR_FAIL_COND_V_MSG(get_frame_count() == 0, ERR_CANT_CREATE,
			"ImageFrames must have at least one frame added.");

	FileAccess *f = FileAccess::open(p_filepath, FileAccess::WRITE);
	ERR_FAIL_COND_V_MSG(!f, ERR_CANT_OPEN, "Error opening file.");

	int error;
	GifFileType *gif = EGifOpen(f, save_gif_func, &error);
	if (!gif) {
		memdelete(f);
		ERR_PRINT(vformat("EGifOpen() failed - %s.", error));
		return ERR_CANT_CREATE;
	}

	// Using dimensions of the first image as the base.
	const Ref<Image> &first = get_frame_image(0);
	ERR_FAIL_COND_V(first.is_null(), ERR_CANT_CREATE);

	const bool has_alpha = first->detect_alpha();

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
		const Ref<Image> &frame = get_frame_image(i);
		const float delay = get_frame_delay(i); // Seconds.

		// Prepare indexed image and generate palette.
		Ref<ImageIndexed> indexed = frame;
		if (indexed.is_null()) {
			indexed.instance();
			indexed->create(frame->get_width(), frame->get_height(), false, frame->get_format(), frame->get_data());
			indexed->convert(Image::FORMAT_RGBA8);
			int num_colors = CLAMP(p_color_count, 1, 256);
			num_colors = next_power_of_2(num_colors);
			indexed->generate_palette(num_colors, ImageIndexed::DITHER_ORDERED, has_alpha, true);
		} else {
			ERR_FAIL_COND_V_MSG(!indexed->has_palette(), ERR_CANT_CREATE,
					"Custom ImageIndexed passed to ImagesFrames must have palette already generated.");
		}

		// Generate GIF color map.
		PoolVector<uint8_t> color_map = indexed->get_palette_data();
		ColorMapObject *gif_color_map = nullptr;
		{
			PoolVector<uint8_t>::Read r = color_map.read();
			GifColorType *gif_colors = (GifColorType *)malloc(sizeof(GifColorType) * indexed->get_palette_size());
			const int pixel_size = Image::get_format_pixel_size(indexed->get_format());

			for (int j = 0; j < indexed->get_palette_size(); ++j) {
				gif_colors[j].Red = r[j * pixel_size + 0];
				gif_colors[j].Green = r[j * pixel_size + 1];
				gif_colors[j].Blue = r[j * pixel_size + 2];
			}
			gif_color_map = GifMakeMapObject(indexed->get_palette_size(), gif_colors);
			ERR_FAIL_NULL_V(gif_color_map, ERR_CANT_CREATE);

			free((GifColorType *)gif_colors);
		}

		// Add delay.
		EGifPutExtensionLeader(gif, GRAPHICS_EXT_FUNC_CODE);
		uint16_t d = 100 * delay;
		// 000: reserved; 010: disposal; 0: no user input; 1: handle transparency.
		uint8_t packed_fields = 0x08;
		if (has_alpha) {
			packed_fields |= 1;
		}
		uint8_t gfx_ext_data[4] = {
			packed_fields,
			static_cast<uint8_t>((d >> 0) & 0xff),
			static_cast<uint8_t>((d >> 8) & 0xff),
			0x00, // Transparent color index (can we always assume 0x00 to represent black color?)
		};
		EGifPutExtensionBlock(gif, 4, gfx_ext_data);
		EGifPutExtensionTrailer(gif);

		// Write!
		if (EGifPutImageDesc(gif, 0, 0, frame->get_width(), frame->get_height(), false, gif_color_map) == GIF_ERROR) {
			GifFreeMapObject(gif_color_map);
			return ERR_CANT_CREATE;
		}
		GifFreeMapObject(gif_color_map);

		PoolVector<uint8_t> index_data = indexed->get_index_data();
		PoolVector<uint8_t>::Write w = index_data.write();
		GifPixelType *raster = static_cast<GifPixelType *>(w.ptr());

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
		uint8_t sub[3] = {
			0x01,
			0x00, // Infinite.
			0x00,
		};
		EGifPutExtensionBlock(gif, 3, sub);
		EGifPutExtensionTrailer(gif);
	}
	// Write out the GIF file.
	EGifCloseFile(gif, &error);
	f->close();
	memdelete(f);

	return OK;
}
#endif // GOOST_ImageIndexed

void ImageFrames::add_frame(const Ref<Image> &p_image, float p_delay) {
	ERR_FAIL_COND(p_image.is_null());

	Frame frame;
	frame.image = p_image;
	frame.delay = p_delay;
	frames.push_back(frame);
}

void ImageFrames::remove_frame(int p_idx) {
	ERR_FAIL_INDEX(p_idx, frames.size());
	frames.remove(p_idx);
}

void ImageFrames::set_frame_image(int p_idx, const Ref<Image> &p_image) {
	ERR_FAIL_INDEX(p_idx, frames.size());
	frames.write[p_idx].image = p_image;
}

Ref<Image> ImageFrames::get_frame_image(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, frames.size(), Ref<Image>());
	return frames[p_idx].image;
}

void ImageFrames::set_frame_delay(int p_idx, float p_delay) {
	ERR_FAIL_INDEX(p_idx, frames.size());
	frames.write[p_idx].delay = p_delay;
}

float ImageFrames::get_frame_delay(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, frames.size(), 0);
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
	ClassDB::bind_method(D_METHOD("save_gif", "filepath", "color_count"), &ImageFrames::save_gif, DEFVAL(256));
#endif
	ClassDB::bind_method(D_METHOD("add_frame", "image", "delay"), &ImageFrames::add_frame, DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("remove_frame", "index"), &ImageFrames::remove_frame);

	ClassDB::bind_method(D_METHOD("set_frame_image", "index", "image"), &ImageFrames::set_frame_image);
	ClassDB::bind_method(D_METHOD("get_frame_image", "index"), &ImageFrames::get_frame_image);

	ClassDB::bind_method(D_METHOD("set_frame_delay", "index", "delay"), &ImageFrames::set_frame_delay);
	ClassDB::bind_method(D_METHOD("get_frame_delay", "index"), &ImageFrames::get_frame_delay);

	ClassDB::bind_method(D_METHOD("get_frame_count"), &ImageFrames::get_frame_count);

	ClassDB::bind_method(D_METHOD("clear"), &ImageFrames::clear);
}
