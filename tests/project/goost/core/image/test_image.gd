extends "res://addons/gut/test.gd"

var output


func after_each():
	output.save_png("res://out/%s.png" % [gut._current_test.name])


func test_image_resize_hqx2_rgb():
	var input = Image.new()
	input.load("res://goost/core/image/samples/rect_rgb.png")
	var input_size = input.get_size()
	output = input
	ImageExtension.resize_hqx(output, 2)
	assert_eq(output.get_size(), input_size * 2)


func test_image_resize_hqx3_rgba():
	var input = Image.new()
	input.load("res://goost/core/image/samples/rect_rgba.png")
	var input_size = input.get_size()
	output = input
	ImageExtension.resize_hqx(output, 3)
	assert_eq(output.get_size(), input_size * 3)


func test_image_rotate_rgba():
	var input = Image.new()
	input.load("res://goost/core/image/samples/rect_rgba.png")
	var input_size = input.get_size()
	output = input
	ImageExtension.rotate(output, deg2rad(45))
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_eq(pixel.a, 0.0)
	output.unlock()


func test_image_rotate_rgb():
	var input = Image.new()
	input.load("res://goost/core/image/samples/rect_rgb.png")
	var input_size = input.get_size()
	output = input
	ImageExtension.rotate(output, deg2rad(45))
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_almost_eq(pixel.r, 0.0, 0.01)
	assert_almost_eq(pixel.g, 0.0, 0.01)
	assert_almost_eq(pixel.b, 0.0, 0.01)
	output.unlock()


func test_image_rotate_grayscale():
	var input = Image.new()
	input.load("res://goost/core/image/samples/rect_grayscale.png")
	var input_size = input.get_size()
	output = input
	ImageExtension.rotate(output, deg2rad(45))
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
	output.lock()
	var pixel = output.get_pixel(output.get_size().x - 1, output.get_size().y / 2)
	assert_eq(pixel, Color("#010101"))
	output.unlock()


func test_image_rotate_90_cw():
	var input = Image.new()
	input.load("res://goost/core/image/samples/rect_rgba.png")
	var input_size = input.get_size()
	output = input
	ImageExtension.rotate_90(output, ImageExtension.CW)
	assert_eq(output.get_size(), Vector2(input_size.y, input_size.x))
	output.lock()
	assert_eq(output.get_pixel(0, 0), Color.green)
	output.unlock()


func test_image_rotate_90_ccw():
	var input = Image.new()
	input.load("res://goost/core/image/samples/rect_rgba.png")
	var input_size = input.get_size()
	output = input
	ImageExtension.rotate_90(output, ImageExtension.CCW)
	assert_eq(output.get_size(), Vector2(input_size.y, input_size.x))
	output.lock()
	assert_eq(output.get_pixel(0, 0), Color(0,0,0,0))
	output.unlock()


func test_image_rotate_180_grayscale():
	var input = Image.new()
	input.load("res://goost/core/image/samples/rect_grayscale.png")
	var input_size = input.get_size()
	output = input
	ImageExtension.rotate_180(output)
	assert_eq(output.get_size(), Vector2(input_size.y, input_size.x))
	output.lock()
	assert_eq(output.get_pixel(0, 63), Color("#010101"))
	output.unlock()


func test_image_render_polygon():
	var polygon = PoolVector2Array([Vector2(10, 10), Vector2(50, 25), Vector2(90, 70), Vector2(12, 100)])
	output = ImageExtension.render_polygon(polygon)
	output.convert(Image.FORMAT_RGB8)
