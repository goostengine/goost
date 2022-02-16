extends "res://addons/gut/test.gd"

var graph: Graph

func before_each():
	graph = Graph.new()


func test_add_vertex():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")

	assert_eq(a.value, "a")
	assert_eq(b.value, "b")


func test_get_graph():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var e = graph.add_edge(a, b)

	assert_eq(a.get_graph(), graph)
	assert_eq(b.get_graph(), graph)
	assert_eq(e.get_graph(), graph)


func test_find_vertex():
	for i in 100:
		var _v = graph.add_vertex(str(i))

	var v = graph.find_vertex("50")
	assert_eq(v.value, "50")


func test_add_directed_edge():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")

	var e1 = graph.add_directed_edge(a, b)

	assert_eq(graph.find_edge(a, b), e1)
	assert_null(graph.find_edge(b, a), e1)

	var e2 = graph.add_directed_edge("c", "d")

	assert_eq(graph.find_edge(e2.a, e2.b), e2)
	assert_null(graph.find_edge(e2.b, e2.a), e2)


func test_add_edge():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")

	var e1 = graph.add_edge(a, b)

	assert_eq(graph.find_edge(a, b), e1)
	assert_eq(graph.find_edge(b, a), e1)

	var e2 = graph.add_edge("c", "d")

	assert_eq(graph.find_edge(e2.a, e2.b), e2)
	assert_eq(graph.find_edge(e2.b, e2.a), e2)


func test_add_edge_by_value_connected():
	var ab = graph.add_edge("a", "b")
	var ac = graph.add_edge("a", "c") # A exists, C does not (yet).
	var bc = graph.add_edge("b", "c") # B exists, C exists as well.

	assert_eq(graph.get_vertex_count(), 3,
			"Vertices should be connected by value (no duplicates)")
	
	var a = graph.find_vertex("a")
	var b = graph.find_vertex("b")
	var c = graph.find_vertex("c")

	assert_true(a in graph.get_vertex_list())
	assert_true(b in graph.get_vertex_list())
	assert_true(c in graph.get_vertex_list())

	assert_eq(graph.find_edge(a, b), ab)
	assert_eq(graph.find_edge(b, a), ab)

	assert_eq(graph.find_edge(a, c), ac)
	assert_eq(graph.find_edge(c, a), ac)

	assert_eq(graph.find_edge(b, c), bc)
	assert_eq(graph.find_edge(c, b), bc)


func test_neighborhood():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")

	var _e1 = graph.add_edge(a, b)
	var _e2 = graph.add_edge(a, c)
	var _e3 = graph.add_edge(b, c)

	var na = a.get_neighbors()
	var nb = b.get_neighbors()
	var nc = c.get_neighbors()

	assert_eq(a.get_neighbor_count(), 2)
	assert_true(b in na)
	assert_true(c in na)

	assert_eq(b.get_neighbor_count(), 2)
	assert_true(a in nb)
	assert_true(c in nb)

	assert_eq(c.get_neighbor_count(), 2)
	assert_true(a in nc)
	assert_true(b in nc)


func test_neighborhood_directed():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")

	var _e1 = graph.add_edge(a, b)
	var _e2 = graph.add_directed_edge(a, c)

	var na = a.get_neighbors()
	var sa = a.get_successors()
	var pc = c.get_predecessors()

	assert_eq(a.get_neighbor_count(), 2)
	assert_true(b in na)
	assert_true(c in na)

	assert_eq(a.get_successor_count(), 1)
	assert_true(c in sa)

	assert_eq(c.get_predecessor_count(), 1)
	assert_true(a in pc)

	var _e3 = graph.add_directed_edge(c, a)
	var _e4 = graph.add_directed_edge(c, b)
	var sc = c.get_successors()

	assert_eq(sc.size(), 2)
	assert_true(a in sc)
	assert_true(b in sc)


