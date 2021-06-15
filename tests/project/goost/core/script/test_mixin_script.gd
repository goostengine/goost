extends "res://addons/gut/test.gd"

class ScriptA extends Mixin:
	# warning-ignore:unused_class_variable
	var foo_a = "Foo A"
	func bar_a():
		return "Bar A"

class ScriptB extends Mixin:
	# warning-ignore:unused_class_variable
	var foo_b = "Foo B"
	func bar_b():
		return "Bar B"
	func test_get_owner():
		return owner


func test_two_scripts():
	var ms = MixinScript.new()
	ms.script_a = ScriptA
	ms.script_b = ScriptB

	var n = Node.new() # Object *p_this
	n.set_script(ms)
	add_child_autofree(n)

	assert_true(n.test_get_owner() == n)

	assert_eq(n.foo_a, "Foo A")
	assert_eq(n.bar_a(), "Bar A")
	assert_eq(n.foo_b, "Foo B")
	assert_eq(n.bar_b(), "Bar B")
