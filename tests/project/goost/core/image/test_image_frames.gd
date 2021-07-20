extends "res://addons/gut/test.gd"

const SAMPLES = {
	icon = "res://goost/core/image/samples/icon.png",
	wip_gif = "res://goost/core/image/samples/wip.gif",
}

func test_load_gif():
	var image_frames = ImageFrames.new()

	image_frames.load(SAMPLES.wip_gif)
	assert_eq(image_frames.get_frame_count(), 3)
	assert_almost_eq(image_frames.get_frame_delay(0), 0.1, 0.01)
	var output = Image.new()
	output.copy_from(image_frames.get_frame_image(0))
	output.lock()
	assert_eq(output.get_pixel(0, 0), Color("#77b4c6"))
	assert_eq(output.get_size(), Vector2(684, 37))
	output.unlock()


func test_save_gif_static():
	var image_frames = ImageFrames.new()

	var frame = Image.new()
	frame.create(64, 32, false, Image.FORMAT_RGB8)
	frame.fill(Color.yellow)
	image_frames.add_frame(frame, 1.0)

	var err = image_frames.save_gif("res://out/static.gif")
	assert_eq(err, OK)


func test_save_gif_animated():
	var image_frames = ImageFrames.new()

	# Different image sizes are tested here.

	var frame = Image.new()
	frame.create(90, 90, false, Image.FORMAT_RGBA8)
	frame.fill(Color.white)
	image_frames.add_frame(frame, 1.0)

	frame = Image.new()
	frame.create(90, 90, false, Image.FORMAT_RGBA8)
	frame.fill(Color.red)
	image_frames.add_frame(frame, 1.0)

	frame = Image.new()
	frame.create(60, 60, false, Image.FORMAT_RGBA8)
	frame.fill(Color.green)
	image_frames.add_frame(frame, 1.0)

	frame = Image.new()
	frame.create(30, 30, false, Image.FORMAT_RGBA8)
	frame.fill(Color.blue)
	image_frames.add_frame(frame, 1.0)

	var err = image_frames.save_gif("res://out/animated.gif")
	assert_eq(err, OK)


func test_save_gif_animated_rotation():
	var image_frames = ImageFrames.new()

	var angle = 0.0
	for i in 8:
		var frame = TestUtils.image_load(SAMPLES.icon)
		GoostImage.rotate(frame, angle, false)
		image_frames.add_frame(frame, 0.02)
		angle += TAU / 8.0

	var err = image_frames.save_gif("res://out/animated_rotation.gif", 60)
	assert_eq(err, OK)


func test_save_gif_animated_rotation_custom_indexed():
	var image_frames = ImageFrames.new()

	var angle = 0.0
	for i in 8:
		var frame = TestUtils.image_load(SAMPLES.icon)
		GoostImage.rotate(frame, angle, false)
		var indexed = ImageIndexed.new()
		indexed.create_from_data(frame.get_width(), frame.get_height(), false, frame.get_format(), frame.get_data())
		indexed.convert(Image.FORMAT_RGBA8)
		indexed.generate_palette(4, ImageIndexed.DITHER_RANDOM, false, true)
		image_frames.add_frame(indexed, 0.02)
		angle += TAU / 8.0

	var err = image_frames.save_gif("res://out/animated_rotation_custom_indexed.gif")
	assert_eq(err, OK)


func test_image_frames_invalid_data():
	Engine.print_error_messages = false

	var frames = ImageFrames.new()
	frames.get_frame_image(-20)
	frames.load(".", 4)
	frames.load_gif_from_buffer(PoolByteArray([]), -31)
	assert_eq(frames.get_frame_count(), 0)

	Engine.print_error_messages = true