func test_edge_list():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")

	# Different weights.
	var ab1 = graph.add_edge(a, b, 1)
	var ab2 = graph.add_edge(b, a, 2) 

	var ac = graph.add_edge(a, c)
	var bc = graph.add_edge(b, c)

	var edges = graph.get_edge_list(a, b)
	assert_eq(edges.size(), 2)
	assert_ne(ab1, ab2, "Multiple undirected edges should be allowed") 
	assert_true(ab1 in edges)
	assert_true(ab2 in edges)

	edges = graph.get_edge_list()
	assert_eq(edges.size(), 4)
	assert_true(ab1 in edges)
	assert_true(ab2 in edges)
	assert_true(ac in edges)
	assert_true(bc in edges)


func test_remove_vertex_one_to_many():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")
	var d = graph.add_vertex("d")

	var _ab = graph.add_edge(a, b)
	var _ac = graph.add_edge(a, c)
	var _ad = graph.add_edge(a, d)

	assert_eq(graph.get_edge_count(), 3)

	graph.remove_vertex(a)

	assert_eq(graph.get_edge_count(), 0)
	assert_eq(b.get_neighbor_count(), 0)
	assert_eq(c.get_neighbor_count(), 0)
	assert_eq(d.get_neighbor_count(), 0)


func test_remove_vertex_many_to_many():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")
	var d = graph.add_vertex("d")

	var _ab = graph.add_edge(a, b)
	var _ac = graph.add_edge(a, c)
	var _ad = graph.add_edge(a, d)
	var _cb = graph.add_edge(c, b)
	var _cd = graph.add_edge(c, d)
	var _bd = graph.add_edge(b, d)

	assert_eq(graph.get_edge_count(), 6)

	a.free() # By destructor.

	assert_eq(graph.get_vertex_count(), 3)

	assert_eq(graph.get_edge_count(), 3)
	assert_eq(c.get_neighbors().size(), 2)
	assert_eq(b.get_neighbors().size(), 2)
	assert_eq(d.get_neighbors().size(), 2)

	graph.remove_vertex(b)

	assert_eq(graph.get_edge_count(), 1)
	assert_eq(c.get_neighbors().size(), 1)
	assert_eq(d.get_neighbors().size(), 1)

	graph.remove_vertex(c)

	assert_eq(graph.get_edge_count(), 0)
	assert_eq(d.get_neighbors().size(), 0)


func test_clear_edges():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")

	var _ab = graph.add_edge(a, b)
	var _ac = graph.add_edge(a, c)
	var _bc = graph.add_edge(b, c)

	assert_eq(graph.get_vertex_count(), 3)
	assert_eq(graph.get_edge_count(), 3)
	assert_eq(a.get_neighbors().size(), 2)
	assert_eq(b.get_neighbors().size(), 2)
	assert_eq(c.get_neighbors().size(), 2)

	graph.clear_edges()

	assert_eq(graph.get_vertex_count(), 3)
	assert_eq(graph.get_edge_count(), 0)
	assert_eq(a.get_neighbors().size(), 0)
	assert_eq(b.get_neighbors().size(), 0)
	assert_eq(c.get_neighbors().size(), 0)


func test_has_edge():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")

	var _ab = graph.add_edge(a, b)
	var _ac = graph.add_edge(a, c)

	assert_true(graph.has_edge(a, b))
	assert_true(graph.has_edge(a, c))
	assert_true(graph.has_edge(b, a))
	assert_false(graph.has_edge(b, c))
	assert_false(graph.has_edge(c, b))


func test_remove_edge():
	var ab = graph.add_edge("a", "b")
	var ac = graph.add_edge("a", "c")
	var bc = graph.add_edge("b", "c")

	assert_eq(graph.get_edge_count(), 3)

	ab.free() # By destructor
	assert_eq(graph.get_edge_count(), 2)

	graph.remove_edge(ac)
	assert_eq(graph.get_edge_count(), 1)

	graph.remove_edge(bc)
	assert_eq(graph.get_edge_count(), 0)

	assert_eq(graph.get_vertex_count(), 3)


func test_self_loop():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")

	var _e1 = graph.add_edge(a, a)
	var _e2 = graph.add_edge(b, a)

	assert_eq(a.get_neighbors()[0], a)

	graph.remove_vertex(a)
	graph.remove_vertex(b)
	# Should not crash...


