extends "res://addons/gut/test.gd"

var output


func after_each():
	output.save_png("res://out/%s.png" % [gut._current_test.name])


func test_image_resize_hqx2_rgb():
	var input = Goost.image_load_no_warning("res://goost/core/image/samples/rect_rgb.png")
	var input_size = input.get_size()
	output = input
	GoostImage.resize_hqx(output, 2)
	assert_eq(output.get_size(), input_size * 2)


func test_image_resize_hqx3_rgba():
	var input = Goost.image_load_no_warning("res://goost/core/image/samples/rect_rgba.png")
	var input_size = input.get_size()
	output = input
	GoostImage.resize_hqx(output, 3)
	assert_eq(output.get_size(), input_size * 3)


func test_image_rotate_rgba():
	var input = Goost.image_load_no_warning("res://goost/core/image/samples/rect_rgba.png")
	var input_size = input.get_size()
	output = input
	GoostImage.rotate(output, deg2rad(45))
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_eq(pixel.a, 0.0)
	output.unlock()


func test_image_rotate_rgb():
	var input = Goost.image_load_no_warning("res://goost/core/image/samples/rect_rgb.png")
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


func test_image_rotate_grayscale():
	var input = Goost.image_load_no_warning("res://goost/core/image/samples/rect_grayscale.png")
	var input_size = input.get_size()
	output = input
	GoostImage.rotate(output, deg2rad(45), true)
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_eq(pixel, Color("#010101"))
	output.unlock()


func test_image_rotate_no_expand():
	var input = Goost.image_load_no_warning("res://goost/core/image/samples/rect_rgba.png")
	var input_size = input.get_size()
	output = input
	GoostImage.rotate(output, deg2rad(45), false)
	assert_eq(output.get_size(), input_size)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_eq(pixel.a, 0.0)
	output.unlock()


func test_image_rotate_90_cw():
	var input = Goost.image_load_no_warning("res://goost/core/image/samples/rect_rgba.png")
	var input_size = input.get_size()
	output = input
	GoostImage.rotate_90(output, GoostImage.CW)
	assert_eq(output.get_size(), Vector2(input_size.y, input_size.x))
	output.lock()
	assert_eq(output.get_pixel(0, 0), Color.green)
	output.unlock()


func test_image_rotate_90_ccw():
	var input = Goost.image_load_no_warning("res://goost/core/image/samples/rect_rgba.png")
	var input_size = input.get_size()
	output = input
	GoostImage.rotate_90(output, GoostImage.CCW)
	assert_eq(output.get_size(), Vector2(input_size.y, input_size.x))
	output.lock()
	assert_eq(output.get_pixel(0, 0), Color(0,0,0,0))
	output.unlock()


func test_image_rotate_180_grayscale():
	var input = Goost.image_load_no_warning("res://goost/core/image/samples/rect_grayscale.png")
	var input_size = input.get_size()
	output = input
	GoostImage.rotate_180(output)
	assert_eq(output.get_size(), Vector2(input_size.y, input_size.x))
	output.lock()
	assert_eq(output.get_pixel(0, 63), Color("#010101"))
	output.unlock()


func test_image_render_polygon_non_filled():
	var polygon = [Vector2(124, 44), Vector2(101, 115), Vector2(26, 115), Vector2(0, 44), Vector2(64, 0)]
	output = GoostImage.render_polygon(polygon, false)
	output.convert(Image.FORMAT_RGB8) # So it's visible on image preview.
	output.lock()
	for p in polygon:
		var pixel = output.get_pixelv(p)
		assert_eq(pixel, Color.white, "Outline on the image should match polygon vertices.")
	output.unlock()


func test_image_render_polygon_filled():
	var polygon = [Vector2(124, 44), Vector2(101, 115), Vector2(26, 115), Vector2(0, 44), Vector2(64, 0)]
	output = GoostImage.render_polygon(polygon, true)
	output.lock()
	var pixel = output.get_pixelv(output.get_size() / 2)
	assert_eq(pixel, Color.white, "Center of the image should be white.")
	output.unlock()


