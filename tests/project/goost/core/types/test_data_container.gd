extends "res://addons/gut/test.gd"

var res: DataContainer


func before_each():
	res = DataContainer.new()


func test_set_type():
	res.type = TYPE_VECTOR2
	assert_eq(res.type, TYPE_VECTOR2)


func test_set_value():
	res.value = Color.blue
	assert_eq(res.value, Color.blue)
	assert_eq(res.type, TYPE_COLOR, "Should automatically switch type.")


func test_set_value_convert_type():
	res.value = PoolStringArray(["Godot", "merge", "Goost"])
	assert_eq(res.type, TYPE_STRING_ARRAY)

	res.type = TYPE_STRING
	assert_eq(res.value, "[Godot, merge, Goost]",
			"Should automatically convert previous value to compatible type.")

	res.value = Vector3(1, 2, 3)
	assert_eq(res.type, TYPE_VECTOR3)
	res.type = TYPE_VECTOR2
	assert_eq(res.value, Vector2(1, 2),
			"Should automatically convert previous value to compatible type.")


func test_changed_signal():
	watch_signals(res)
	res.value = "Godot"
	var err = res.connect("changed", self, "_on_changed", [res], CONNECT_ONESHOT)
	assert_eq(err, OK)
	res.value = "Goost"
	assert_signal_emitted(res, "changed")


func _on_changed(p_res):
	assert_eq(p_res.value, "Goost")


func test_convert_from_null_to_string():
	res.value = null
	assert_eq(res.type, TYPE_NIL)

	res.type = TYPE_STRING
	assert_ne(res.value, "Null")
	assert_ne(res.value, "Nil")
	assert_ne(res.value, "null")
	assert_ne(res.value, "nil")
	assert_eq(res.value, "")
