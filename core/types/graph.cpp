#include "graph.h"

#include "core/script_language.h"
#include "core/string_names.h"

#include "core/math/disjoint_set.h"

using EdgeKey = Pair<uint32_t, uint32_t>;
using EdgeList = LocalVector<GraphEdge *, int>;

void GraphData::remove_vertex(GraphVertex *p_vertex) {
	EdgeList edges_to_delete;

	const uint32_t *n = nullptr;
	while ((n = p_vertex->neighbors.next(n))) {
		GraphVertex *n_vertex = p_vertex->neighbors[*n];

		EdgeList &list = get_edges(p_vertex->id, n_vertex->id);

		for (int i = 0; i < list.size(); ++i) {
			// Removing edges updates neighbors as well, but doing so here
			// will lead to unexpected result, so defer deletion.
			edges_to_delete.push_back(list[i]);
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
	GraphVertex *&a = p_edge->a;
	GraphVertex *&b = p_edge->b;

	EdgeList &list = get_edges(a->id, b->id);

	for (int i = 0; i < list.size(); ++i) {
		if (p_edge == list[i]) {
			a->neighbors.erase(b->id);
			b->neighbors.erase(a->id);
			list.remove_unordered(i);
			break;
		}
	}
	if (list.empty()) {
		edges.erase(EdgeKey(a->id, b->id));
	}
}

void Graph::remove_edge(GraphEdge *p_edge) {
	// Calls into GraphData::remove_edge() during NOTIFICATION_PREDELETE
	memdelete(p_edge);
}

GraphVertex *Graph::_create_vertex() {
	GraphVertex *vertex = nullptr;
	if (get_script_instance() && get_script_instance()->has_method(StringNames::get_singleton()->_create_vertex)) {
		Object *obj = get_script_instance()->call(StringNames::get_singleton()->_create_vertex);
		vertex = Object::cast_to<GraphVertex>(obj);
		ERR_FAIL_NULL_V_MSG(vertex, nullptr, "The returned object is not a valid GraphVertex");
	} else {
		vertex = memnew(GraphVertex);
	}
	return vertex;
}

GraphEdge *Graph::_create_edge() {
	GraphEdge *edge = nullptr;
	if (get_script_instance() && get_script_instance()->has_method(StringNames::get_singleton()->_create_edge)) {
		Object *obj = get_script_instance()->call(StringNames::get_singleton()->_create_edge);
		edge = Object::cast_to<GraphEdge>(obj);
		ERR_FAIL_NULL_V_MSG(edge, nullptr, "The returned object is not a valid GraphEdge");
	} else {
		edge = memnew(GraphEdge);
	}
	return edge;
}

GraphVertex *Graph::add_vertex(const Variant &p_value) {
	GraphVertex *v = _create_vertex();

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

GraphVertex *Graph::find_vertex(const Variant &p_value) {
	const uint32_t *k = nullptr;
	while ((k = graph->vertices.next(k))) {
		GraphVertex *v = graph->vertices[*k];
		if (v->value == p_value) {
			return v;
		}
	}
	return nullptr;
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

Array GraphVertex::get_neighbors() const {
	Array ret;

	const uint32_t *n = nullptr;
	while ((n = neighbors.next(n))) {
		ret.push_back(neighbors[*n]);
	}
	return ret;
}

Array GraphVertex::get_successors() const {
	Array ret;

	const uint32_t *n = nullptr;
	while ((n = neighbors.next(n))) {
		const GraphVertex *a = this;
		const GraphVertex *b = neighbors[*n];

		const EdgeList &list = graph->get_edges(a->id, b->id);

		for (int i = 0; i < list.size(); ++i) {
			GraphEdge *edge = list[i];
			if (!edge->is_directed()) {
				continue;
			}
			if (edge->get_a() == this) {
				ret.push_back(edge->get_b());
				// There may be multiple edges connected from the same vertex.
				break;
			}
		}
	}
	return ret;
}

int GraphVertex::get_successor_count() const {
	return get_successors().size(); // TODO: can be optimized.
}

Array GraphVertex::get_predecessors() const {
	Array ret;

	const uint32_t *n = nullptr;
	while ((n = neighbors.next(n))) {
		const GraphVertex *a = this;
		const GraphVertex *b = neighbors[*n];

		const EdgeList &list = graph->get_edges(a->id, b->id);

		for (int i = 0; i < list.size(); ++i) {
			GraphEdge *edge = list[i];
			if (!edge->is_directed()) {
				continue;
			}
			if (edge->get_b() == this) {
				ret.push_back(edge->get_a());
				// There may be multiple edges connected from the same vertex.
				break;
			}
		}
	}
	return ret;
}

int GraphVertex::get_predecessor_count() const {
	return get_predecessors().size(); // TODO: can be optimized.
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

	GraphEdge *edge = _create_edge();
	edge->a = a;
	edge->b = b;
	edge->value = p_value;
	edge->directed = p_directed;
	edge->id = next_edge_id++;
	edge->graph = graph;

	const auto &key = EdgeKey(a->id, b->id);
	if (!graph->edges.has(key)) {
		EdgeList list;
		list.reserve(4); // Slight optimization for directed or multi-edges.
		list.push_back(edge);
		graph->edges[key] = list;
	} else {
		EdgeList &list = graph->edges[key];
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

EdgeList &GraphData::get_edges(uint32_t a_id, uint32_t b_id) {
	return edges[EdgeKey(a_id, b_id)];
}

GraphEdge *Graph::find_edge(GraphVertex *p_a, GraphVertex *p_b) const {
	const EdgeList &list = graph->get_edges(p_a->id, p_b->id);

	for (int i = 0; i < list.size(); ++i) {
		GraphEdge *edge = list[i];
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
		EdgeList &list = graph->edges[*k];

		for (int i = 0; i < list.size(); ++i) {
			GraphEdge *edge = list[i];
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

Array Graph::find_connected_component(GraphVertex *p_vertex) {
	Array component;

	for (V->initialize(p_vertex); V->has_next();) {
		component.push_back(V->next());
	}
	return component;
}

bool Graph::is_strongly_connected() {
	if (graph->vertices.size() <= 1) {
		return true;
	}
	if (graph->edges.size() < graph->vertices.size() - 1) {
		return false;
	}
	// Any vertex will do.
	const uint32_t *k = nullptr;
	k = graph->vertices.next(k);
	GraphVertex *root = graph->vertices[*k];

	uint32_t count = 0;
	for (V->initialize(root); V->has_next(); V->next()) {
		count++;
	}
	return count == graph->vertices.size();
}

Dictionary Graph::get_connected_components() {
	Dictionary components;

	DisjointSet<GraphVertex *> set;
	{
		const uint32_t *k = nullptr;
		while ((k = graph->vertices.next(k))) {
			set.insert(graph->vertices[*k]);
		}
	}
	{
		const EdgeKey *k = nullptr;
		while ((k = graph->edges.next(k))) {
			EdgeList &list = graph->edges[*k];
			for (int i = 0; i < list.size(); ++i) {
				set.create_union(list[i]->a, list[i]->b);
			}
		}
	}
	Vector<GraphVertex *> roots;
	set.get_representatives(roots);

	for (int i = 0; i < roots.size(); ++i) {
		GraphVertex *r = roots[i];
		Vector<GraphVertex *> members;
		set.get_members(members, r);

		Array member_array;
		for (int j = 0; j < members.size(); ++j) {
			member_array.push_back(members[j]);
		}
		components[r] = member_array;
	}
	return components;
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
	EdgeList to_delete;

	const EdgeKey *k = nullptr;
	while ((k = graph->edges.next(k))) {
		EdgeList &list = graph->edges[*k];

		for (int i = 0; i < list.size(); ++i) {
			to_delete.push_back(list[i]);
		}
	}
	for (int i = 0; i < to_delete.size(); ++i) {
		memdelete(to_delete[i]);
	}
	graph->edges.clear();
}

void Graph::set_iterator(const Ref<GraphIterator> &p_iterator) {
	if (p_iterator.is_valid()) {
		V = p_iterator;
	} else {
		V = default_iterator;
	}
}

void Graph::_bind_methods() {
	BIND_VMETHOD(MethodInfo(Variant::OBJECT, "_create_vertex"));
	BIND_VMETHOD(MethodInfo(Variant::OBJECT, "_create_edge"));

	ClassDB::bind_method(D_METHOD("add_vertex", "value"), &Graph::add_vertex);
	ClassDB::bind_method(D_METHOD("remove_vertex", "vertex"), &Graph::remove_vertex);
	ClassDB::bind_method(D_METHOD("find_vertex", "value"), &Graph::find_vertex);
	ClassDB::bind_method(D_METHOD("has_vertex", "vertex"), &Graph::has_vertex);
	ClassDB::bind_method(D_METHOD("get_vertex_list"), &Graph::get_vertex_list);
	ClassDB::bind_method(D_METHOD("get_vertex_count"), &Graph::get_vertex_count);

	ClassDB::bind_method(D_METHOD("add_edge", "a", "b", "value"), &Graph::add_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("add_directed_edge", "from", "to", "value"), &Graph::add_directed_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("remove_edge", "edge"), &Graph::remove_edge);
	ClassDB::bind_method(D_METHOD("find_edge", "a", "b"), &Graph::find_edge);
	ClassDB::bind_method(D_METHOD("has_edge", "a", "b"), &Graph::has_edge);
	ClassDB::bind_method(D_METHOD("get_edge_list", "a", "b"), &Graph::get_edge_list, DEFVAL(Variant()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("get_edge_count"), &Graph::get_edge_count);

	ClassDB::bind_method(D_METHOD("find_connected_component", "vertex"), &Graph::find_connected_component);
	ClassDB::bind_method(D_METHOD("get_connected_components"), &Graph::get_connected_components);
	ClassDB::bind_method(D_METHOD("is_strongly_connected"), &Graph::is_strongly_connected);

	ClassDB::bind_method(D_METHOD("clear"), &Graph::clear);
	ClassDB::bind_method(D_METHOD("clear_edges"), &Graph::clear_edges);

	ClassDB::bind_method(D_METHOD("set_iterator", "iterator"), &Graph::set_iterator);
	ClassDB::bind_method(D_METHOD("get_iterator"), &Graph::get_iterator);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "iterator"), "set_iterator", "get_iterator");
}

Graph::Graph() {
	graph = memnew(GraphData);
	// Setup default depth-first search iterator.
	Ref<GraphIteratorDFS> dfs;
	dfs.instance();
	default_iterator = dfs;
	V = default_iterator;
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
	ClassDB::bind_method(D_METHOD("get_neighbors"), &GraphVertex::get_neighbors);
	ClassDB::bind_method(D_METHOD("get_neighbor_count"), &GraphVertex::get_neighbor_count);

	ClassDB::bind_method(D_METHOD("get_successors"), &GraphVertex::get_successors);
	ClassDB::bind_method(D_METHOD("get_successor_count"), &GraphVertex::get_successor_count);

	ClassDB::bind_method(D_METHOD("get_predecessors"), &GraphVertex::get_predecessors);
	ClassDB::bind_method(D_METHOD("get_predecessor_count"), &GraphVertex::get_predecessor_count);

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

void GraphIterator::_bind_methods() {
	BIND_VMETHOD(MethodInfo(Variant::NIL, "initialize", PropertyInfo(Variant::OBJECT, "root")));
	BIND_VMETHOD(MethodInfo(Variant::BOOL, "has_next"));
	BIND_VMETHOD(MethodInfo(Variant::OBJECT, "next"));
}

void GraphIterator::initialize(GraphVertex *p_root) {
	ERR_FAIL_NULL(p_root);

	auto s = get_script_instance();
	if (s && s->has_method(StringNames::get_singleton()->initialize)) {
		Variant root = p_root;
		s->call(StringNames::get_singleton()->initialize, root);
	} else {
		root = p_root;
	}
}

bool GraphIterator::has_next() const {
	auto s = get_script_instance();
	if (s && s->has_method(StringNames::get_singleton()->has_next)) {
		return s->call(StringNames::get_singleton()->has_next);
	}
	return false;
}

GraphVertex *GraphIterator::next() {
	auto s = get_script_instance();
	if (s && s->has_method(StringNames::get_singleton()->next)) {
		Object *obj = s->call(StringNames::get_singleton()->next);
		GraphVertex *v = Object::cast_to<GraphVertex>(obj);
		return v;
	}
	return nullptr;
}

void GraphIteratorDFS::initialize(GraphVertex *p_root) {
	ERR_FAIL_NULL(p_root);

	graph = p_root->graph;

	stack.clear();
	visited.clear();

	stack.push(Element(p_root));
	visited.insert(p_root);

	next_vertex = p_root;
}

bool GraphIteratorDFS::has_next() const {
	return next_vertex != nullptr;
}

GraphVertex *GraphIteratorDFS::next() {
	GraphVertex *n = next_vertex;
	advance();
	return n;
}

void GraphIteratorDFS::advance() {
	next_vertex = nullptr;

	while (!stack.is_empty()) {
		Element &v = stack.pop();

		if (v.neighbor) {
			GraphVertex *child = v.parent->neighbors[*v.neighbor];
			v.neighbor = v.parent->neighbors.next(v.neighbor);
			stack.push(v);

			if (!visited.has(child)) {
				visited.insert(child);
				stack.push(Element(child));
				next_vertex = child;
				break;
			}
		}
	}
}
