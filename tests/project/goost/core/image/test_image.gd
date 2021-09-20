extends "res://addons/gut/test.gd"

const SAMPLES = {
	icon = "res://goost/core/image/samples/icon.png",
	icon_svg = "res://goost/core/image/samples/icon.svg",
	icon_binary = "res://goost/core/image/samples/icon_binary.png",
	rect_grayscale = "res://goost/core/image/samples/rect_grayscale.png",
	rect_rgb = "res://goost/core/image/samples/rect_rgb.png",
	rect_rgba = "res://goost/core/image/samples/rect_rgba.png",
	stroke = "res://goost/core/image/samples/stroke.png",
	wip_gif = "res://goost/core/image/samples/wip.gif",
}
var output


func after_each():
	if output:
		output.save_png("res://out/%s.png" % [gut._current_test.name])


func test_replace_color():
	var input = TestUtils.image_load(SAMPLES.rect_rgb)
	output = input

	GoostImage.replace_color(output, Color.red, Color.blue)
	output.lock()
	assert_eq(output.get_pixel(0, 0), Color.blue)
	output.unlock()


func test_resize_hqx2_rgb():
	var input = TestUtils.image_load(SAMPLES.rect_rgb)
	var input_size = input.get_size()
	output = input
	GoostImage.resize_hqx(output, 2)
	assert_eq(output.get_size(), input_size * 2)


func test_resize_hqx3_rgba():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	var input_size = input.get_size()
	output = input
	GoostImage.resize_hqx(output, 3)
	assert_eq(output.get_size(), input_size * 3)


func test_rotate_rgba():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	var input_size = input.get_size()
	output = input
	GoostImage.rotate(output, deg2rad(45))
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_eq(pixel.a, 0.0)
	output.unlock()


func test_rotate_rgb():
	var input = TestUtils.image_load(SAMPLES.rect_rgb)
	assert_eq(input.get_format(), Image.FORMAT_RGB8)
	var input_size = input.get_size()
	output = input
	GoostImage.rotate(output, deg2rad(45))
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_almost_eq(pixel.r, 0.0, 0.01)
	assert_almost_eq(pixel.g, 0.0, 0.01)
	assert_almost_eq(pixel.b, 0.0, 0.01)
	output.unlock()


func test_rotate_grayscale():
	var input = TestUtils.image_load(SAMPLES.rect_grayscale)
	var input_size = input.get_size()
	output = input
	GoostImage.rotate(output, deg2rad(45), true)
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_eq(pixel, Color("#010101"))
	output.unlock()


func test_rotate_no_expand():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	var input_size = input.get_size()
	output = input
	GoostImage.rotate(output, deg2rad(45), false)
	assert_eq(output.get_size(), input_size)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_eq(pixel.a, 0.0)
	output.unlock()


func test_rotate_90_cw():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	var input_size = input.get_size()
	output = input
	GoostImage.rotate_90(output, GoostImage.CW)
	assert_eq(output.get_size(), Vector2(input_size.y, input_size.x))
	output.lock()
	assert_eq(output.get_pixel(0, 0), Color.green)
	output.unlock()


func test_rotate_90_ccw():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	var input_size = input.get_size()
	output = input
	GoostImage.rotate_90(output, GoostImage.CCW)
	assert_eq(output.get_size(), Vector2(input_size.y, input_size.x))
	output.lock()
	assert_eq(output.get_pixel(0, 0), Color(0,0,0,0))
	output.unlock()


func test_rotate_180_grayscale():
	var input = TestUtils.image_load(SAMPLES.rect_grayscale)
	var input_size = input.get_size()
	output = input
	GoostImage.rotate_180(output)
	assert_eq(output.get_size(), Vector2(input_size.y, input_size.x))
	output.lock()
	assert_eq(output.get_pixel(0, 63), Color("#010101"))
	output.unlock()