func test_connected_component_tree():
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")
	var d = graph.add_vertex("d")
	var e = graph.add_vertex("e")

	var _ab = graph.add_edge(a, b)
	var _bc = graph.add_edge(b, c)
	var _bd = graph.add_edge(b, d)
	var ae = graph.add_edge(a, e)

	var abcde = graph.find_connected_component(a)
	assert_eq(abcde.size(), 5)
	assert_true(a in abcde)
	assert_true(b in abcde)
	assert_true(c in abcde)
	assert_true(d in abcde)
	assert_true(e in abcde)

	assert_true(graph.is_strongly_connected())

	graph.remove_edge(ae)

	assert_false(graph.is_strongly_connected())


func test_connected_component_cycle():
	var V = []
	for i in 4:
		V.push_back(graph.add_vertex(i))

	var _e = null
	_e = graph.add_edge(V[0], V[1])
	_e = graph.add_edge(V[0], V[2])
	_e = graph.add_edge(V[0], V[3])
	_e = graph.add_edge(V[1], V[2])
	_e = graph.add_edge(V[1], V[3])
	_e = graph.add_edge(V[2], V[3])

	graph.set_iterator_dfs()

	var component = graph.find_connected_component(V[0])
	assert_eq(component.size(), 4)
	assert_true(V[0] in component)
	assert_true(V[1] in component)
	assert_true(V[2] in component)
	assert_true(V[3] in component)

	graph.set_iterator_bfs()

	component = graph.find_connected_component(V[0])
	assert_eq(component.size(), 4)
	assert_true(V[0] in component)
	assert_true(V[1] in component)
	assert_true(V[2] in component)
	assert_true(V[3] in component)

	
func create_grid(p_graph, p_width, p_height):
	p_graph.clear()

	var id = 0

	var grid = []
	for j in p_height:
		var row = []
		for i in p_width:
			var v = p_graph.add_vertex(id)
			row.push_back(v)
			id += 1
		grid.push_back(row)

	for row in grid:
		for i in p_width - 1:
			var a = row[i]
			var b = row[i + 1]
			p_graph.add_edge(a, b)
			
	for j in p_height - 1:
		var row_a = grid[j]
		var row_b = grid[j + 1]
		
		for i in p_width:
			var a = row_a[i]
			var b = row_b[i]
			p_graph.add_edge(a, b)

	return grid


func test_breadth_iterator():
	var grid = create_grid(graph, 5, 10)

	graph.set_iterator_bfs()
	var G = graph.iterator
	G.initialize(grid[0][0])

	var count = 0
	while G.has_next():
		var v = G.next()
		count += 1
		if count == 1:
			assert_eq(v, grid[0][0])

		elif count <= 3:
			assert_true(v in [grid[1][0], grid[0][1]])
		
		elif count >= 4 and count <= 6:
			assert_true(v in [grid[2][0], grid[1][1], grid[0][2]])

	assert_eq(count, 50)


func test_depth_iterator():
	var grid = create_grid(graph, 10, 10)

	graph.set_iterator_dfs()
	var G = graph.iterator
	G.initialize(grid[0][0])

	var count = 0
	while G.has_next():
		var _v = G.next()
		count += 1

	assert_eq(count, 100)


func test_gdscript_iterator():
	for i in 5:
		var _v = graph.add_vertex(i)
	for i in range(1, 5):
		var _e = graph.add_edge(0, i)

	for vertex in graph:
		assert_not_null(vertex)
		if OS.is_stdout_verbose():
			gut.p(vertex.value)

	if OS.is_stdout_verbose():	
		gut.p("Neighbors:")

	for neighbor in graph.find_vertex(0):
		assert_not_null(neighbor)
		if OS.is_stdout_verbose():
			gut.p(neighbor.value)


