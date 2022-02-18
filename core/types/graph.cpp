#include "graph.h"

#include "core/script_language.h"
#include "core/string_names.h"

#include "core/types/templates/priority_queue.h"
#include "core/types/templates/union_find.h"

#include <limits>

using EdgeKey = GraphData::EdgeKey;
using EdgeList = LocalVector<GraphEdge *, int>;

void GraphData::remove_vertex(GraphVertex *p_vertex) {
	if (clearing_all) {
		return;
	}
	iterating = false;

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
			// since the vertex is going to be deleted.
			n_vertex->neighbors.erase(p_vertex->id);
			if (n_vertex->successors.has(p_vertex->id)) {
				n_vertex->successors.erase(p_vertex->id);
			}
			if (n_vertex->predecessors.has(p_vertex->id)) {
				n_vertex->predecessors.erase(p_vertex->id);
			}
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
	if (clearing_all) {
		return;
	}
	iterating = false;

	GraphVertex *&a = p_edge->a;
	GraphVertex *&b = p_edge->b;

	EdgeList &list = get_edges(a->id, b->id);

	for (int i = 0; i < list.size(); ++i) {
		if (p_edge == list[i]) {
			a->neighbors.erase(b->id);
			b->neighbors.erase(a->id);
			if (p_edge->directed) {
				a->successors.erase(b->id);
				b->predecessors.erase(a->id);
			}
			list.remove_unordered(i);
			break;
		}
	}
	if (list.empty()) {
		edges.erase(EdgeKey(a->id, b->id));
	}
}

