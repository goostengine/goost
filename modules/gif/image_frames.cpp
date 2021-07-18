#include "image_frames.h"
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

Error ImageFrames::save_gif(const String &p_filepath) {
	ERR_FAIL_COND_V(get_frame_count() == 0, ERR_CANT_CREATE);

	int error;
	CharString fp = p_filepath.utf8();

	GifFileType *gif = EGifOpenFileName(fp.get_data(), false, &error);
	if (!gif) {
		print_line(vformat("EGifOpenFileName() failed - %s", error));
		return ERR_CANT_CREATE;
	}

	const int palette_size = 2;
	const int byte_count = 16;

	GifColorType colors[palette_size];
	colors[0].Red = 0x00;
	colors[0].Green = 0x00;
	colors[0].Blue = 0x00;

	colors[1].Red = 0xFF;
	colors[1].Green = 0xFF;
	colors[1].Blue = 0xFF;

	GifByteType pix1[byte_count] = {
		0,
		1,
		0,
		1,
		1,
		0,
		1,
		0,
		0,
		1,
		0,
		1,
		1,
		0,
		1,
		0,
	};

	GifByteType pix2[byte_count] = {
		1,
		0,
		1,
		0,
		0,
		1,
		0,
		1,
		1,
		0,
		1,
		0,
		0,
		1,
		0,
		1,
	};

	gif->SWidth = 4;
	gif->SHeight = 4;
	gif->SColorResolution = 8;
	gif->SBackGroundColor = 0;
	gif->SColorMap = GifMakeMapObject(palette_size, colors);

	if (EGifPutScreenDesc(gif,
				gif->SWidth,
				gif->SHeight,
				gif->SColorResolution,
				gif->SBackGroundColor,
				gif->SColorMap) == GIF_ERROR) {
		return ERR_CANT_CREATE;
	}

	for (int i = 0; i < 2; ++i) {
		ColorMapObject *cmap = nullptr;
		const float delay = 1.0; // Seconds.

		GifByteType *raster = (GifByteType *)malloc(byte_count);
		if (i == 0) {
			memcpy(raster, pix1, byte_count);
		} else if (i == 1) {
			memcpy(raster, pix2, byte_count);
		}

		// Add delay.
		EGifPutExtensionLeader(gif, GRAPHICS_EXT_FUNC_CODE);
		static unsigned char gfx_ext_data[4] = {
			0x04,
			CLAMP(int(100 * delay), 0, 255) % 0xFF,
			CLAMP(int(100 * delay), 0, 255) / 0xFF,
			0x00
		};
		EGifPutExtensionBlock(gif, 4, gfx_ext_data);
		EGifPutExtensionTrailer(gif);

		if (EGifPutImageDesc(gif, 0, 0, gif->SWidth, gif->SHeight, false, cmap) == GIF_ERROR) {
			return ERR_CANT_CREATE;
		}

		for (int j = 0; j < gif->SHeight; j++) {
			if (EGifPutLine(gif, raster + j * gif->SWidth, gif->SWidth) == GIF_ERROR) {
				return ERR_CANT_CREATE;
			}
		}
	}

	// Add loop.
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

	EGifCloseFile(gif, &error);

	return OK;
}

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

	ClassDB::bind_method(D_METHOD("save_gif", "filepath"), &ImageFrames::save_gif);

	ClassDB::bind_method(D_METHOD("add_frame", "image", "delay", "idx"), &ImageFrames::add_frame, DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("remove_frame", "idx"), &ImageFrames::remove_frame);

	ClassDB::bind_method(D_METHOD("set_frame_image", "idx", "image"), &ImageFrames::set_frame_image);
	ClassDB::bind_method(D_METHOD("get_frame_image", "idx"), &ImageFrames::get_frame_image);

	ClassDB::bind_method(D_METHOD("set_frame_delay", "idx", "delay"), &ImageFrames::set_frame_delay);
	ClassDB::bind_method(D_METHOD("get_frame_delay", "idx"), &ImageFrames::get_frame_delay);

	ClassDB::bind_method(D_METHOD("get_frame_count"), &ImageFrames::get_frame_count);

	ClassDB::bind_method(D_METHOD("clear"), &ImageFrames::clear);
}
