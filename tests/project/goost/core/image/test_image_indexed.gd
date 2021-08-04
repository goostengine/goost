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


func test_save_indexed_png():
    var image = ImageIndexed.new()
    image.create(64, 64, false, Image.FORMAT_RGBA8)
    image.fill(Color.blue)
    image.generate_palette(4, ImageIndexed.DITHER_RANDOM, false, true)
    image.save_indexed_png("res://out/indexed.png")
    assert_not_null(image)


func test_save_indexed_png_empty():
    Engine.print_error_messages = false

    var image = ImageIndexed.new()
    image.save_indexed_png("res://out/indexed.png")
    assert_not_null(image)

    Engine.print_error_messages = true
