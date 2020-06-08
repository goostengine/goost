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


func test_image_rotate():
	var input = Image.new()
	input.load("res://goost/core/image/samples/rect_rgb.png")
	var input_size = input.get_size()
	output = input
	ImageExtension.rotate(output, deg2rad(45))
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)


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
