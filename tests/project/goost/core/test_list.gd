extends "res://addons/gut/test.gd"

var list: LinkedList


func before_each():
	list = LinkedList.new()
	
	
func populate_test_data(list: LinkedList):
	var elements = []
	elements.append(list.push_back("Goost"))
	elements.append(list.push_back(37))
	elements.append(list.push_back(Vector2.ONE))
	elements.append(list.push_back(Color.blue))
	return elements


func test_push_pop_back():
	# Push back
	var n: ListElement
	n = list.push_back("Goost")
	assert_eq(n.value, "Goost")
	n = list.push_back(37)
	assert_eq(n.value, 37)
	n = list.push_back(Color.blue)
	assert_eq(n.value, Color.blue)

	# Pop back
	var v = null
	v = list.back().value
	list.pop_back()
	assert_eq(v, Color.blue)
	v = list.back().value
	list.pop_back()
	assert_eq(v, 37)
	v = list.back().value
	list.pop_back()
	assert_eq(v, "Goost")

	assert_null(list.back())
	assert_null(list.front())


func test_push_pop_front():
	# Push front
	var n: ListElement
	n = list.push_front("Goost")
	assert_eq(n.value, "Goost")
	n = list.push_front(37)
	assert_eq(n.value, 37)
	n = list.push_front(Color.blue)
	assert_eq(n.value, Color.blue)

	# Pop front
	var v = null
	v = list.front().value
	list.pop_front()
	assert_eq(v, Color.blue)
	v = list.front().value
	list.pop_front()
	assert_eq(v, 37)
	v = list.front().value
	list.pop_front()
	assert_eq(v, "Goost")

	assert_null(list.back())
	assert_null(list.front())


func test_size():
	var elements = populate_test_data(list)
	var original_size = elements.size()
	assert_eq(list.size(), original_size)


func test_front():
	populate_test_data(list)
	assert_eq(list.front().value, "Goost")


func test_back():
	populate_test_data(list)
	assert_eq(list.back().value, Color.blue)
	

func test_find():
	populate_test_data(list)
	assert_eq(list.find("Goost").value, "Goost")


func test_erase():
	var elements = populate_test_data(list)
	var original_size = elements.size()
	var erased = list.erase(Color.blue)
	assert_true(erased)
	assert_eq(list.size(), original_size - 1)
	erased = list.erase("does not exist")
	assert_false(erased)
	

func test_remove():
	var elements = populate_test_data(list)
	var original_size = elements.size()
	list.remove(list.find("Goost"))
	assert_eq(list.size(), original_size - 1)
	assert_null(list.find("Goost"))


func test_empty():
	populate_test_data(list)
	var n: ListElement = list.front()
	while n:
		list.remove(n)
		n = list.front()
	assert_eq(list.size(), 0)


func test_clear():
	populate_test_data(list)
	list.clear()
	assert_eq(list.size(), 0)
	assert_null(list.find("Goost"))
	assert_null(list.find(37))
	assert_null(list.find(Color.blue))


func test_next():
	populate_test_data(list)
	var n = list.front()
	assert_eq(n.value, "Goost")
	n = n.next()
	assert_eq(n.value, 37)


func test_prev():
	populate_test_data(list)
	var n = list.back()
	assert_eq(n.value, Color.blue)
	n = n.prev()
	assert_eq(n.value, Vector2.ONE)


func test_move_to_front():
	var elements = populate_test_data(list)
	list.move_to_front(elements[2])
	assert_eq(list.front().value, Vector2.ONE)


func test_move_to_back():
	var elements = populate_test_data(list)
	list.move_to_back(elements[0])
	assert_eq(list.back().value, "Goost")


func test_move_before():
	var elements = populate_test_data(list)
	assert_eq(elements[3].value, Color.blue)
	list.move_before(elements[3], elements[1])
	assert_eq(list.front().next().value, elements[3].value)


func test_cleanup():
	assert_eq(list.size(), 0)
	var n = list.push_back("Goost")
	assert_eq(list.size(), 1)
	assert_not_null(n)
	list = null
	assert_null(n)