void Graph::remove_edge(GraphEdge *p_edge) {
	ERR_FAIL_NULL(p_edge);
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

void Graph::_set_data(const Dictionary &p_data) {
	ERR_FAIL_COND(p_data.empty());
	ERR_FAIL_COND(!p_data.has("vertices"));
	ERR_FAIL_COND(!p_data.has("edges"));

	clear();

	Array vertices = p_data["vertices"];
	ERR_FAIL_COND(vertices.size() % 2 != 0);

	for (int i = 0; i < vertices.size(); i += 2) {
		uint32_t id = vertices[i + 0];
		Variant value = vertices[i + 1];
		_add_vertex(value, id);
	}

	Array edges = p_data["edges"];
	for (int i = 0; i < edges.size(); ++i) {
		Array edge_data = edges[i];
		ERR_FAIL_COND(edge_data.size() < 4);

		uint32_t a_id = edge_data[0];
		uint32_t b_id = edge_data[1];
		Variant value = edge_data[2];
		bool directed = edge_data[3];

		GraphVertex *a = graph->vertices[a_id];
		GraphVertex *b = graph->vertices[b_id];

		_add_edge(a, b, value, directed);
	}
}

Dictionary Graph::_get_data() const {
	Dictionary data;

	Array vertices;
	{
		const uint32_t *k = nullptr;
		while ((k = graph->vertices.next(k))) {
			const GraphVertex *v = graph->vertices[*k];
			vertices.push_back(v->id);
			vertices.push_back(v->value);
		}
	}
	Array edges;
	{
		const EdgeKey *k = nullptr;
		while ((k = graph->edges.next(k))) {
			EdgeList &list = graph->edges[*k];
			for (int i = 0; i < list.size(); ++i) {
				const GraphEdge *e = list[i];
				Array edge_data;
				edge_data.push_back(e->a->id);
				edge_data.push_back(e->b->id);
				edge_data.push_back(e->value);
				edge_data.push_back(e->directed);
				edges.push_back(edge_data);
			}
		}
	}
	data["vertices"] = vertices;
	data["edges"] = edges;

	return data;
}

GraphVertex *Graph::_add_vertex(const Variant &p_value, uint32_t p_id) {
	graph->iterating = false;

	GraphVertex *v = _create_vertex();

	uint32_t id = p_id;
	if (id == 0) {
		id = next_vertex_id++;
	}
	v->id = id;
	v->value = p_value;

	graph->vertices[v->id] = v;
	v->graph = graph;

	return v;
}

GraphVertex *Graph::add_vertex(const Variant &p_value) {
	return _add_vertex(p_value, 0);
}

void Graph::remove_vertex(GraphVertex *p_vertex) {
	ERR_FAIL_NULL(p_vertex);
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
	ERR_FAIL_NULL_V(p_vertex, false);
	return graph->vertices.has(p_vertex->id);
}

Array Graph::get_vertices() const {
	Array vertices;

	const uint32_t *k = nullptr;
	while ((k = graph->vertices.next(k))) {
		vertices.push_back(graph->vertices[*k]);
	}
	return vertices;
}

Ref<Graph> GraphVertex::get_graph() const {
	ERR_FAIL_NULL_V(graph, Ref<Graph>());

	Object *obj = ObjectDB::get_instance(graph->id);
	Ref<Graph> graph_ref(Object::cast_to<Graph>(obj));

	return graph_ref;
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
	while ((n = successors.next(n))) {
		ret.push_back(successors[*n]);
	}
	return ret;
}

Array GraphVertex::get_predecessors() const {
	Array ret;

	const uint32_t *n = nullptr;
	while ((n = predecessors.next(n))) {
		ret.push_back(predecessors[*n]);
	}
	return ret;
}

Array GraphVertex::get_edges() const {
	Array ret;
	
	const uint32_t *v = nullptr;
	while ((v = neighbors.next(v))) {
		const EdgeList &list = graph->get_edges(this->id, neighbors[*v]->id);
		for (int i = 0; i < list.size(); ++i) {
			ret.push_back(list[i]);
		}
	}
	return ret;
}

Array GraphVertex::get_inward_edges() const {
	Array ret;
	
	const uint32_t *v = nullptr;
	while ((v = predecessors.next(v))) {
		const EdgeList &list = graph->get_edges(this->id, predecessors[*v]->id);
		for (int i = 0; i < list.size(); ++i) {
			const GraphEdge *edge = list[i];
			if (edge->is_directed() && this == edge->get_b() && predecessors[*v] == edge->get_a()) {
				ret.push_back(edge);
			}
		}
	}
	return ret;
}

Array GraphVertex::get_outward_edges() const {
	Array ret;

	const uint32_t *v = nullptr;
	while ((v = successors.next(v))) {
		const EdgeList &list = graph->get_edges(this->id, successors[*v]->id);
		for (int i = 0; i < list.size(); ++i) {
			const GraphEdge *edge = list[i];
			if (edge->is_directed() && this == edge->get_a() && successors[*v] == edge->get_b()) {
				ret.push_back(edge);
			}
		}
	}
	return ret;
}

GraphEdge *Graph::_add_edge(const Variant &p_a, const Variant &p_b, const Variant &p_value, bool p_directed) {
	ERR_FAIL_COND_V(p_a.get_type() == Variant::NIL, nullptr);
	ERR_FAIL_COND_V(p_b.get_type() == Variant::NIL, nullptr);

	graph->iterating = false;

	GraphVertex *a = nullptr;
	GraphVertex *b = nullptr;

	// By default, check if the passed value is GraphVertex.
	// If not, then attempt to find an existing vertex by value.
	// If that fails as well, then just create a new vertex.
	if (p_a.get_type() == Variant::OBJECT) {
		a = Object::cast_to<GraphVertex>(p_a);
	}
	if (!a) {
		a = find_vertex(p_a);
		if (!a) {
			a = add_vertex(p_a);
		}
	}
	if (p_b.get_type() == Variant::OBJECT) {
		b = Object::cast_to<GraphVertex>(p_b);
	}
	if (!b) {
		b = find_vertex(p_b);
		if (!b) {
			b = add_vertex(p_b);
		}
	}
#ifdef DEBUG_ENABLED
	ERR_FAIL_NULL_V(a, nullptr);
	ERR_FAIL_NULL_V(b, nullptr);
	// This issue may happen when `GraphVertex`
	// is instantiated outside of `add_vertex()` call.
	ERR_FAIL_COND_V_MSG(a->graph != graph, nullptr, "The 'A' vertex is not owned by this graph");
	ERR_FAIL_COND_V_MSG(b->graph != graph, nullptr, "The 'B' vertex is not owned by this graph");
#endif
	// Setup neighbors.
	a->neighbors[b->id] = b;
	b->neighbors[a->id] = a;
	if (p_directed) {
		a->successors[b->id] = b;
		b->predecessors[a->id] = a;
	}
	// Instantiate a new edge.
	GraphEdge *edge = _create_edge();
	edge->a = a;
	edge->b = b;
	edge->value = p_value;
	edge->directed = p_directed;
	edge->graph = graph;

	// Either add or insert the edge.
	const auto &key = EdgeKey(a->id, b->id);
	if (!graph->edges.has(key)) {
		EdgeList list;
		list.reserve(2); // Slight optimization for directed or multi-edges.
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
	ERR_FAIL_NULL_V(p_a, nullptr);
	ERR_FAIL_NULL_V(p_b, nullptr);

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

GraphEdge *Graph::_find_minimum_edge(GraphVertex *p_a, GraphVertex *p_b) const {
	ERR_FAIL_NULL_V(p_a, nullptr);
	ERR_FAIL_NULL_V(p_b, nullptr);

	const EdgeList &list = graph->get_edges(p_a->id, p_b->id);
	if (list.empty()) {
		return nullptr;
	}
	GraphEdge *min_edge = list[0];
	real_t min_weight = min_edge->value;

	for (int i = 1; i < list.size(); ++i) {
		GraphEdge *edge = list[i];
		real_t weight = edge->value;
		if (weight < min_weight) {
			min_weight = weight;
			min_edge = edge;
		}
	}
	return min_edge;
}

bool Graph::has_edge(GraphVertex *p_a, GraphVertex *p_b) const {
	ERR_FAIL_NULL_V(p_a, false);
	ERR_FAIL_NULL_V(p_b, false);
	return find_edge(p_a, p_b) != nullptr;
}

Array Graph::get_edges(GraphVertex *p_a, GraphVertex *p_b) const {
	ERR_FAIL_COND_V_MSG(!p_a && p_b, Array(), "Parameter 'a' is not a valid GraphVertex");
	ERR_FAIL_COND_V_MSG(p_a && !p_b, Array(), "Parameter 'b' is not a valid GraphVertex");

	Array edges;

	const bool fetch_all = !p_a || !p_b;

	const EdgeKey *k = nullptr;
	while ((k = graph->edges.next(k))) {
		EdgeList &list = graph->edges[*k];

		for (int i = 0; i < list.size(); ++i) {
			GraphEdge *edge = list[i];
			if (fetch_all) { // Get all edges in the graph.
				edges.push_back(edge);
			} else { // Get all edges between vertices.
				if (p_a == edge->a && p_b == edge->b) {
					edges.push_back(edge);
				} else if (!edge->directed && p_a == edge->b && p_b == edge->a) {
					edges.push_back(edge);
				}
			}
		}
	}
	return edges;
}

Array Graph::find_connected_component(GraphVertex *p_vertex) {
	ERR_FAIL_NULL_V(p_vertex, Array());

	Array component;

	for (G->initialize(p_vertex); G->has_next();) {
		component.push_back(G->next());
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
	for (G->initialize(root); G->has_next(); G->next()) {
		count++;
	}
	return count == graph->vertices.size();
}

struct SortEdgesMST {
	_FORCE_INLINE_ bool operator()(const GraphEdge *a, const GraphEdge *b) const {
		real_t weight_a = a->get_value();
		real_t weight_b = b->get_value();

		if (weight_a < weight_b) {
			return true;
		} else if (weight_a > weight_b) {
			return false;
		}
		return false;
	}
};

// Kruskal's algorithm.
Array Graph::minimum_spanning_tree() const {
	// Sort all edges in increasing weight.
	Vector<GraphEdge *> edges;
	{
		const EdgeKey *k = nullptr;
		while ((k = graph->edges.next(k))) {
			EdgeList &list = graph->edges[*k];
			for (int i = 0; i < list.size(); ++i) {
				edges.push_back(list[i]);
			}
		}
		SortArray<GraphEdge *, SortEdgesMST> sorter;
		sorter.sort(edges.ptrw(), edges.size());
	}
	// Make set.
	UnionFind<GraphVertex *> set;
	{
		const uint32_t *k = nullptr;
		while ((k = graph->vertices.next(k))) {
			set.insert(graph->vertices[*k]);
		}
	}
	// Find tree.
	Array edges_tree;

	for (int i = 0; i < edges.size(); ++i) {
		const GraphEdge *edge = edges[i];
		if (set.find(edge->a) != set.find(edge->b)) {
			// No cycle is created, merge.
			edges_tree.push_back(edge);
			set.merge(edge->a, edge->b);
		}
	}
	return edges_tree;
}

struct DistanceComparator {
	HashMap<uint32_t, real_t> distance;

	_FORCE_INLINE_ bool operator()(const GraphVertex *a, const GraphVertex *b) const {
		return distance[a->get_id()] < distance[b->get_id()];
	}
};

// Dijkstra's algorithm.
Dictionary Graph::shortest_path_tree(GraphVertex *p_root) const {
	ERR_FAIL_NULL_V(p_root, Dictionary());

	Dictionary tree;

	PriorityQueue<GraphVertex *, DistanceComparator> queue;
	LocalVector<GraphVertex *> vertices;

	HashMap<uint32_t, real_t> &distance = queue.compare.distance;
	HashMap<uint32_t, uint32_t> backtrace;
	{
		const uint32_t *k = nullptr;
		while ((k = graph->vertices.next(k))) {
			GraphVertex *v = graph->vertices[*k];
			distance[v->id] = v == p_root ? 0.0 : std::numeric_limits<real_t>::infinity();
			backtrace[v->id] = 0;
			vertices.push_back(v);
		}
	}
	queue.initialize(vertices);

	// Find shortest path tree.
	while (!queue.is_empty()) {
		GraphVertex *u = queue.pop();

		const uint32_t *k = nullptr;
		while ((k = u->neighbors.next(k))) {
			GraphVertex *v = graph->vertices[*k];
			GraphEdge *edge = _find_minimum_edge(u, v);
			real_t weight = edge->value;

			if (distance[v->id] > distance[u->id] + weight) {
				distance[v->id] = distance[u->id] + weight;
				backtrace[v->id] = u->id; // Previous.
				queue.update(v);
			}
		}
	}
	// Convert output.
	Dictionary out_backtrace;
	{
		const uint32_t *k = nullptr;
		while ((k = backtrace.next(k))) {
			GraphVertex *vc = graph->vertices[*k];
			GraphVertex *vp = nullptr;
			if (backtrace[*k] != 0) {
				vp = graph->vertices[backtrace[*k]];
			}
			out_backtrace[vc] = vp;
		}
	}
	Dictionary out_distance;
	{
		const uint32_t *k = nullptr;
		while ((k = distance.next(k))) {
			GraphVertex *v = graph->vertices[*k];
			real_t d = distance[*k];
			out_distance[v] = d;
		}
	}
	Array out_edges;
	{
		const uint32_t *k = nullptr;
		while ((k = backtrace.next(k))) {
			if (backtrace[*k] == 0) {
				continue; // Stumbled upon root.
			}
			GraphVertex *current = graph->vertices[*k];
			GraphVertex *previous = graph->vertices[backtrace[*k]];
			GraphEdge *edge = _find_minimum_edge(previous, current);
			out_edges.push_back(edge);
		}
	}
	tree["backtrace"] = out_backtrace;
	tree["distance"] = out_distance;
	tree["edges"] = out_edges;

	return tree;
}

Dictionary Graph::get_connected_components() {
	Dictionary components;

	UnionFind<GraphVertex *> set;
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
				set.merge(list[i]->a, list[i]->b);
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
	graph->iterating = false;

	graph->clearing_all = true;
	clear_edges();

	LocalVector<GraphVertex *, int> vertices_to_delete;

	const uint32_t *k = nullptr;
	while ((k = graph->vertices.next(k))) {
		vertices_to_delete.push_back(graph->vertices[*k]);
	}
	for (int i = 0; i < vertices_to_delete.size(); ++i) {
		memdelete(vertices_to_delete[i]);
	}
	graph->vertices.clear();
	next_vertex_id = 1;

	graph->clearing_all = false;
}

void Graph::clear_edges() {
	graph->iterating = false;

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
	ERR_FAIL_COND_MSG(p_iterator.is_null(), "Invalid iterator");
	G = p_iterator;
}

Variant Graph::_iter_init(const Array &p_iter) {
	_iter_current = graph->vertices.next(nullptr);
	graph->iterating = true;
	return _iter_current != nullptr;
}

Variant Graph::_iter_next(const Array &p_iter) {
	ERR_FAIL_COND_V(!graph->iterating, false);
	_iter_current = graph->vertices.next(_iter_current);
	graph->iterating = _iter_current != nullptr;
	return graph->iterating;
}

Variant Graph::_iter_get(const Variant &p_iter) {
	ERR_FAIL_COND_V(!graph->iterating, Variant());
	return graph->vertices[*_iter_current];
}

void Graph::_bind_methods() {
	BIND_VMETHOD(MethodInfo(Variant::OBJECT, "_create_vertex"));
	BIND_VMETHOD(MethodInfo(Variant::OBJECT, "_create_edge"));

	ClassDB::bind_method(D_METHOD("_iter_init"), &Graph::_iter_init);
	ClassDB::bind_method(D_METHOD("_iter_get"), &Graph::_iter_get);
	ClassDB::bind_method(D_METHOD("_iter_next"), &Graph::_iter_next);

	ClassDB::bind_method(D_METHOD("add_vertex", "value"), &Graph::add_vertex);
	ClassDB::bind_method(D_METHOD("remove_vertex", "vertex"), &Graph::remove_vertex);
	ClassDB::bind_method(D_METHOD("find_vertex", "value"), &Graph::find_vertex);
	ClassDB::bind_method(D_METHOD("has_vertex", "vertex"), &Graph::has_vertex);
	ClassDB::bind_method(D_METHOD("get_vertices"), &Graph::get_vertices);
	ClassDB::bind_method(D_METHOD("get_vertex_count"), &Graph::get_vertex_count);

	ClassDB::bind_method(D_METHOD("add_edge", "a", "b", "value"), &Graph::add_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("add_directed_edge", "from", "to", "value"), &Graph::add_directed_edge, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("remove_edge", "edge"), &Graph::remove_edge);
	ClassDB::bind_method(D_METHOD("find_edge", "a", "b"), &Graph::find_edge);
	ClassDB::bind_method(D_METHOD("has_edge", "a", "b"), &Graph::has_edge);
	ClassDB::bind_method(D_METHOD("get_edges", "a", "b"), &Graph::get_edges, DEFVAL(Variant()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("get_edge_count"), &Graph::get_edge_count);

	ClassDB::bind_method(D_METHOD("find_connected_component", "vertex"), &Graph::find_connected_component);
	ClassDB::bind_method(D_METHOD("get_connected_components"), &Graph::get_connected_components);
	ClassDB::bind_method(D_METHOD("is_strongly_connected"), &Graph::is_strongly_connected);

	ClassDB::bind_method(D_METHOD("minimum_spanning_tree"), &Graph::minimum_spanning_tree);
	ClassDB::bind_method(D_METHOD("shortest_path_tree", "root"), &Graph::shortest_path_tree);

	ClassDB::bind_method(D_METHOD("clear"), &Graph::clear);
	ClassDB::bind_method(D_METHOD("clear_edges"), &Graph::clear_edges);

	ClassDB::bind_method(D_METHOD("set_iterator", "iterator"), &Graph::set_iterator);
	ClassDB::bind_method(D_METHOD("get_iterator"), &Graph::get_iterator);

	ClassDB::bind_method(D_METHOD("set_iterator_dfs"), &Graph::set_iterator_dfs);
	ClassDB::bind_method(D_METHOD("set_iterator_bfs"), &Graph::set_iterator_bfs);

	ClassDB::bind_method(D_METHOD("_set_data", "data"), &Graph::_set_data);
	ClassDB::bind_method(D_METHOD("_get_data"), &Graph::_get_data);

	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "_set_data", "_get_data");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "iterator", PROPERTY_HINT_NONE, "", 0), "set_iterator", "get_iterator");
}

Graph::Graph() {
	graph = memnew(GraphData);
	graph->id = get_instance_id();
	ERR_FAIL_COND(graph->id == 0);

	G_dfs.instance();
	G_bfs.instance();
	G = G_dfs; // Set depth-first search iterator by default.
}

Graph::~Graph() {
	clear();
	if (graph) {
		memdelete(graph);
	}
}

Variant GraphVertex::_iter_init(const Array &p_iter) {
	ERR_FAIL_NULL_V(graph, false);
	_iter_current = neighbors.next(nullptr);
	graph->iterating = true;
	return _iter_current != nullptr;
}

Variant GraphVertex::_iter_next(const Array &p_iter) {
	ERR_FAIL_NULL_V(graph, false);
	_iter_current = neighbors.next(_iter_current);
	graph->iterating = _iter_current != nullptr;
	return graph->iterating;
}

Variant GraphVertex::_iter_get(const Variant &p_iter) {
	ERR_FAIL_NULL_V(graph, Variant());
	ERR_FAIL_COND_V(!graph->iterating, Variant());
	return neighbors[*_iter_current];
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
	ClassDB::bind_method(D_METHOD("_iter_init"), &GraphVertex::_iter_init);
	ClassDB::bind_method(D_METHOD("_iter_get"), &GraphVertex::_iter_get);
	ClassDB::bind_method(D_METHOD("_iter_next"), &GraphVertex::_iter_next);

	ClassDB::bind_method(D_METHOD("get_neighbors"), &GraphVertex::get_neighbors);
	ClassDB::bind_method(D_METHOD("get_neighbor_count"), &GraphVertex::get_neighbor_count);

	ClassDB::bind_method(D_METHOD("get_successors"), &GraphVertex::get_successors);
	ClassDB::bind_method(D_METHOD("get_successor_count"), &GraphVertex::get_successor_count);

	ClassDB::bind_method(D_METHOD("get_predecessors"), &GraphVertex::get_predecessors);
	ClassDB::bind_method(D_METHOD("get_predecessor_count"), &GraphVertex::get_predecessor_count);

	ClassDB::bind_method(D_METHOD("get_edges"), &GraphVertex::get_edges);
	ClassDB::bind_method(D_METHOD("get_inward_edges"), &GraphVertex::get_inward_edges);
	ClassDB::bind_method(D_METHOD("get_outward_edges"), &GraphVertex::get_outward_edges);

	ClassDB::bind_method(D_METHOD("set_value", "value"), &GraphVertex::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &GraphVertex::get_value);

	ClassDB::bind_method(D_METHOD("get_graph"), &GraphVertex::get_graph);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_value", "get_value");
}

Ref<Graph> GraphEdge::get_graph() const {
	ERR_FAIL_NULL_V(graph, Ref<Graph>());

	Object *obj = ObjectDB::get_instance(graph->id);
	Ref<Graph> graph_ref(Object::cast_to<Graph>(obj));

	return graph_ref;
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

	ClassDB::bind_method(D_METHOD("get_graph"), &GraphEdge::get_graph);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "a"), "", "get_a");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "b"), "", "get_b");
	ADD_PROPERTY(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_value", "get_value");
}

