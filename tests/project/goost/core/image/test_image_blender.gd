extends "res://addons/gut/test.gd"

const SAMPLES = {
	icon = "res://goost/core/image/samples/icon.png",
	rect_rgb_0_25a = "res://goost/core/image/samples/rect_rgb_0_25a.png",
	rect_rgba = "res://goost/core/image/samples/rect_rgba.png"
}
var output


func after_each():
	output.save_png("res://out/%s.png" % [gut._current_test.name])


func test_blend_rect():
	var image_blender = ImageBlender.new()
	
	var input = TestUtils.image_load(SAMPLES.rect_rgb_0_25a)
	output = input
	
	var src_rect = Rect2(Vector2(31, 31), Vector2(2, 2))
	var dst_pos = Vector2(15, 47)

	image_blender.blend_rect(input, src_rect, output, dst_pos)

	output.lock()

	_compare_colors(output.get_pixel(15, 47), Color(0.25, 0.1875, 0, 0.4375))
	_compare_colors(output.get_pixel(16, 47), Color(0, 0.1875, 0, 0.4375))
	_compare_colors(output.get_pixel(15, 48), Color(0, 0.4375, 0, 0.4375))
	_compare_colors(output.get_pixel(16, 48), Color(0, 0.1875, 0.25, 0.4375))

	output.unlock()


func test_stamp_rect():
	var image_blender = ImageBlender.new()
	
	var input = TestUtils.image_load(SAMPLES.rect_rgba)
	output = TestUtils.image_load(SAMPLES.icon)
	
	var src_rect = Rect2(Vector2(31, 31), Vector2(2, 2))
	var dst_init_pos = Vector2(1, 32)
	var dst_end_pos = Vector2(65, 32)

	image_blender.stamp_rect(input, src_rect, output, dst_init_pos, dst_end_pos, 4)

	output.lock()

	_compare_colors(output.get_pixel(0, 31), Color.red)
	_compare_colors(output.get_pixel(0, 32), Color.green)
	_compare_colors(output.get_pixel(1, 32), Color.blue)

	_compare_colors(output.get_pixel(2, 31), Color(0.2109, 0.6718, 0.8281))
	_compare_colors(output.get_pixel(2, 32), Color(0.2109, 0.6718, 0.8281))
	_compare_colors(output.get_pixel(3, 32), Color(0.2109, 0.6718, 0.8281))

	_compare_colors(output.get_pixel(60, 31), Color.red)
	_compare_colors(output.get_pixel(60, 32), Color.green)
	_compare_colors(output.get_pixel(61, 32), Color.blue)

	output.unlock()


