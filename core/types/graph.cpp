#include "graph.h"

using EdgeKey = Pair<uint32_t, uint32_t>;

void GraphData::remove_vertex(GraphVertex *p_vertex) {
	Vector<GraphEdge *> edges_to_delete;

	const uint32_t *n = nullptr;
	while ((n = p_vertex->neighbors.next(n))) {
		GraphVertex *n_vertex = p_vertex->neighbors[*n];

		List<GraphEdge *> &list = get_edges(p_vertex->id, n_vertex->id);

		for (List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
			// Removing edges updates neighbors as well, but doing so here
			// will lead to unexpected result, so defer deletion.
			edges_to_delete.push_back(E->get());
		}
		if (p_vertex != n_vertex) {
			// Handle self-loops, there's no point to remove neighbors to self,
			// since the vertex is going to be deleted anyway.
			n_vertex->neighbors.erase(p_vertex->id);
		}
	}
	for (int i = 0; i < edges_to_delete.size(); ++i) {
		memdelete(edges_to_delete[i]);
	}
	// Remove the vertex itself from the graph.
	vertices.erase(p_vertex->id);
	p_vertex->graph = nullptr;
}

void GraphData::remove_edge(GraphEdge *p_edge) {
	GraphVertex *a = p_edge->a;
	GraphVertex *b = p_edge->b;

	List<GraphEdge *> &list = get_edges(a->id, b->id);

	for (List<GraphEdge *>::Element *E = list.front(); E;) {
		auto N = E->next();
		if (p_edge == E->get()) {
			list.erase(E);
		}
		E = N;
	}
	a->neighbors.erase(b->id);
	b->neighbors.erase(a->id);

	if (list.empty()) {
		edges.erase(EdgeKey(a->id, b->id));
	}
}

void Graph::remove_edge(GraphEdge *p_edge) {
	// Calls into GraphData::remove_edge() during NOTIFICATION_PREDELETE
	memdelete(p_edge);
}

GraphVertex *Graph::add_vertex(const Variant &p_value) {
	GraphVertex *v = memnew(GraphVertex);

	v->id = next_vertex_id++;
	v->value = p_value;

	graph->vertices[v->id] = v;
	v->graph = graph;

	return v;
}

void Graph::remove_vertex(GraphVertex *p_vertex) {
	// Calls into GraphData::remove_vertex() during NOTIFICATION_PREDELETE
	memdelete(p_vertex);
}

bool Graph::has_vertex(GraphVertex *p_vertex) const {
	return graph->vertices.has(p_vertex->id);
}

Array Graph::get_vertex_list() const {
	Array vertex_list;

	const uint32_t *k = nullptr;
	while ((k = graph->vertices.next(k))) {
		vertex_list.push_back(graph->vertices[*k]);
	}
	return vertex_list;
}

Array Graph::get_neighbors(GraphVertex *p_vertex) {
	Array neighbors;

	const uint32_t *n = nullptr;
	while ((n = p_vertex->neighbors.next(n))) {
		neighbors.push_back(p_vertex->neighbors[*n]);
	}
	return neighbors;
}

Array Graph::get_successors(GraphVertex *p_vertex) {
	Array successors;

	const uint32_t *n = nullptr;
	while ((n = p_vertex->neighbors.next(n))) {
		GraphVertex *a = p_vertex;
		GraphVertex *b = p_vertex->neighbors[*n];

		const List<GraphEdge *> &list = graph->get_edges(a->id, b->id);

		for (const List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
			GraphEdge *edge = E->get();
			if (!edge->directed) {
				continue;
			}
			if (p_vertex == edge->a) {
				successors.push_back(edge->b);
				// There may be multiple edges connected from the same vertex.
				break;
			}
		}
	}
	return successors;
}

Array Graph::get_predecessors(GraphVertex *p_vertex) {
	Array predecessors;

	const uint32_t *n = nullptr;
	while ((n = p_vertex->neighbors.next(n))) {
		GraphVertex *a = p_vertex;
		GraphVertex *b = p_vertex->neighbors[*n];

		const List<GraphEdge *> &list = graph->get_edges(a->id, b->id);

		for (const List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
			GraphEdge *edge = E->get();
			if (!edge->directed) {
				continue;
			}
			if (p_vertex == edge->b) {
				predecessors.push_back(edge->a);
				// There may be multiple edges connected from the same vertex.
				break;
			}
		}
	}
	return predecessors;
}

GraphEdge *Graph::_add_edge(const Variant &p_a, const Variant &p_b, const Variant &p_value, bool p_directed) {
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
	a->neighbors[b->id] = b;
	b->neighbors[a->id] = a;

	GraphEdge *edge = memnew(GraphEdge);
	edge->a = a;
	edge->b = b;
	edge->value = p_value;
	edge->directed = p_directed;
	edge->id = next_edge_id++;
	edge->graph = graph;

	auto &key = EdgeKey(a->id, b->id);
	if (!graph->edges.has(key)) {
		List<GraphEdge *> list;
		list.push_back(edge);
		graph->edges[key] = list;
	} else {
		List<GraphEdge *> &list = graph->edges[key];
		list.push_back(edge);
	}
	return edge;
}

