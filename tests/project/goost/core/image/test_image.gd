extends "res://addons/gut/test.gd"

var texture
var input
var input_size
var output

func setup():
	texture = preload("res://icon.png")
	input = texture.get_data()
	input_size = input.get_size()


func before_each():
	output = input.duplicate()


func after_each():
	output.save_png("res://out/%s.png" % [gut._current_test.name])


func test_image_resize_hqx2():
	ImageExtension.resize_hqx(output, 2)
	assert_eq(output.get_size(), input_size * 2)


func test_image_resize_hqx3():
	ImageExtension.resize_hqx(output, 3)
	assert_eq(output.get_size(), input_size * 3)


func test_image_rotate():
	ImageExtension.rotate(output, deg2rad(45))
	assert_almost_eq(output.get_size(), input_size * sqrt(2), Vector2.ONE * 0.5)