func test_render_polygon_non_filled():
	var polygon = [Vector2(124, 44), Vector2(101, 115), Vector2(26, 115), Vector2(0, 44), Vector2(64, 0)]
	output = GoostImage.render_polygon(polygon, false)
	output.convert(Image.FORMAT_RGB8) # So it's visible on image preview.
	output.lock()
	for p in polygon:
		var pixel = output.get_pixelv(p)
		assert_eq(pixel, Color.white, "Outline on the image should match polygon vertices.")
	output.unlock()


func test_render_polygon_filled():
	var polygon = [Vector2(124, 44), Vector2(101, 115), Vector2(26, 115), Vector2(0, 44), Vector2(64, 0)]
	output = GoostImage.render_polygon(polygon, true)
	output.lock()
	var pixel = output.get_pixelv(output.get_size() / 2)
	assert_eq(pixel, Color.white, "Center of the image should be white.")
	output.unlock()


func test_render_polygon_invalid():
	# Input from the fuzzer...
	Engine.print_error_messages = false
	output = GoostImage.render_polygon([Vector2(0.695821, 0.718215), Vector2(-30.700972, -42.334), Vector2(0.959952, -0.280165), Vector2(-11.923018, 20.621294), Vector2(-0.594874, 0.803819), Vector2(0.801119, -0.598505), Vector2(-0.129389, 0.991594), Vector2(-8.80065, -27.064463), Vector2(-0.987193, -0.159532), Vector2(-0.778665, 0.62744), Vector2(0.764317, 0.644841), Vector2(2.220136, 26.017015), Vector2(0.986621, -0.163031), Vector2(-0.935623, 0.353002), Vector2(0.904148, 0.427219)], true)
	Engine.print_error_messages = true
	assert_null(output)


func test_render_polygon_filled_color():
	var polygon = [Vector2(124, 44), Vector2(101, 115), Vector2(26, 115), Vector2(0, 44), Vector2(64, 0)]
	output = GoostImage.render_polygon(polygon, true, Color.green, Color.blue)
	output.lock()
	var pixel_fg = output.get_pixelv(output.get_size() / 2)
	assert_eq(pixel_fg, Color.green)
	var pixel_bg = output.get_pixel(0, 0)
	assert_eq(pixel_bg, Color.blue)
	output.unlock()


func test_binarize_adaptive():
	output = TestUtils.image_load(SAMPLES.icon)
	GoostImage.binarize(output)
	output.lock()
	assert_eq(output.get_pixel(8, 9), Color.white)
	output.unlock()
	assert_eq(output.get_format(), Image.FORMAT_L8)


func test_binarize_threshold():
	output = TestUtils.image_load(SAMPLES.icon)
	GoostImage.binarize(output, 0.85)
	output.lock()
	assert_eq(output.get_pixel(8, 9), Color.black)
	assert_eq(output.get_pixel(32, 24), Color.white)
	output.unlock()
	assert_eq(output.get_format(), Image.FORMAT_L8)


func test_binarize_threshold_invert():
	output = TestUtils.image_load(SAMPLES.icon)
	GoostImage.binarize(output, 0.85, true)
	output.lock()
	assert_eq(output.get_pixel(8, 9), Color.white)
	assert_eq(output.get_pixel(32, 24), Color.black)
	output.unlock()
	assert_eq(output.get_format(), Image.FORMAT_L8)


func test_centroid():
	output = TestUtils.image_load(SAMPLES.stroke)
	var centroid = GoostImage.get_centroid(output)
	assert_almost_eq(centroid, Vector2(37, 38), Vector2(0.5, 0.5))
	output.lock()
	output.set_pixelv(centroid, Color.red) # For debugging purposes.
	output.unlock()


func test_morph_type_dilate():
	output = TestUtils.image_load(SAMPLES.stroke)
	GoostImage.morph(output, GoostImage.MORPH_DILATE)
	output.lock()
	assert_eq(output.get_pixel(50, 35), Color.white)
	output.unlock()


