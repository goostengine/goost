extends "res://addons/gut/test.gd"

func test_create_clear():
	var grid = Grid2D.new()
	assert_true(grid.is_empty())

	grid.create(512, 1024)
	assert_false(grid.is_empty())

	assert_eq(grid.get_width(), 512)
	assert_eq(grid.get_height(), 1024)

	grid.clear()
	assert_true(grid.is_empty())


func test_create_from_data():
	var grid = Grid2D.new()
	grid.create_from_data(2, 3, [0, 1, 2, 3, 4, 5])
	gut.p(grid)

	assert_eq(grid.get_element(0, 0), 0)
	assert_eq(grid.get_element(1, 0), 1)
	assert_eq(grid.get_element(0, 1), 2)
	assert_eq(grid.get_element(1, 1), 3)
	assert_eq(grid.get_element(0, 2), 4)
	assert_eq(grid.get_element(1, 2), 5)


func test_data():
	var grid = Grid2D.new()
	var arr = [0, 1, 2, 3, 4, 5]
	var data = {"width": 2, "height": 3, "data": arr}
	grid.data = data

	assert_eq(grid.get_element(0, 0), 0)
	assert_eq(grid.get_element(1, 0), 1)
	assert_eq(grid.get_element(0, 1), 2)
	assert_eq(grid.get_element(1, 1), 3)
	assert_eq(grid.get_element(0, 2), 4)
	assert_eq(grid.get_element(1, 2), 5)

	var d = grid.data

	assert_eq(d.width, 2)
	assert_eq(d.height, 3)

	var darr = d.data
	for i in 6:
		assert_eq(darr[i], arr[i])


func test_resize():
	var grid = Grid2D.new()
	grid.create(4, 4)

	var n = 0
	for y in grid.get_height():
		for x in grid.get_width():
			grid.set_element(x, y, n)
			n += 1

	assert_eq(grid.get_element(0, 0), 0)
	assert_eq(grid.get_element(1, 0), 1)
	assert_eq(grid.get_element(0, 1), 4)
	assert_eq(grid.get_element(1, 1), 5)

	grid.resize(2, 2)

	var msg = "This should just resize the data, and does not crop the grid."
	assert_eq(grid.get_element(0, 0), 0, msg)
	assert_eq(grid.get_element(1, 0), 1, msg)
	assert_eq(grid.get_element(0, 1), 2, msg)
	assert_eq(grid.get_element(1, 1), 3, msg)


func test_element_cell():
	var grid = Grid2D.new()
	grid.create(4, 4)
	assert_null(grid.get_element(0, 0))

	grid.set_element(0, 0, "Goost")
	assert_not_null(grid.get_element(0, 0))
	assert_eq(grid.get_element(0, 0), "Goost")

	assert_eq(grid.get_cell(Vector2(3, 3)), null)

	grid.set_cell(Vector2(0, 0), null)
	assert_null(grid.get_element(0, 0))

	assert_true(grid.has_cell(Vector2(0, 0)))
	assert_true(grid.has_cell(Vector2(3, 0)))
	assert_true(grid.has_cell(Vector2(0, 3)))
	assert_true(grid.has_cell(Vector2(3, 3)))
	assert_false(grid.has_cell(Vector2(0, -1)))
	assert_false(grid.has_cell(Vector2(4, 3)))

	assert_null(grid.get_cell_or_null(Vector2(-1, -1)))
	assert_null(grid.get_cell_or_null(Vector2(3, 4)))
	assert_null(grid.get_cell_or_null(Vector2(4, 3)))

	grid.set_cell(Vector2(0, 0), "Goost")

	assert_not_null(grid.get_cell_or_null(Vector2(0, 0)))
	assert_eq(grid.get_cell_or_null(Vector2(0, 0)), "Goost")


func test_custom_iterator():
	var grid = Grid2D.new()
	grid.create(4, 4)

	var n = 0
	for y in grid.get_height():
		for x in grid.get_width():
			grid.set_element(x, y, n)
			n += 1
	gut.p(grid)

	n = 0
	for element in grid:
		assert_eq(element, n)
		n += 1


func test_fill():
	var grid = Grid2D.new()
	grid.fill("Goost") # Should not crash filling empty grid.

	grid.create(4, 4)
	grid.fill("Goost")

	for y in grid.get_height():
		for x in grid.get_width():
			assert_eq(grid.get_element(x, y), "Goost")
