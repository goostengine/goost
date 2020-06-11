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
	GoostImage.rotate(output, deg2rad(45))
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_eq(pixel, Color("#010101"))
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