func test_morph_type_erode():
	output = TestUtils.image_load(SAMPLES.stroke)
	GoostImage.morph(output, GoostImage.MORPH_ERODE)
	output.lock()
	assert_eq(output.get_pixel(16, 33), Color.black)
	assert_eq(output.get_pixel(39, 7), Color.white)
	output.unlock()


func test_dilate_5px():
	output = TestUtils.image_load(SAMPLES.stroke)
	GoostImage.dilate(output, 5)
	output.lock()
	assert_ne(output.get_pixel(11, 38), Color.black)
	output.unlock()


func test_erode_5px():
	output = TestUtils.image_load(SAMPLES.stroke)
	GoostImage.erode(output, 5)
	output.lock()
	assert_eq(output.get_pixel(50, 32), Color.black)
	output.unlock()


func test_morph_open():
	output = TestUtils.image_load(SAMPLES.stroke)
	GoostImage.morph(output, GoostImage.MORPH_OPEN)
	output.lock()
	assert_eq(output.get_pixel(38, 6), Color.white)
	assert_eq(output.get_pixel(33, 19), Color.white)
	assert_eq(output.get_pixel(16, 33), Color.black)
	assert_eq(output.get_pixel(19, 10), Color.black)
	output.unlock()


func test_morph_close():
	output = TestUtils.image_load(SAMPLES.stroke)
	GoostImage.morph(output, GoostImage.MORPH_CLOSE)
	output.lock()
	assert_eq(output.get_pixel(55, 16), Color.white)
	assert_eq(output.get_pixel(51, 36), Color.white)
	assert_eq(output.get_pixel(34, 53), Color.white)
	assert_eq(output.get_pixel(20, 53), Color.white)
	output.unlock()


func debug_color(color):
	var pixel_debug = Image.new()
	pixel_debug.create(64, 64, false, Image.FORMAT_RGBA8)
	pixel_debug.fill(color)
	output = pixel_debug


func test_get_pixel_average():
	output = TestUtils.image_load(SAMPLES.icon)
	var color = GoostImage.get_pixel_average(output)
	output.lock()
	assert_almost_eq(color.r, 0.2, 0.001)
	assert_almost_eq(color.g, 0.4, 0.001)
	assert_almost_eq(color.b, 0.501, 0.001)
	assert_eq(color.a, 1.0)
	output.unlock()
	debug_color(color)


func test_get_pixel_average_grayscale():
	output = TestUtils.image_load(SAMPLES.icon_binary)
	var color = GoostImage.get_pixel_average(output)
	output.lock()
	assert_almost_eq(color.r, 0.449, 0.001)
	assert_almost_eq(color.g, 0.449, 0.001)
	assert_almost_eq(color.b, 0.449, 0.001)
	assert_eq(color.a, 1.0)
	output.unlock()
	debug_color(color)


func test_get_pixel_average_rect_full():
	output = TestUtils.image_load(SAMPLES.rect_rgba)
	var s = output.get_size()
	var rect = Rect2(0, 0, s.x, s.y) # Full rect.
	var color = GoostImage.get_pixel_average(output, rect)
	output.lock()
	assert_almost_eq(color.r, 0.247, 0.001)
	assert_almost_eq(color.g, 0.247, 0.001)
	assert_almost_eq(color.b, 0.247, 0.001)
	output.unlock()
	debug_color(color)


func test_get_pixel_average_rect_top_left():
	output = TestUtils.image_load(SAMPLES.rect_rgba)
	var s = output.get_size()
	var rect = Rect2(0, 0, s.x / 2, s.y / 2) # Top-left rect.
	var color = GoostImage.get_pixel_average(output, rect)
	output.lock()
	assert_eq(color.r, 1.0)
	assert_eq(color.g, 0.0)
	assert_eq(color.b, 0.0)
	output.unlock()
	debug_color(color)


