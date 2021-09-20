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
	ms.add_mixin(MixinA)
	ms.add_mixin(MixinB)

	var n = Node.new()
	n.set_script(ms)
	add_child_autofree(n)

	assert_eq(n.foo_a, "Foo A")
	assert_eq(n.bar_a(), "Bar A")
	assert_eq(n.foo_b, "Foo B")
	assert_eq(n.bar_b(), "Bar B")

	ms = n.get_script()
	ms.remove_mixin(0)

	assert_false("foo_a" in n)
	assert_false(n.has_method("bar_a"))
	assert_true("foo_b" in n)
	assert_true(n.has_method("bar_b"))

	ms.remove_mixin(0)

	assert_false("foo_a" in n)
	assert_false(n.has_method("bar_a"))
	assert_false("foo_b" in n)
	assert_false(n.has_method("bar_b"))

	ms.add_mixin(MixinB)

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

	ms.clear_mixins()

	assert_false("foo_a" in n)
	assert_false("foo_b" in n)


func test_setget_scripts():
	var ms = MixinScript.new()
	var mixins = [MixinA, MixinB]

	for m in mixins:
		ms.add_mixin(m)

	var n = Node.new()
	n.set_script(ms)
	add_child_autofree(n)

	for idx in ms.get_mixin_count():
		var m = ms.get_mixin(idx)
		assert_eq(m, mixins[idx])

	ms.set_mixin(0, MixinB)
	assert_eq(MixinB, ms.get_mixin(0))

	
class MixinFirst extends Mixin:
	func tell():
		return "First"

class MixinSecond extends Mixin:
	func tell():
		return "Second"


func test_move_script():
	var ms = MixinScript.new()
	ms.add_mixin(MixinFirst)
	ms.add_mixin(MixinSecond)

	var n = Node.new()
	n.set_script(ms)
	add_child_autofree(n)

	assert_eq(n.tell(), "First")
	ms.move_mixin(0, MixinFirst)
	assert_eq(n.tell(), "First", "Did not move")
	ms.move_mixin(1, MixinFirst)
	assert_eq(n.tell(), "Second")


func test_mixin_get_owner():
	var m = Mixin.new()
	var o = m.get_owner()
	assert_null(o)
	m.free()
	assert_freed(m, "Freed")


func test_mixin_set_to_self():
	var ms = MixinScript.new()
	Engine.print_error_messages = false
	ms.add_mixin(ms)
	Engine.print_error_messages = true

	var n = Node.new()
	n.set_script(ms)
	add_child_autofree(n)

	assert_not_null(ms)