func test_blend_equations():
	var image_blender = ImageBlender.new()
	
	var input = TestUtils.image_load(SAMPLES.rect_rgb_0_25a)
	output = TestUtils.image_load(SAMPLES.rect_rgba)
	
	var src_rect = Rect2(Vector2(31, 31), Vector2(2, 2))

	var dst_pos = Vector2(15, 47)
	image_blender.rgb_equation = ImageBlender.FUNC_REVERSE_SUBTRACT
	image_blender.alpha_equation = ImageBlender.FUNC_REVERSE_SUBTRACT
	image_blender.blend_rect(input, src_rect, output, dst_pos)

	dst_pos = Vector2(47, 15)
	image_blender.rgb_equation = ImageBlender.FUNC_SUBTRACT
	image_blender.alpha_equation = ImageBlender.FUNC_SUBTRACT
	image_blender.blend_rect(input, src_rect, output, dst_pos)

	dst_pos = Vector2(15, 15)
	image_blender.rgb_equation = ImageBlender.FUNC_MAX
	image_blender.alpha_equation = ImageBlender.FUNC_MAX
	image_blender.blend_rect(input, src_rect, output, dst_pos)

	dst_pos = Vector2(47, 47)
	image_blender.rgb_equation = ImageBlender.FUNC_MIN
	image_blender.alpha_equation = ImageBlender.FUNC_MIN
	image_blender.blend_rect(input, src_rect, output, dst_pos)

	output.lock()

	_compare_colors(output.get_pixel(15, 47), Color(0, 0.75, 0, 0.5))
	_compare_colors(output.get_pixel(16, 47), Color(0, 0.75, 0, 0.5))
	_compare_colors(output.get_pixel(15, 48), Color(0, 0.5, 0, 0.5))
	_compare_colors(output.get_pixel(16, 48), Color(0, 0.75, 0, 0.5))

	_compare_colors(output.get_pixel(47, 15), Color(0.25, 0, 0, 0.25))
	_compare_colors(output.get_pixel(48, 15), Color(0, 0, 0, 0.25))
	_compare_colors(output.get_pixel(47, 16), Color(0, 0.25, 0, 0.25))
	_compare_colors(output.get_pixel(48, 16), Color(0, 0, 0.25, 0.25))

	_compare_colors(output.get_pixel(15, 15), Color(1, 0, 0, 1))
	_compare_colors(output.get_pixel(16, 15), Color(1, 0, 0, 1))
	_compare_colors(output.get_pixel(15, 16), Color(1, 1, 0, 1))
	_compare_colors(output.get_pixel(16, 16), Color(1, 0, 1, 1))

	_compare_colors(output.get_pixel(47, 47), Color(0, 0, 0, 0.25))
	_compare_colors(output.get_pixel(48, 47), Color(0, 0, 0, 0.25))
	_compare_colors(output.get_pixel(47, 48), Color(0, 0, 0, 0.25))
	_compare_colors(output.get_pixel(48, 48), Color(0, 0, 1, 0.25))

	output.unlock()


func test_blend_factors():
	var image_blender = ImageBlender.new()
	
	var input = TestUtils.image_load(SAMPLES.rect_rgb_0_25a)
	output = TestUtils.image_load(SAMPLES.rect_rgba)
	
	var src_rect = Rect2(Vector2(31, 31), Vector2(2, 2))

	var dst_pos = Vector2(15, 47)
	image_blender.rgb_src_factor = ImageBlender.FACTOR_ZERO
	image_blender.rgb_dst_factor = ImageBlender.FACTOR_SRC_COLOR
	image_blender.alpha_src_factor = ImageBlender.FACTOR_SRC_COLOR
	image_blender.alpha_dst_factor = ImageBlender.FACTOR_ZERO
	image_blender.blend_rect(input, src_rect, output, dst_pos)

	dst_pos = Vector2(47, 15)
	image_blender.rgb_src_factor = ImageBlender.FACTOR_ONE_MINUS_DST_COLOR
	image_blender.rgb_dst_factor = ImageBlender.FACTOR_ONE_MINUS_DST_COLOR
	image_blender.alpha_src_factor = ImageBlender.FACTOR_ONE_MINUS_DST_COLOR
	image_blender.alpha_dst_factor = ImageBlender.FACTOR_ONE_MINUS_DST_COLOR
	image_blender.blend_rect(input, src_rect, output, dst_pos)

	dst_pos = Vector2(15, 15)
	image_blender.rgb_src_factor = ImageBlender.FACTOR_DST_COLOR
	image_blender.rgb_dst_factor = ImageBlender.FACTOR_ONE_MINUS_SRC_COLOR
	image_blender.alpha_src_factor = ImageBlender.FACTOR_ONE_MINUS_SRC_COLOR
	image_blender.alpha_dst_factor = ImageBlender.FACTOR_DST_COLOR
	image_blender.blend_rect(input, src_rect, output, dst_pos)

	dst_pos = Vector2(47, 47)
	image_blender.rgb_src_factor = ImageBlender.FACTOR_DST_ALPHA
	image_blender.rgb_dst_factor = ImageBlender.FACTOR_ONE_MINUS_DST_ALPHA
	image_blender.alpha_src_factor = ImageBlender.FACTOR_ONE_MINUS_DST_ALPHA
	image_blender.alpha_dst_factor = ImageBlender.FACTOR_DST_ALPHA
	image_blender.blend_rect(input, src_rect, output, dst_pos)

	output.lock()

	_compare_colors(output.get_pixel(15, 47), Color(0, 0, 0, 0.0625))
	_compare_colors(output.get_pixel(16, 47), Color(0, 0, 0, 0.0625))
	_compare_colors(output.get_pixel(15, 48), Color(0, 1, 0, 0.0625))
	_compare_colors(output.get_pixel(16, 48), Color(0, 0, 0, 0.0625))

	_compare_colors(output.get_pixel(47, 15), Color(1, 0, 0, 0.25))
	_compare_colors(output.get_pixel(48, 15), Color(0, 0, 0, 0.25))
	_compare_colors(output.get_pixel(47, 16), Color(0, 1, 0, 0.25))
	_compare_colors(output.get_pixel(48, 16), Color(0, 0, 1, 0.25))

	_compare_colors(output.get_pixel(15, 15), Color(1, 0, 0, 1))
	_compare_colors(output.get_pixel(16, 15), Color(1, 0, 0, 1))
	_compare_colors(output.get_pixel(15, 16), Color(1, 0, 0, 1))
	_compare_colors(output.get_pixel(16, 16), Color(1, 0, 0, 1))

	_compare_colors(output.get_pixel(47, 47), Color(1, 0, 0, 1))
	_compare_colors(output.get_pixel(48, 47), Color(0, 0, 0, 1))
	_compare_colors(output.get_pixel(47, 48), Color(0, 1, 0, 1))
	_compare_colors(output.get_pixel(48, 48), Color(0, 0, 1, 1))

	output.unlock()