func test_get_pixel_average_rect_bottom_right():
	output = TestUtils.image_load(SAMPLES.rect_rgba)
	var s = output.get_size()
	var rect = Rect2(s.x / 2, s.y / 2, s.x / 2, s.y / 2) # Bottom-right rect.
	var color = GoostImage.get_pixel_average(output, rect)
	output.lock()
	assert_eq(color.r, 0.0)
	assert_eq(color.g, 0.0)
	assert_eq(color.b, 1.0)
	output.unlock()
	debug_color(color)


func test_get_pixel_average_rect_mixed():
	output = TestUtils.image_load(SAMPLES.rect_rgba)
	var s = output.get_size()
	var rect = Rect2(0, 0, s.x / 2, s.y) # Between R and G.
	var color = GoostImage.get_pixel_average(output, rect)
	output.lock()
	assert_almost_eq(color.r, 0.5, 0.01)
	assert_almost_eq(color.g, 0.5, 0.01)
	assert_eq(color.b, 0.0)
	output.unlock()
	debug_color(color)


func test_get_pixel_average_mask_full():
	output = TestUtils.image_load(SAMPLES.rect_rgba)
	var s = output.get_size()
	var mask = Image.new()
	mask.create(s.x, s.y, false, Image.FORMAT_L8)
	mask.fill(Color.white)
	var color = GoostImage.get_pixel_average(output, Rect2(), mask)
	output.lock()
	assert_almost_eq(color.r, 0.247, 0.001)
	assert_almost_eq(color.g, 0.247, 0.001)
	assert_almost_eq(color.b, 0.247, 0.001)
	output.unlock()
	debug_color(color)


func test_get_pixel_average_mask_top_left():
	output = TestUtils.image_load(SAMPLES.rect_rgba)
	var s = output.get_size()
	var mask = Image.new()
	mask.create(s.x / 2.0, s.y / 2.0, false, Image.FORMAT_L8)
	mask.fill(Color.white)
	var color = GoostImage.get_pixel_average(output, Rect2(), mask)
	output.lock()
	assert_eq(color.r, 1.0)
	assert_eq(color.g, 0.0)
	assert_eq(color.b, 0.0)
	output.unlock()
	debug_color(color)


func test_repeat():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	var input_size = input.get_size()
	output = GoostImage.repeat(input, Vector2(3, 4), GoostImage.TILE)
	assert_eq(output.get_size(), Vector2(input_size.x * 3, input_size.y * 4))


func test_repeat_flip_x():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	var input_size = input.get_size()
	output = GoostImage.repeat(input, Vector2(4, 3), GoostImage.TILE_FLIP_X)
	assert_eq(output.get_size(), Vector2(input_size.x * 4, input_size.y * 3))


func test_repeat_flip_y():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	var input_size = input.get_size()
	output = GoostImage.repeat(input, Vector2(3, 4), GoostImage.TILE_FLIP_Y)
	assert_eq(output.get_size(), Vector2(input_size.x * 3, input_size.y * 4))


func test_repeat_flip_xy():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	var input_size = input.get_size()
	output = GoostImage.repeat(input, Vector2(4, 4), GoostImage.TILE_FLIP_XY)
	assert_eq(output.get_size(), Vector2(input_size.x * 4, input_size.y * 4))


func test_tile_arbitrary_size():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	output = GoostImage.tile(input, Vector2(300, 400))
	assert_eq(output.get_size(), Vector2(300, 400))


func test_tile_dest_smaller_than_tile():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	output = GoostImage.tile(input, Vector2(16, 8))
	assert_eq(output.get_size(), Vector2(16, 8))
	output.lock()
	assert_eq(output.get_pixel(15, 7), Color.red)
	output.unlock()


func test_tile_thin_slice():
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	output = GoostImage.tile(input, Vector2(16, 100))
	assert_eq(output.get_size(), Vector2(16, 100))
	output.lock()
	assert_eq(output.get_pixel(15, 99), Color.green)
	output.unlock()


