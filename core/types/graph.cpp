#include "graph.h"

#include "core/set.h"

#ifdef DEBUG_ENABLED

#define ERR_INVALID_VERTEX(m_v) \
	ERR_FAIL_NULL(v);           \
	ERR_FAIL_COND(!graph->data.has(v));

#define ERR_INVALID_VERTEX_V(m_v, m_ret) \
	ERR_FAIL_NULL_V(v, m_ret);           \
	ERR_FAIL_COND_V(!graph->data.has(v), m_ret);

#define ERR_INVALID_VERTICES(m_a, m_b)  \
	ERR_FAIL_NULL(a);                   \
	ERR_FAIL_NULL(b);                   \
	ERR_FAIL_COND(!graph->data.has(a)); \
	ERR_FAIL_COND(!graph->data.has(b));

#define ERR_INVALID_VERTICES_V(m_a, m_b, m_ret)  \
	ERR_FAIL_NULL_V(a, m_ret);                   \
	ERR_FAIL_NULL_V(b, m_ret);                   \
	ERR_FAIL_COND_V(!graph->data.has(a), m_ret); \
	ERR_FAIL_COND_V(!graph->data.has(b), m_ret);
#else

#define ERR_INVALID_VERTEX(m_v)
#define ERR_INVALID_VERTEX_V(m_v, m_ret)
#define ERR_INVALID_VERTICES(m_a, m_b)
#define ERR_INVALID_VERTICES_V(m_a, m_b, m_ret)

#endif // DEBUG_ENABLED

GraphVertex *Graph::add_vertex(const Variant &p_value) {
	GraphVertex *v = memnew(GraphVertex);

	v->value = p_value;
	graph->data[v] = List<GraphEdge *>();
	v->graph = graph;

	return v;
}

void Graph::remove_vertex(GraphVertex *v) {
	ERR_INVALID_VERTEX(v);
	// Calls into GraphData::remove_vertex() during NOTIFICATION_PREDELETE
	memdelete(v);
}

void GraphData::remove_vertex(GraphVertex *v) {
	using EdgeItem = List<GraphEdge *>::Element *;
	using EdgeList = List<GraphEdge *> *;

	Map<EdgeItem, EdgeList> edges_to_remove;

	// Find all vertices connected to this one, including the vertex to be removed.
	Vector<GraphVertex *> closed_neighborhood;
	closed_neighborhood.push_back(v);

	List<GraphEdge *> &list_v = data[v];
	for (EdgeItem E = list_v.front(); E; E = E->next()) {
		GraphEdge *edge = E->get();
		if (v == edge->a) {
			closed_neighborhood.push_back(edge->b);
		} else {
			closed_neighborhood.push_back(edge->a);
		}
	}
	// Find edges that are associated with the vertices above.
	for (int i = 0; i < closed_neighborhood.size(); ++i) {
		GraphVertex *n = closed_neighborhood[i];
		List<GraphEdge *> &list_n = data[n];

		for (EdgeItem E = list_n.front(); E; E = E->next()) {
			GraphEdge *edge = E->get();
			if (v == edge->a || v == edge->b) {
				edges_to_remove[E] = &list_n;
			}
		}
	}
	// Delete all edges associated with the vertex.
	Set<GraphEdge *> edges_to_delete;

	for (Map<EdgeItem, EdgeList>::Element *E = edges_to_remove.front(); E; E = E->next()) {
		EdgeItem item = E->key();
		GraphEdge *edge = item->get();
		edges_to_delete.insert(edge);

		EdgeList list = E->get();
		list->erase(item);
	}
	for (Set<GraphEdge *>::Element *E = edges_to_delete.front(); E; E = E->next()) {
		memdelete(E->get());
	}
	v->graph = nullptr;
	bool erased = data.erase(v);
	ERR_FAIL_COND(!erased);
}

Array Graph::get_neighbors(GraphVertex *v) {
	ERR_INVALID_VERTEX_V(v, Array());

	Array neighborhood;
	const List<GraphEdge *> &list = graph->data[v];

	for (const List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
		GraphEdge *edge = E->get();
		if (edge->bidirectional) {
			if (v == edge->a) {
				neighborhood.push_back(edge->b);
			} else {
				neighborhood.push_back(edge->a);
			}
		}
	}
	return neighborhood;
}

Array Graph::get_successors(GraphVertex *v) {
	ERR_INVALID_VERTEX_V(v, Array());

	Array successors;
	const List<GraphEdge *> &list = graph->data[v];

	for (const List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
		GraphEdge *edge = E->get();
		if (edge->bidirectional) {
			continue;
		}
		if (v == edge->a) {
			successors.push_back(edge->b);
		}
	}
	return successors;
}

