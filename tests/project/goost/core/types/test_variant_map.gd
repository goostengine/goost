extends "res://addons/gut/test.gd"

func test_create_clear():
	var map = VariantMap.new()
	assert_true(map.is_empty())

	map.create(512, 1024)
	assert_false(map.is_empty())

	assert_eq(map.get_width(), 512)
	assert_eq(map.get_height(), 1024)

	map.clear()
	assert_true(map.is_empty())


func test_create_from_data():
	var map = VariantMap.new()
	map.create_from_data(2, 3, [0, 1, 2, 3, 4, 5])
	gut.p(map)

	assert_eq(map.get_element(0, 0), 0)
	assert_eq(map.get_element(1, 0), 1)
	assert_eq(map.get_element(0, 1), 2)
	assert_eq(map.get_element(1, 1), 3)
	assert_eq(map.get_element(0, 2), 4)
	assert_eq(map.get_element(1, 2), 5)


func test_data():
	var map = VariantMap.new()
	var arr = [0, 1, 2, 3, 4, 5]
	var data = {"width": 2, "height": 3, "data": arr}
	map.data = data

	assert_eq(map.get_element(0, 0), 0)
	assert_eq(map.get_element(1, 0), 1)
	assert_eq(map.get_element(0, 1), 2)
	assert_eq(map.get_element(1, 1), 3)
	assert_eq(map.get_element(0, 2), 4)
	assert_eq(map.get_element(1, 2), 5)

	var d = map.data

	assert_eq(d.width, 2)
	assert_eq(d.height, 3)

	var darr = d.data
	for i in 6:
		assert_eq(darr[i], arr[i])


func test_resize():
	var map = VariantMap.new()
	map.create(4, 4)

	var n = 0
	for y in map.get_height():
		for x in map.get_width():
			map.set_element(x, y, n)
			n += 1

	assert_eq(map.get_element(0, 0), 0)
	assert_eq(map.get_element(1, 0), 1)
	assert_eq(map.get_element(0, 1), 4)
	assert_eq(map.get_element(1, 1), 5)

	map.resize(2, 2)

	var msg = "This should just resize the data, and does not crop the map."
	assert_eq(map.get_element(0, 0), 0, msg)
	assert_eq(map.get_element(1, 0), 1, msg)
	assert_eq(map.get_element(0, 1), 2, msg)
	assert_eq(map.get_element(1, 1), 3, msg)


func test_element_cell():
	var map = VariantMap.new()
	map.create(4, 4)
	assert_null(map.get_element(0, 0))

	map.set_element(0, 0, "Goost")
	assert_not_null(map.get_element(0, 0))
	assert_eq(map.get_element(0, 0), "Goost")

	assert_eq(map.get_cell(Vector2(3, 3)), null)

	map.set_cell(Vector2(0, 0), null)
	assert_null(map.get_element(0, 0))

	assert_true(map.has_cell(Vector2(0, 0)))
	assert_true(map.has_cell(Vector2(3, 0)))
	assert_true(map.has_cell(Vector2(0, 3)))
	assert_true(map.has_cell(Vector2(3, 3)))
	assert_false(map.has_cell(Vector2(0, -1)))
	assert_false(map.has_cell(Vector2(4, 3)))

	assert_null(map.get_cell_or_null(Vector2(-1, -1)))
	assert_null(map.get_cell_or_null(Vector2(3, 4)))
	assert_null(map.get_cell_or_null(Vector2(4, 3)))

	map.set_cell(Vector2(0, 0), "Goost")

	assert_not_null(map.get_cell_or_null(Vector2(0, 0)))
	assert_eq(map.get_cell_or_null(Vector2(0, 0)), "Goost")


func test_custom_iterator():
	var map = VariantMap.new()
	map.create(4, 4)

	var n = 0
	for y in map.get_height():
		for x in map.get_width():
			map.set_element(x, y, n)
			n += 1
	gut.p(map)

	n = 0
	for element in map:
		assert_eq(element, n)
		n += 1


func test_fill():
	var map = VariantMap.new()
	map.fill("Goost") # Should not crash filling empty map.

	map.create(4, 4)
	map.fill("Goost")

	for y in map.get_height():
		for x in map.get_width():
			assert_eq(map.get_element(x, y), "Goost")