func test_render_svg():
	var file = File.new()
	file.open(SAMPLES.icon_svg, File.READ)
	var svg_document = file.get_as_text()
	output = GoostImage.render_svg(svg_document, 2.0)
	output.lock()
	assert_eq(output.get_size(), Vector2(246, 246))
	assert_eq(output.get_pixel(60, 50), Color("#36acd4"))
	assert_eq(output.get_pixel(210, 143), Color("#303d5a"))
	assert_eq(output.get_pixel(130, 100), Color("#ffffff"))
	output.unlock()
	file.close()


func test_bucket_fill_4_connected():
	var input = TestUtils.image_load(SAMPLES.icon_binary)
	input.convert(Image.FORMAT_RGBA8)
	var filled = GoostImage.bucket_fill(input, Vector2(20, 20), Color.red, true, GoostImage.FOUR_CONNECTED)
	output = input
	# Image.
	output.lock()
	assert_eq(output.get_pixel(20, 20), Color.red)
	assert_eq(output.get_pixel(44, 44), Color.red)
	assert_eq(output.get_pixel(45, 26), Color.white, "Should not fill this portion, it's 4-connected.")
	assert_eq(output.get_pixel(62, 37), Color.white)
	output.unlock()
	# Filled area.
	filled.lock()
	assert_eq(filled.get_pixel(20, 20), Color.red)
	assert_eq(filled.get_pixel(44, 44), Color.red)
	assert_eq(filled.get_pixel(45, 26), Color(0,0,0,0), "Should be black, not white.")
	assert_eq(filled.get_pixel(62, 37), Color(0,0,0,0), "Should be black, not white.")
	filled.unlock()


func test_bucket_fill_8_connected():
	var input = TestUtils.image_load(SAMPLES.icon_binary)
	input.convert(Image.FORMAT_RGBA8)
	var filled = GoostImage.bucket_fill(input, Vector2(20, 20), Color.red, true, GoostImage.EIGHT_CONNECTED)
	output = input
	# Image.
	output.lock()
	assert_eq(output.get_pixel(20, 20), Color.red)
	assert_eq(output.get_pixel(44, 44), Color.red)
	assert_eq(output.get_pixel(45, 26), Color.red, "Should fill this portion, it's 8-connected.")
	assert_eq(output.get_pixel(62, 37), Color.white)
	output.unlock()
	# Filled area.
	filled.lock()
	assert_eq(filled.get_pixel(20, 20), Color.red)
	assert_eq(filled.get_pixel(44, 44), Color.red)
	assert_eq(filled.get_pixel(45, 26), Color.red)
	assert_eq(filled.get_pixel(62, 37), Color(0,0,0,0), "Should be black, not white.")
	filled.unlock()


class TestInvalidData extends "res://addons/gut/test.gd":
	func before_all():
		Engine.print_error_messages = false

	func after_all():
		Engine.print_error_messages = true

	func test_null():
		var image = null

		GoostImage.replace_color(image, Color.red, Color.blue)
		var _filled = GoostImage.bucket_fill(image, Vector2(), Color())
		GoostImage.resize_hqx(image, 9000)
		GoostImage.rotate(image, -9000, true)
		GoostImage.rotate_90(image, -1)
		GoostImage.rotate_180(image)
		GoostImage.binarize(image, 0, false)
		GoostImage.dilate(image, 9000)
		GoostImage.erode(image, 9000)
		GoostImage.morph(image, -1, Vector2(9000, -9000))
		var _centroid = GoostImage.get_centroid(Image.new())
		var average = GoostImage.get_pixel_average(image)
		assert_eq(average, Color())
		average = GoostImage.get_pixel_average(Image.new(), Rect2(Vector2(-0.371352, -0.928492), Vector2(-0.643768, -0.76522)), null)
		assert_eq(average, Color())
		var _has = GoostImage.get_pixel_or_null(image, -1, -1)
		_has = GoostImage.get_pixelv_or_null(image, Vector2(-1, 1))

		assert_null(image)
