extends "res://addons/gut/test.gd"


func test_push_pop_back():
	var list = LinkedList.new()

	# Push back
	var n: ListElement
	n = list.push_back("Goost")
	assert_eq(n.value, "Goost")
	n = list.push_back(37)
	assert_eq(n.value, 37)
	n = list.push_back(Color.red)
	assert_eq(n.value, Color.red)

	# Pop back
	var v = null
	v = list.back().value
	list.pop_back()
	assert_eq(v, Color.red)
	v = list.back().value
	list.pop_back()
	assert_eq(v, 37)
	v = list.back().value
	list.pop_back()
	assert_eq(v, "Goost")

	assert_null(list.back())
	assert_null(list.front())


func test_cleanup():
	var list = LinkedList.new()
	var n = list.push_back("Goost")
	assert_not_null(n)
	list.unreference()
	list = null
	assert_null(n)
