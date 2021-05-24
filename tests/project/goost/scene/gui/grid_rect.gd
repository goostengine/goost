extends Control

signal finished


func _ready():
	$grid.show_behind_parent = true

	# Position : [ Point : Point-to-position ]
	var data: Dictionary
	# Default.
	reset_to_default()
	data = {
		Vector2(0, 0) : [ Vector2(0, 0), Vector2(0, 0) ],
		Vector2(128, 0) : [ Vector2(4, 0), Vector2(128, 0) ],
		Vector2(32, 32) : [ Vector2(1, 1), Vector2(32, 32) ],
		Vector2(47.9, 32) : [ Vector2(1, 1), Vector2(32, 32) ],
	}
	yield(test(data), "completed")

	# Offset.
	$grid.origin_offset = Vector2(32, 32)
	data = {
		Vector2(0, 0) : [ Vector2(-1, -1), Vector2(0, 0) ],
		Vector2(32, 32) : [ Vector2(0, 0), Vector2(32, 32) ],
		Vector2(47.9, 32) : [ Vector2(0, 0), Vector2(32, 32) ],
		Vector2(137, 83) : [ Vector2(3, 2), Vector2(128, 96) ],
	}
	yield(test(data), "completed")

	# Custom cell size.
	$grid.cell_size = Vector2(101.53, 56.86)
	data = {
		Vector2(776, 431) : [ Vector2(8, 8), Vector2(812.23999, 454.880005) ],
		Vector2(110, 293) : [ Vector2(1, 5), Vector2(101.529999, 284.299988) ],
	}
	yield(test(data), "completed")

	# Scale.
	$grid.origin_scale = Vector2(0.5, 0.5)
	data = {
		Vector2(0, 0) : [ Vector2(0, 0), Vector2(0, 0) ],
		Vector2(128, 0) : [ Vector2(8, 0), Vector2(128, 0) ],
		Vector2(67, 58) : [ Vector2(4, 4), Vector2(64, 64) ],
		Vector2(168, 58) : [ Vector2(11, 4), Vector2(176, 64) ],
	}
	yield(test(data), "completed")

	# Offset + Scale.
	$grid.origin_offset = Vector2(146, 246)
	$grid.origin_scale = Vector2(1.5, 0.5)
	data = {
		Vector2(106, 153) : [ Vector2(-1, -6), Vector2(98, 150) ],
		Vector2(279, 184) : [ Vector2(3, -4), Vector2(290, 182) ],
		Vector2(328, 495) : [ Vector2(4, 16), Vector2(338, 502) ],
	}
	yield(test(data), "completed")

	# Centered origin.
	$grid.origin_centered = true
	data = {
		Vector2(902, 426) : [ Vector2(-2, -4), Vector2(896, 412) ],
		Vector2(1384, 177) : [ Vector2(13, -11), Vector2(1376, 188) ],
		Vector2(704, 795) : [ Vector2(-8, 8), Vector2(704, 796) ],
	}
	yield(test(data), "completed")

	# Centered origin + origin_offset + origin_scale.
	$grid.origin_centered = true
	$grid.origin_offset = Vector2(-100, -200)
	$grid.origin_scale = Vector2(0.74, 1.33)
	data = {
		Vector2(1048, 681) : [ Vector2(8, 8), Vector2(1049.439941, 680.47998) ],
		Vector2(300, 221) : [ Vector2(-24, -3), Vector2(291.679993, 212.319992) ],
		Vector2(671, 1019) : [ Vector2(-8, 16), Vector2(670.559998, 1020.960022) ],
	}
	yield(test(data), "completed")

	# Centered origin + origin_offset + cell_origin
	$grid.cell_origin = GridRect.CELL_ORIGIN_CENTER
	$grid.origin_centered = true
	$grid.origin_offset = Vector2(-100, -200)
	data = {
		Vector2(985, 374) : [ Vector2(3, 1), Vector2(972, 388) ],
		Vector2(669, 214) : [ Vector2(-6, -4), Vector2(684, 228) ],
	}
	yield(test(data), "completed")

	# Flip origin.
	$grid.origin_centered = true
	$grid.origin_scale = Vector2(-1, -1)
	data = {
		Vector2(1051, 630) : [ Vector2(-3, -3), Vector2(1056, 636) ],
	}
	yield(test(data), "completed")

	emit_signal("finished")


func reset_to_default():
	$grid.cell_size = Vector2(32, 32)
	$grid.cell_origin = GridRect.CELL_ORIGIN_TOP_LEFT
	$grid.origin_offset = Vector2()
	$grid.origin_scale = Vector2.ONE
	$grid.origin_centered = false


func click(position):
	var event = InputEventMouseButton.new()
	event.position = position
	event.pressed = true
	event.button_index = BUTTON_LEFT
	Input.call_deferred("parse_input_event", event)
	var point = yield($grid, "point_clicked")
	return point


var _debug_pos = Vector2()

func test(data):
	for position in data:
		var point = click(position)
		if point is GDScriptFunctionState:
			point = yield(point, "completed")
		var snapped_point = point[1]
		assert(snapped_point == data[position][0])
		var pos = $grid.point_to_view(snapped_point)
		_debug_pos = pos
		assert(pos.is_equal_approx(data[position][1]))
	reset_to_default()


func _process(_delta):
	$debug_position.text = str(get_global_mouse_position())
#	$debug_position.text = str(_debug_pos)
	update()


func _draw():
	draw_circle(_debug_pos, 2, Color.white)
