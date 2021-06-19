extends "res://addons/gut/test.gd"

class MixinA extends Mixin:
	# warning-ignore:unused_class_variable
	var foo_a = "Foo A"
	func bar_a():
		return "Bar A"

class MixinB extends Mixin:
	# warning-ignore:unused_class_variable
	var foo_b = "Foo B"
	func bar_b():
		return "Bar B"
	func test_get_owner():
		return owner


func test_two_scripts():
	var ms = MixinScript.new()
	ms.mixins = [MixinA, MixinB]

	var n = Node.new()
	n.set_script(ms)
	add_child_autofree(n)

	assert_true(n.test_get_owner() == n)

	assert_eq(n.foo_a, "Foo A")
	assert_eq(n.bar_a(), "Bar A")
	assert_eq(n.foo_b, "Foo B")
	assert_eq(n.bar_b(), "Bar B")


func test_add_remove():
	var ms = MixinScript.new()
	ms.add_script(MixinA)
	ms.add_script(MixinB)

	var n = Node.new()
	n.set_script(ms)
	add_child_autofree(n)

	assert_eq(n.foo_a, "Foo A")
	assert_eq(n.bar_a(), "Bar A")
	assert_eq(n.foo_b, "Foo B")
	assert_eq(n.bar_b(), "Bar B")

	ms = n.get_script()
	ms.remove_script(0)

	assert_false("foo_a" in n)
	assert_false(n.has_method("bar_a"))
	assert_true("foo_b" in n)
	assert_true(n.has_method("bar_b"))

	ms.remove_script(0)

	assert_false("foo_a" in n)
	assert_false(n.has_method("bar_a"))
	assert_false("foo_b" in n)
	assert_false(n.has_method("bar_b"))

	ms.add_script(MixinB)

	assert_eq(n.foo_b, "Foo B")
	assert_eq(n.bar_b(), "Bar B")


func test_clear_scripts():
	var ms = MixinScript.new()
	ms.mixins = [MixinA, MixinB]

	var n = Node.new()
	n.set_script(ms)
	add_child_autofree(n)

	assert_eq(n.foo_a, "Foo A")
	assert_eq(n.foo_b, "Foo B")

	ms.clear_scripts()

	assert_false("foo_a" in n)
	assert_false("foo_b" in n)


func test_setget_scripts():
	var ms = MixinScript.new()
	var mixins = [MixinA, MixinB]

	for m in mixins:
		ms.add_script(m)

	var n = Node.new()
	n.set_script(ms)
	add_child_autofree(n)

	for idx in ms.get_script_count():
		var m = ms.get_script_at_index(idx)
		assert_eq(m, mixins[idx])

	ms.set_script_at_index(0, MixinB)
	assert_eq(MixinB, ms.get_script_at_index(0))

	
class MixinFirst extends Mixin:
	func tell():
		return "First"

class MixinSecond extends Mixin:
	func tell():
		return "Second"


func test_move_script():
	var ms = MixinScript.new()
	ms.add_script(MixinFirst)
	ms.add_script(MixinSecond)

	var n = Node.new()
	n.set_script(ms)
	add_child_autofree(n)

	assert_eq(n.tell(), "First")
	ms.move_script(0, MixinFirst)
	assert_eq(n.tell(), "First", "Did not move")
	ms.move_script(1, MixinFirst)
	assert_eq(n.tell(), "Second")
