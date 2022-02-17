extends "res://addons/gut/test.gd"

class CustomSprite extends Sprite:
	pass

func test_node_spawned():
	var spawner = Spawner2D.new()
	spawner.resource = CustomSprite
	add_child_autofree(spawner)
	watch_signals(spawner)

	var node = spawner.spawn()
	assert_not_null(node)

	yield(spawner, "node_spawned")
	assert_signal_emitted(spawner, "node_spawned")
    
	node.queue_free()