Array Graph::get_predecessors(GraphVertex *v) {
	ERR_INVALID_VERTEX_V(v, Array());

	Array predecessors;
	const List<GraphEdge *> &list = graph->data[v];

	for (const List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
		GraphEdge *edge = E->get();
		if (edge->bidirectional) {
			continue;
		}
		if (v == edge->b) {
			predecessors.push_back(edge->a);
		}
	}
	return predecessors;
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

	graph->data[a].push_back(edge);
	graph->data[b].push_back(edge);
	edge->graph = graph;

	return edge;
}

GraphEdge *Graph::add_edge(const Variant &p_a, const Variant &p_b, real_t p_weight) {
	return _add_edge(p_a, p_b, p_weight, true);
}

void GraphData::remove_edge(GraphEdge *e) {
	e->graph = nullptr;
}

GraphEdge *Graph::add_directed_edge(const Variant &p_a, const Variant &p_b, real_t p_weight) {
	return _add_edge(p_a, p_b, p_weight, false);
}

GraphEdge *Graph::find_edge(GraphVertex *a, GraphVertex *b) const {
	ERR_INVALID_VERTICES_V(a, b, nullptr);

	const List<GraphEdge *> &list = graph->data[a];
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

Array Graph::get_edge_list(GraphVertex *a, GraphVertex *b) const {
	Array edge_list;

	Set<GraphEdge *> edge_set;

	if (!a && !b) {
		// Get all edges in the graph.
		for (Map<GraphVertex *, List<GraphEdge *>>::Element *E = graph->data.front(); E; E = E->next()) {
			const List<GraphEdge *> &list = E->get();
			for (const List<GraphEdge *>::Element *I = list.front(); I; I = I->next()) {
				edge_set.insert(I->get());
			}
		}
	} else {
		// Get all edges between vertices.
		for (Map<GraphVertex *, List<GraphEdge *>>::Element *E = graph->data.front(); E; E = E->next()) {
			const List<GraphEdge *> &list = E->get();
			for (const List<GraphEdge *>::Element *I = list.front(); I; I = I->next()) {
				GraphEdge *edge = I->get();
				if (a == edge->a && b == edge->b) {
					edge_set.insert(edge);
				}
				if (edge->bidirectional && a == edge->b && b == edge->a) {
					edge_set.insert(edge);
				}
			}
		}
	}
	for (const Set<GraphEdge *>::Element *I = edge_set.front(); I; I = I->next()) {
		edge_list.push_back(I->get());
	}
	return edge_list;
}

int Graph::get_edge_count() const {
	Set<GraphEdge *> edge_set;

	for (const Map<GraphVertex *, List<GraphEdge *>>::Element *E = graph->data.front(); E; E = E->next()) {
		const List<GraphEdge *> &list = E->get();
		for (const List<GraphEdge *>::Element *I = list.front(); I; I = I->next()) {
			edge_set.insert(I->get());
		}
	}
	return edge_set.size();
}

void Graph::clear() {
	Vector<GraphVertex *> to_remove;
	for (Map<GraphVertex *, List<GraphEdge *>>::Element *E = graph->data.front(); E; E = E->next()) {
		to_remove.push_back(E->key());
	}
	for (int i = 0; i < to_remove.size(); ++i) {
		memdelete(to_remove[i]); // This will also delete associated edges.
	}
}

void Graph::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_vertex", "value"), &Graph::add_vertex);
	ClassDB::bind_method(D_METHOD("remove_vertex", "vertex"), &Graph::remove_vertex);
	ClassDB::bind_method(D_METHOD("has_vertex", "vertex"), &Graph::has_vertex);
	ClassDB::bind_method(D_METHOD("get_vertex_count"), &Graph::get_vertex_count);

	ClassDB::bind_method(D_METHOD("get_neighbors", "vertex"), &Graph::get_neighbors);
	ClassDB::bind_method(D_METHOD("get_successors", "vertex"), &Graph::get_successors);
	ClassDB::bind_method(D_METHOD("get_predecessors", "vertex"), &Graph::get_predecessors);

	ClassDB::bind_method(D_METHOD("add_edge", "a", "b", "weight"), &Graph::add_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("add_directed_edge", "from", "to", "weight"), &Graph::add_directed_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("find_edge", "a", "b"), &Graph::find_edge);
	ClassDB::bind_method(D_METHOD("get_edge_list", "a", "b"), &Graph::get_edge_list, DEFVAL(Variant()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("get_edge_count"), &Graph::get_edge_count);
}

Graph::Graph() {
	graph = memnew(GraphData);
}

Graph::~Graph() {
	clear();
	if (graph) {
		memdelete(graph);
	}
}

void GraphVertex::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PREDELETE: {
			if (graph) {
				graph->remove_vertex(this);
			}
		} break;
	}
}

void GraphVertex::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_value", "value"), &GraphVertex::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &GraphVertex::get_value);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_value", "get_value");
}

void GraphEdge::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PREDELETE: {
			if (graph) {
				graph->remove_edge(this);
			}
		} break;
	}
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
