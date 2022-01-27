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