func test_get_connected_components():
	var V = []
	for i in 10:
		V.push_back(graph.add_vertex(i))

	var _e = null
	_e = graph.add_edge(V[0], V[1])
	_e = graph.add_edge(V[0], V[2])
	_e = graph.add_edge(V[3], V[4])
	_e = graph.add_edge(V[6], V[7])
	_e = graph.add_edge(V[6], V[8])
	_e = graph.add_edge(V[7], V[8])
	_e = graph.add_edge(V[8], V[9])
	_e = graph.add_edge(V[7], V[9])

	var roots = graph.get_connected_components()
	assert_eq(roots.size(), 4)
	
	for r in roots:
		var members = roots[r]
		assert_true(r in members, "Representative vertex should be a member")

	for r in roots:
		var members = graph.find_connected_component(r)
		assert_lt(members.size(), 5)
		match members.size():
			1:
				assert_eq(members[0], V[5])
			2:
				assert_true(V[3] in members)
				assert_true(V[4] in members)
			3:
				assert_true(V[0] in members)
				assert_true(V[1] in members)
				assert_true(V[2] in members)
			4:
				assert_true(V[6] in members)
				assert_true(V[7] in members)
				assert_true(V[8] in members)
				assert_true(V[9] in members)


class GraphIteratorCustom extends GraphIterator:
	var v: GraphVertex
	var count

	func initialize(root):
		v = root
		count = 0

	func has_next():
		count += 1
		return count < 5

	func next():
		var n = v
		v = v.get_successors()[0]
		return n


func test_iterator():
	var it = graph.iterator
	assert(it is GraphIterator)

	graph.iterator = GraphIteratorCustom.new()

	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")

	var _ab = graph.add_directed_edge(a, b)
	var _bc = graph.add_directed_edge(b, c)
	var _ca = graph.add_directed_edge(c, a)

	var component = graph.find_connected_component(a)
	assert_eq(component.size(), 4)
	assert_eq(component[0], a)
	assert_eq(component[1], b)
	assert_eq(component[2], c)
	assert_eq(component[3], a)

	var V = graph.iterator
	V.initialize(a)

	var steps = 0
	while V.has_next():
		var _n = V.next()
		steps += 1
	assert_eq(steps, 4)
	
	
func test_mst():
	var V = []
	V.push_back(graph.add_vertex(Vector2(0, 0)))
	V.push_back(graph.add_vertex(Vector2(200, 0)))
	V.push_back(graph.add_vertex(Vector2(50, 60)))
	V.push_back(graph.add_vertex(Vector2(100, 100)))
	V.push_back(graph.add_vertex(Vector2(0, 100)))

	# Generate complete graph.
	for i in range(0, V.size()):
		var a = V[i]
		for j in range(i + 1, V.size()):
			var b = V[j]
			# Euclidean minimum spanning tree.
			var w = a.value.distance_to(b.value)
			var _e = graph.add_edge(a, b, w)

	assert_eq(graph.get_edge_count(), 10, "Should generate complete graph")

	var mst = graph.minimum_spanning_tree()

	assert_false(graph.find_edge(V[0], V[1]) in mst)
	assert_false(graph.find_edge(V[0], V[3]) in mst)
	assert_false(graph.find_edge(V[0], V[4]) in mst)
	assert_false(graph.find_edge(V[1], V[2]) in mst)
	assert_false(graph.find_edge(V[1], V[4]) in mst)
	assert_false(graph.find_edge(V[3], V[4]) in mst)

	assert_true(graph.find_edge(V[0], V[2]) in mst)
	assert_true(graph.find_edge(V[1], V[3]) in mst)
	assert_true(graph.find_edge(V[2], V[3]) in mst)
	assert_true(graph.find_edge(V[2], V[4]) in mst)


func test_shortest_path_tree():
	var V = []
	for i in 4:
		V.push_back(graph.add_vertex(i))

	var _e
	_e = graph.add_edge(V[0], V[1], 2)
	_e = graph.add_edge(V[0], V[3], 5)
	_e = graph.add_edge(V[1], V[2], 1)
	_e = graph.add_edge(V[3], V[2], 3)

	var tree = graph.shortest_path_tree(V[0])
	assert_eq(tree.edges.size(), 3)

	for current in tree.backtrace:
		var previous = tree.backtrace[current]
		var distance = tree.distance[current]
		match current.value:
			0:
				assert_null(previous)
				assert_eq(distance, 0.0)
			1:
				assert_eq(previous.value, 0)
				assert_eq(distance, 2.0)
			2:
				assert_eq(previous.value, 1)
				assert_eq(distance, 3.0)
			3:
				assert_eq(previous.value, 0)
				assert_eq(distance, 5.0)

		if OS.is_stdout_verbose():
			print("Current: ", current.value,
				", Previous: ", previous.value if previous else "None",
				", Distance: ", distance)


