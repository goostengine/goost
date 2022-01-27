#include "graph.h"

#ifdef DEBUG_ENABLED

#define ERR_INVALID_VERTICES(m_a, m_b) \
	ERR_FAIL_NULL(a);               \
	ERR_FAIL_NULL(b);               \
	ERR_FAIL_COND(!graph.has(a));   \
	ERR_FAIL_COND(!graph.has(b));

#define ERR_INVALID_VERTICES_V(m_a, m_b, m_ret) \
	ERR_FAIL_NULL_V(a, m_ret);               \
	ERR_FAIL_NULL_V(b, m_ret);               \
	ERR_FAIL_COND_V(!graph.has(a), m_ret);   \
	ERR_FAIL_COND_V(!graph.has(b), m_ret);   
#else

#define ERR_INVALID_VERTICES(m_a, m_b)
#define ERR_INVALID_VERTICES_V(m_a, m_b, m_ret)

#endif // DEBUG_ENABLED

GraphVertex *Graph::add_vertex(const Variant &p_value) {
	GraphVertex *v = memnew(GraphVertex);

	v->value = p_value;
	graph[v] = List<GraphEdge *>();

	return v;
}

GraphEdge *Graph::_add_edge(const Variant &p_a, const Variant &p_b, real_t p_weight, bool p_bidirectional) {
	ERR_FAIL_COND_V(p_a.get_type() == Variant::NIL, nullptr);
	ERR_FAIL_COND_V(p_b.get_type() == Variant::NIL, nullptr);

	GraphVertex *a;
	GraphVertex *b;

	if (p_a.get_type() == Variant::OBJECT) {
		a = Object::cast_to<GraphVertex>(p_a);
		ERR_FAIL_NULL_V(a, nullptr);
	} else {
		a = add_vertex(p_a);
	}
	if (p_b.get_type() == Variant::OBJECT) {
		b = Object::cast_to<GraphVertex>(p_b);
		ERR_FAIL_NULL_V(b, nullptr);
	} else {
		b = add_vertex(p_b);
	}
	GraphEdge *edge = memnew(GraphEdge);
	edge->a = a;
	edge->b = b;
	edge->weight = p_weight;
	edge->bidirectional = p_bidirectional;

	graph[a].push_back(edge);
	graph[b].push_back(edge);

	return edge;
}

GraphEdge *Graph::add_edge(const Variant &p_a, const Variant &p_b, real_t p_weight) {
	return _add_edge(p_a, p_b, p_weight, true);
}

GraphEdge *Graph::add_directed_edge(const Variant &p_a, const Variant &p_b, real_t p_weight) {
	return _add_edge(p_a, p_b, p_weight, false);
}

GraphEdge *Graph::find_edge(GraphVertex *a, GraphVertex *b) const {
	ERR_INVALID_VERTICES_V(a, b, nullptr);

	const List<GraphEdge *> &list = graph[a];
	for (const List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
		GraphEdge *edge = E->get();
		if (a == edge->a && b == edge->b) {
			return edge;
		}
		if (edge->bidirectional && a == edge->b && b == edge->a) {
			return edge;
		}
	}
	return nullptr;
}

void Graph::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_vertex", "value"), &Graph::add_vertex);

	ClassDB::bind_method(D_METHOD("add_edge", "a", "b", "weight"), &Graph::add_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("add_directed_edge", "a", "b", "weight"), &Graph::add_directed_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("find_edge", "a", "b"), &Graph::find_edge);
}

void GraphVertex::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_value", "value"), &GraphVertex::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &GraphVertex::get_value);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_value", "get_value");
}

void GraphEdge::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_vertex_a"), &GraphEdge::get_vertex_a);
	ClassDB::bind_method(D_METHOD("get_vertex_b"), &GraphEdge::get_vertex_b);

	ClassDB::bind_method(D_METHOD("set_weight", "weight"), &GraphEdge::set_weight);
	ClassDB::bind_method(D_METHOD("get_weight"), &GraphEdge::get_weight);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "vertex_a"), "", "get_vertex_a");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "vertex_b"), "", "get_vertex_b");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "weight"), "set_weight", "get_weight");
}
