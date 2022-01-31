extends "res://addons/gut/test.gd"

var graph: Graph

func before_each():
	graph = Graph.new()

    
func test_add_vertex():
    var a = graph.add_vertex("a")
    var b = graph.add_vertex("b")

    assert_eq(a.value, "a")
    assert_eq(b.value, "b")


func test_add_directed_edge():
    var a = graph.add_vertex("a")
    var b = graph.add_vertex("b")

    var e1 = graph.add_directed_edge(a, b)

    assert_eq(graph.find_edge(a, b), e1)
    assert_null(graph.find_edge(b, a), e1)

    var e2 = graph.add_directed_edge("c", "d")

    assert_eq(graph.find_edge(e2.vertex_a, e2.vertex_b), e2)
    assert_null(graph.find_edge(e2.vertex_b, e2.vertex_a), e2)


func test_add_edge():
    var a = graph.add_vertex("a")
    var b = graph.add_vertex("b")

    var e1 = graph.add_edge(a, b)

    assert_eq(graph.find_edge(a, b), e1)
    assert_eq(graph.find_edge(b, a), e1)

    var e2 = graph.add_edge("c", "d")

    assert_eq(graph.find_edge(e2.vertex_a, e2.vertex_b), e2)
    assert_eq(graph.find_edge(e2.vertex_b, e2.vertex_a), e2)


func test_neighborhood():
    var a = graph.add_vertex("a")
    var b = graph.add_vertex("b")
    var c = graph.add_vertex("c")

    var _e1 = graph.add_edge(a, b)
    var _e2 = graph.add_edge(a, c)
    var _e3 = graph.add_edge(b, c)

    var na = graph.get_neighbors(a)
    var nb = graph.get_neighbors(b)
    var nc = graph.get_neighbors(c)

    assert_eq(na.size(), 2)
    assert_true(b in na)
    assert_true(c in na)

    assert_eq(nb.size(), 2)
    assert_true(a in nb)
    assert_true(c in nb)

    assert_eq(nc.size(), 2)
    assert_true(a in nc)
    assert_true(b in nc)


func test_neighborhood_directed():
    var a = graph.add_vertex("a")
    var b = graph.add_vertex("b")
    var c = graph.add_vertex("c")

    var _e1 = graph.add_edge(a, b)
    var _e2 = graph.add_directed_edge(a, c)

    var na = graph.get_neighbors(a)
    var sa = graph.get_successors(a)
    var pc = graph.get_predecessors(c)

    assert_eq(na.size(), 2)
    assert_true(b in na)
    assert_true(c in na)

    assert_eq(sa.size(), 1)
    assert_true(c in sa)

    assert_eq(pc.size(), 1)
    assert_true(a in pc)

    var _e3 = graph.add_directed_edge(c, a)
    var _e4 = graph.add_directed_edge(c, b)
    var sc = graph.get_successors(c)

    assert_eq(sc.size(), 2)
    assert_true(a in sc)
    assert_true(b in sc)


func test_edge_list():
    var a = graph.add_vertex("a")
    var b = graph.add_vertex("b")
    var c = graph.add_vertex("c")

    var ab1 = graph.add_edge(a, b)
    var ab2 = graph.add_edge(a, b)
    var ac = graph.add_edge(a, c)
    var bc = graph.add_edge(b, c)

    var edges = graph.get_edge_list(a, b)
    assert_eq(edges.size(), 2)
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
    assert_eq(graph.get_neighbors(b).size(), 0)
    assert_eq(graph.get_neighbors(c).size(), 0)
    assert_eq(graph.get_neighbors(d).size(), 0)


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

    graph.remove_vertex(a)

    assert_eq(graph.get_vertex_count(), 3)

    assert_eq(graph.get_edge_count(), 3)
    assert_eq(graph.get_neighbors(c).size(), 2)
    assert_eq(graph.get_neighbors(b).size(), 2)
    assert_eq(graph.get_neighbors(d).size(), 2)

    graph.remove_vertex(b)

    assert_eq(graph.get_edge_count(), 1)
    assert_eq(graph.get_neighbors(c).size(), 1)
    assert_eq(graph.get_neighbors(d).size(), 1)

    graph.remove_vertex(c)

    assert_eq(graph.get_edge_count(), 0)
    assert_eq(graph.get_neighbors(d).size(), 0)


func test_clear_edges():
    var a = graph.add_vertex("a")
    var b = graph.add_vertex("b")
    var c = graph.add_vertex("c")

    var _ab = graph.add_edge(a, b)
    var _ac = graph.add_edge(a, c)
    var _bc = graph.add_edge(b, c)

    assert_eq(graph.get_vertex_count(), 3)
    assert_eq(graph.get_edge_count(), 3)
    assert_eq(graph.get_neighbors(a).size(), 2)
    assert_eq(graph.get_neighbors(b).size(), 2)
    assert_eq(graph.get_neighbors(c).size(), 2)

    graph.clear_edges()

    assert_eq(graph.get_vertex_count(), 3)
    assert_eq(graph.get_edge_count(), 0)
    assert_eq(graph.get_neighbors(a).size(), 0)
    assert_eq(graph.get_neighbors(b).size(), 0)
    assert_eq(graph.get_neighbors(c).size(), 0)

    
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
    var a = graph.add_vertex("a")
    var b = graph.add_vertex("b")
    var c = graph.add_vertex("c")

    var ab = graph.add_edge(a, b)
    var ac = graph.add_edge(a, c)
    var bc = graph.add_edge(b, c)

    assert_eq(graph.get_edge_count(), 3)

    graph.remove_edge(ab)
    assert_eq(graph.get_edge_count(), 2)

    graph.remove_edge(ac)
    assert_eq(graph.get_edge_count(), 1)

    graph.remove_edge(bc)
    assert_eq(graph.get_edge_count(), 0)

    assert_eq(graph.get_vertex_count(), 3)


func test_self_loop():
    var a = graph.add_vertex("a")
    var b = graph.add_vertex("b")

    graph.add_edge(a, a)
    graph.add_edge(b, a)

    assert_eq(graph.get_neighbors(a)[0], a)

    graph.remove_vertex(a)
    graph.remove_vertex(b)
    # Should not crash...


# func test_perf():
#     var rng = RandomNumberGenerator.new()
#     rng.seed = 480789
    
#     var count = 100000

#     var t1 = OS.get_ticks_msec()
#     for i in count:
#         graph.add_vertex(i)

#     var vertices = graph.get_vertex_list()

#     for i in count:
#         var ui = rng.randi() % count
#         var vi = rng.randi() % count
#         graph.add_edge(vertices[ui], vertices[vi])

#     for i in count:
#         var v = vertices[i]
#         graph.remove_vertex(v)

#     var t2 = OS.get_ticks_msec()
#     gut.p(t2 - t1)