func test_save_load():
	var path = "res://goost/core/types/graph.tres"

	# Save
	var a = graph.add_vertex("a")
	var b = graph.add_vertex("b")
	var c = graph.add_vertex("c")

	var ab = graph.add_edge(a, b, "ab")
	var ac = graph.add_directed_edge(a, c, "ac")
	var bc = graph.add_edge(b, c, "bc")

	assert_eq(a.value, "a")
	assert_eq(b.value, "b")
	assert_eq(c.value, "c")

	assert_eq(ab.a, a)
	assert_eq(ab.b, b)
	assert_eq(ab.value, "ab")
	assert_false(ab.is_directed())

	assert_eq(ac.a, a)
	assert_eq(ac.b, c)
	assert_eq(ac.value, "ac")
	assert_true(ac.is_directed())

	assert_eq(bc.a, b)
	assert_eq(bc.b, c)
	assert_eq(bc.value, "bc")
	assert_false(bc.is_directed())
	
	var _err = ResourceSaver.save(path, graph)
	assert_file_exists(path)

	# Load
	graph = load(path)

	a = graph.find_vertex("a")
	b = graph.find_vertex("b")
	c = graph.find_vertex("c")

	ab = graph.find_edge(a, b)
	ac = graph.find_edge(a, c)
	bc = graph.find_edge(b, c)

	assert_eq(a.value, "a")
	assert_eq(b.value, "b")
	assert_eq(c.value, "c")

	assert_eq(ab.a, a)
	assert_eq(ab.b, b)
	assert_eq(ab.value, "ab")
	assert_false(ab.is_directed())

	assert_eq(ac.a, a)
	assert_eq(ac.b, c)
	assert_eq(ac.value, "ac")
	assert_true(ac.is_directed())

	assert_eq(bc.a, b)
	assert_eq(bc.b, c)
	assert_eq(bc.value, "bc")
	assert_false(bc.is_directed())

	# Cleanup
	var dir = Directory.new()
	dir.remove(path)


class TestInvalidData extends "res://addons/gut/test.gd":
	var graph: Graph

	func before_all():
		Engine.print_error_messages = false

	func before_each():
		graph = Graph.new()

	func after_all():
		Engine.print_error_messages = true
		
	func test_add_edge():
		var v = ClassDB.instance("GraphVertex")
		var e = graph.add_edge(v, Array([]), Array([]))
		assert_null(e)
		v.free()
		e.free()
		
	func test_gdscript_iterator():
		var ret = graph._iter_next([])
		assert_eq(ret, false)
		graph.clear()
		ret = graph._iter_get([])
		
		var v = ClassDB.instance("GraphVertex")
		ret = v._iter_next([])
		assert_eq(ret, false)
		v.free()


class _TestPerformance extends "res://addons/gut/test.gd":

	func test_add_remove_dense():
		var graph = Graph.new()
		var rng = RandomNumberGenerator.new()
		rng.seed = 480789

		var count = 10

		var t1 = OS.get_ticks_msec()
		for i in count:
			var _v = graph.add_vertex(i)

		var vertices = graph.get_vertex_list()

		for i in 1000000:
			var ui = rng.randi() % count
			var vi = rng.randi() % count
			var _e = graph.add_edge(vertices[ui], vertices[vi])

		for i in count:
			var v = vertices[i]
			graph.remove_vertex(v)

		var t2 = OS.get_ticks_msec()
		gut.p(t2 - t1)


	func test_dfs():
		var graph = Graph.new()
		var rng = RandomNumberGenerator.new()
		rng.seed = 480789

		var count = 100000

		for i in count:
			var _v = graph.add_vertex(i)

		var vertices = graph.get_vertex_list()

		for i in count:
			var ui = rng.randi() % count
			var vi = rng.randi() % count
			var _e = graph.add_edge(vertices[ui], vertices[vi])

		var t1 = OS.get_ticks_msec()

		var component = graph.find_connected_component(vertices[0])
		assert_eq(component.size(), 79500)

		var t2 = OS.get_ticks_msec()
		gut.p(t2 - t1)