void GraphIterator::_bind_methods() {
	BIND_VMETHOD(MethodInfo(Variant::NIL, "initialize", PropertyInfo(Variant::OBJECT, "root")));
	BIND_VMETHOD(MethodInfo(Variant::BOOL, "has_next"));
	BIND_VMETHOD(MethodInfo(Variant::OBJECT, "next"));
}

void GraphIteratorDFS::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "root"), &GraphIteratorDFS::initialize);
	ClassDB::bind_method(D_METHOD("has_next"), &GraphIteratorDFS::has_next);
	ClassDB::bind_method(D_METHOD("next"), &GraphIteratorDFS::next);
}

void GraphIteratorBFS::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "root"), &GraphIteratorBFS::initialize);
	ClassDB::bind_method(D_METHOD("has_next"), &GraphIteratorBFS::has_next);
	ClassDB::bind_method(D_METHOD("next"), &GraphIteratorBFS::next);
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

void GraphIteratorBFS::initialize(GraphVertex *p_root) {
	ERR_FAIL_NULL(p_root);

	graph = p_root->graph;

	queue.clear();
	visited.clear();

	queue.push_back(p_root);
	visited.insert(p_root);
}

bool GraphIteratorBFS::has_next() const {
	return !queue.is_empty();
}

GraphVertex *GraphIteratorBFS::next() {
	ERR_FAIL_COND_V(!has_next(), nullptr);

	GraphVertex *next_vertex = queue.pop_front();

	const uint32_t *k = nullptr;
	while ((k = next_vertex->neighbors.next(k))) {
		GraphVertex *child = graph->vertices[*k];
		if (!visited.has(child)) {
			visited.insert(child);
			queue.push_back(child);
		}
	}
	return next_vertex;
}