func _compare_colors(color, expected_color):
	assert_almost_eq(color.r, expected_color.r, 0.01)
	assert_almost_eq(color.g, expected_color.g, 0.01)
	assert_almost_eq(color.b, expected_color.b, 0.01)
	assert_almost_eq(color.a, expected_color.a, 0.01)


class TestInvalidData extends "res://addons/gut/test.gd":
	const SAMPLES = {
		rect_rgb = "res://goost/core/image/samples/rect_rgb.png"
	}

	func before_all():
		Engine.print_error_messages = false

	func after_all():
		Engine.print_error_messages = true

	func test_null():
		var image = null

		var _image_blender = ImageBlender.new()

		var _src_image = TestUtils.image_load(SAMPLES.rect_rgb)
		var _src_rect = Rect2(Vector2(0, 0), Vector2(32, 32))
		var _dst_image = TestUtils.image_load(SAMPLES.rect_rgb)
		var _dst_pos = Vector2(32, 32)

		var _invalid_src_image = Image.new()
		var _invalid_src_rect = Rect2(Vector2(-10.089389, 3.712648), Vector2(0.970958, -0.239249))
		var _invalid_dst_pos = Vector2(0.627138, 0.778908)

		_image_blender.blend_rect(_invalid_src_image, _src_rect, _dst_image, _dst_pos)
		_image_blender.blend_rect(_src_image, _invalid_src_rect, _dst_image, _dst_pos)
		_image_blender.blend_rect(_src_image, _src_rect, image, _dst_pos)
		_image_blender.blend_rect(_src_image, _src_rect, _dst_image, _invalid_dst_pos)

		var _dst_end_pos = Vector2(48, 48)
		var _spacing = 1

		var _invalid_dst_end_pos = Vector2(-31.293953, 20.851303)
		var _invalid_spacing = -0.209764

		_image_blender.stamp_rect(_invalid_src_image, _src_rect, _dst_image, _dst_pos, _dst_end_pos, _spacing)
		_image_blender.stamp_rect(_src_image, _invalid_src_rect, _dst_image, _dst_pos, _dst_end_pos, _spacing)
		_image_blender.stamp_rect(_src_image, _src_rect, image, _dst_pos, _dst_end_pos, _spacing)
		_image_blender.stamp_rect(_src_image, _src_rect, _dst_image, _invalid_dst_pos, _dst_end_pos, _spacing)
		_image_blender.stamp_rect(_src_image, _src_rect, _dst_image, _dst_pos, _invalid_dst_end_pos, _spacing)
		_image_blender.stamp_rect(_src_image, _src_rect, _dst_image, _dst_pos, _dst_end_pos, _invalid_spacing)

		assert_null(image)
