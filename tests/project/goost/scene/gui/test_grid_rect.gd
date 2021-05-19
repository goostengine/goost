extends "res://addons/gut/test.gd"


func test_snap():
	var grid = preload("grid_rect.tscn").instance()
	add_child(grid)
	watch_signals(grid)
	yield(grid, "finished")
	assert_signal_emitted(grid, "finished")
	grid.queue_free()
	grid = null


func test_metadata():
	var grid = GridRect.new()
	add_child(grid)

	grid.set_cell_metadata(Vector2(-1, 1), Color.blue)
	assert_eq(grid.get_cell_metadata(Vector2(-1, 1)), Color.blue)

	grid.clear_cell_metadata()
	assert_eq(grid.get_cell_metadata(Vector2(-1, 1)), null)

	grid.queue_free()
	grid = null