GraphEdge *Graph::add_edge(const Variant &p_a, const Variant &p_b, const Variant &p_value) {
	return _add_edge(p_a, p_b, p_value, false);
}

GraphEdge *Graph::add_directed_edge(const Variant &p_a, const Variant &p_b, const Variant &p_value) {
	return _add_edge(p_a, p_b, p_value, true);
}

List<GraphEdge *> &GraphData::get_edges(uint32_t a_id, uint32_t b_id) {
	return edges[EdgeKey(a_id, b_id)];
}

GraphEdge *Graph::find_edge(GraphVertex *p_a, GraphVertex *p_b) const {
	const List<GraphEdge *> &list = graph->get_edges(p_a->id, p_b->id);

	for (const List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
		GraphEdge *edge = E->get();
		if (p_a == edge->a && p_b == edge->b) {
			return edge;
		}
		if (!edge->directed && p_a == edge->b && p_b == edge->a) {
			return edge;
		}
	}
	return nullptr;
}

bool Graph::has_edge(GraphVertex *p_a, GraphVertex *p_b) const {
	return find_edge(p_a, p_b) != nullptr;
}

Array Graph::get_edge_list(GraphVertex *p_a, GraphVertex *p_b) const {
	Array edge_list;

	const bool all = !p_a && !p_b;

	const EdgeKey *k = nullptr;
	while ((k = graph->edges.next(k))) {
		List<GraphEdge *> &list = graph->edges[*k];

		for (List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
			GraphEdge *edge = E->get();
			if (all) { // Get all edges in the graph.
				edge_list.push_back(edge);
			} else { // Get all edges between vertices.
				if (p_a == edge->a && p_b == edge->b) {
					edge_list.push_back(edge);
				} else if (!edge->directed && p_a == edge->b && p_b == edge->a) {
					edge_list.push_back(edge);
				}
			}
		}
	}
	return edge_list;
}

void Graph::clear() {
	Vector<GraphVertex *> to_delete;

	const uint32_t *k = nullptr;
	while ((k = graph->vertices.next(k))) {
		to_delete.push_back(graph->vertices[*k]);
	}
	for (int i = 0; i < to_delete.size(); ++i) {
		// This will also delete edges, see GraphData::remove_vertex()
		memdelete(to_delete[i]);
	}
	graph->vertices.clear();
	graph->edges.clear();
}

void Graph::clear_edges() {
	Vector<GraphEdge *> to_delete;

	const EdgeKey *k = nullptr;
	while ((k = graph->edges.next(k))) {
		List<GraphEdge *> &list = graph->edges[*k];

		for (List<GraphEdge *>::Element *E = list.front(); E; E = E->next()) {
			to_delete.push_back(E->get());
		}
	}
	for (int i = 0; i < to_delete.size(); ++i) {
		memdelete(to_delete[i]);
	}
	graph->edges.clear();
}

void Graph::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_vertex", "value"), &Graph::add_vertex);
	ClassDB::bind_method(D_METHOD("remove_vertex", "vertex"), &Graph::remove_vertex);
	ClassDB::bind_method(D_METHOD("has_vertex", "vertex"), &Graph::has_vertex);
	ClassDB::bind_method(D_METHOD("get_vertex_list"), &Graph::get_vertex_list);
	ClassDB::bind_method(D_METHOD("get_vertex_count"), &Graph::get_vertex_count);

	ClassDB::bind_method(D_METHOD("get_neighbors", "vertex"), &Graph::get_neighbors);
	ClassDB::bind_method(D_METHOD("get_successors", "vertex"), &Graph::get_successors);
	ClassDB::bind_method(D_METHOD("get_predecessors", "vertex"), &Graph::get_predecessors);

	ClassDB::bind_method(D_METHOD("add_edge", "a", "b", "value"), &Graph::add_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("add_directed_edge", "from", "to", "value"), &Graph::add_directed_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("remove_edge", "edge"), &Graph::remove_edge);
	ClassDB::bind_method(D_METHOD("find_edge", "a", "b"), &Graph::find_edge);
	ClassDB::bind_method(D_METHOD("has_edge", "a", "b"), &Graph::has_edge);
	ClassDB::bind_method(D_METHOD("get_edge_list", "a", "b"), &Graph::get_edge_list, DEFVAL(Variant()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("get_edge_count"), &Graph::get_edge_count);

	ClassDB::bind_method(D_METHOD("clear"), &Graph::clear);
	ClassDB::bind_method(D_METHOD("clear_edges"), &Graph::clear_edges);
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
	ClassDB::bind_method(D_METHOD("get_a"), &GraphEdge::get_a);
	ClassDB::bind_method(D_METHOD("get_b"), &GraphEdge::get_b);

	ClassDB::bind_method(D_METHOD("is_loop"), &GraphEdge::is_loop);
	ClassDB::bind_method(D_METHOD("is_directed"), &GraphEdge::is_directed);

	ClassDB::bind_method(D_METHOD("set_value", "value"), &GraphEdge::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &GraphEdge::get_value);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "a"), "", "get_a");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "b"), "", "get_b");
	ADD_PROPERTY(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_value", "get_value");
}
