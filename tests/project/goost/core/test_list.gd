extends "res://addons/gut/test.gd"

func test_push_back():
	var list = VariantList.new()
	var n: VariantListElement
	n = list.push_back("Goost")
	assert_eq(n.value, "Goost")
	n = list.push_back(37)
	assert_eq(n.value, 37)
	n = list.push_back(Color.red)
	assert_eq(n.value, Color.red)