func test_image_render_polygon_filled_color():
	var polygon = [Vector2(124, 44), Vector2(101, 115), Vector2(26, 115), Vector2(0, 44), Vector2(64, 0)]
	output = GoostImage.render_polygon(polygon, true, Color.green, Color.blue)
	output.lock()
	var pixel_fg = output.get_pixelv(output.get_size() / 2)
	assert_eq(pixel_fg, Color.green)
	var pixel_bg = output.get_pixel(0, 0)
	assert_eq(pixel_bg, Color.blue)
	output.unlock()


func test_image_binarize_adaptive():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/icon.png")
	GoostImage.binarize(output)
	output.lock()
	assert_eq(output.get_pixel(8, 9), Color.white)
	output.unlock()
	output.convert(Image.FORMAT_RGB8) # For debugging purposes.


func test_image_binarize_threshold():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/icon.png")
	GoostImage.binarize(output, 0.85)
	output.lock()
	assert_eq(output.get_pixel(8, 9), Color(0,0,0,0))
	assert_eq(output.get_pixel(32, 24), Color.white)
	output.unlock()
	output.convert(Image.FORMAT_RGB8) # For debugging purposes.


func test_image_binarize_threshold_invert():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/icon.png")
	GoostImage.binarize(output, 0.85, true)
	output.lock()
	assert_eq(output.get_pixel(8, 9), Color.white)
	assert_eq(output.get_pixel(32, 24), Color(0,0,0,0))
	output.unlock()
	output.convert(Image.FORMAT_RGB8) # For debugging purposes.


func test_image_centroid():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/stroke.png")
	var centroid = GoostImage.get_centroid(output)
	assert_almost_eq(centroid, Vector2(37, 38), Vector2(0.5, 0.5))
	output.lock()
	output.set_pixelv(centroid, Color.red) # For debugging purposes.
	output.unlock()


func test_image_morph_type_dilate():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/stroke.png")
	GoostImage.morph(output, GoostImage.MORPH_DILATE)
	output.lock()
	assert_eq(output.get_pixel(50, 35), Color.white)
	output.unlock()


func test_image_morph_type_erode():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/stroke.png")
	GoostImage.morph(output, GoostImage.MORPH_ERODE)
	output.lock()
	assert_eq(output.get_pixel(16, 33), Color.black)
	assert_eq(output.get_pixel(39, 7), Color.white)
	output.unlock()


func test_image_dilate_5px():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/stroke.png")
	GoostImage.dilate(output, 5)
	output.lock()
	assert_ne(output.get_pixel(11, 38), Color.black)
	output.unlock()


func test_image_erode_5px():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/stroke.png")
	GoostImage.erode(output, 5)
	output.lock()
	assert_eq(output.get_pixel(50, 32), Color.black)
	output.unlock()


func test_image_morph_open():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/stroke.png")
	GoostImage.morph(output, GoostImage.MORPH_OPEN)
	output.lock()
	assert_eq(output.get_pixel(38, 6), Color.white)
	assert_eq(output.get_pixel(33, 19), Color.white)
	assert_eq(output.get_pixel(16, 33), Color.black)
	assert_eq(output.get_pixel(19, 10), Color.black)
	output.unlock()


func test_image_morph_close():
	output = Goost.image_load_no_warning("res://goost/core/image/samples/stroke.png")
	GoostImage.morph(output, GoostImage.MORPH_CLOSE)
	output.lock()
	assert_eq(output.get_pixel(55, 16), Color.white)
	assert_eq(output.get_pixel(51, 36), Color.white)
	assert_eq(output.get_pixel(34, 53), Color.white)
	assert_eq(output.get_pixel(20, 53), Color.white)
	output.unlock()
