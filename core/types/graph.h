#pragma once

#include "core/resource.h"

#include "core/hash_map.h"
#include "core/local_vector.h"
#include "core/oa_hash_map.h"

#include "core/types/templates/queue.h"
#include "core/types/templates/stack.h"

class GraphVertex;
class GraphEdge;

class GraphIterator;
class GraphIteratorDFS;
class GraphIteratorBFS;

struct GraphData {
	struct EdgeKey {
		uint32_t a;
		uint32_t b;

		EdgeKey(const uint32_t &p_a, const uint32_t &p_b) {
			if (p_a < p_b) {
				a = p_a;
				b = p_b;
			} else {
				a = p_b;
				b = p_a;
			}
		}
		bool operator==(const EdgeKey &other) const {
			return (a == other.a) && (b == other.b);
		}
	};
	using EdgeList = LocalVector<GraphEdge *, int>;

	struct EdgeKeyHasher {
		static _FORCE_INLINE_ uint32_t hash(const EdgeKey &key) {
			return (key.a << 16) + key.b;
		}
	};
	ObjectID id;
	HashMap<uint32_t, GraphVertex *> vertices;
	HashMap<EdgeKey, EdgeList, EdgeKeyHasher> edges;

	void remove_vertex(GraphVertex *p_vertex);
	void remove_edge(GraphEdge *p_vertex);
	// Allows to speed up the clean() method by skipping neighbor
	// updates and removing edges from the adjacency list.
	bool clearing_all = false; 

	_FORCE_INLINE_ EdgeList &get_edges(uint32_t a_id, uint32_t b_id);
};

class Graph : public Resource {
	GDCLASS(Graph, Resource);

	GraphData *graph = nullptr;

	uint32_t next_vertex_id = 1;

	Ref<GraphIterator> G;
	Ref<GraphIteratorDFS> G_dfs; // Default one.
	Ref<GraphIteratorBFS> G_bfs;

protected:
	static void _bind_methods();

	// Storage
	void _set_data(const Dictionary &p_data);
	Dictionary _get_data() const;

	// Utilities
	GraphVertex *_add_vertex(const Variant &p_value, uint32_t p_id = 0);
	GraphEdge *_add_edge(const Variant &p_a, const Variant &p_b, const Variant &p_value, bool p_directed);
	GraphEdge *_find_minimum_edge(GraphVertex *p_vertex_a, GraphVertex *p_vertex_b) const;

public:
	// Instantiation
	virtual GraphVertex *_create_vertex();
	virtual GraphEdge *_create_edge();

	// Vertex
	GraphVertex *add_vertex(const Variant &p_value);
	void remove_vertex(GraphVertex *p_vertex);
	GraphVertex *find_vertex(const Variant &p_value);
	bool has_vertex(GraphVertex *p_vertex) const;
	Array get_vertex_list() const;
	int get_vertex_count() const { return graph->vertices.size(); }

	// Edge
	GraphEdge *add_edge(const Variant &p_vertex_a, const Variant &p_vertex_b, const Variant &p_value);
	GraphEdge *add_directed_edge(const Variant &p_vertex_from, const Variant &p_vertex_to, const Variant &p_value);
	void remove_edge(GraphEdge *p_edge);
	GraphEdge *find_edge(GraphVertex *p_vertex_a, GraphVertex *p_vertex_b) const;
	bool has_edge(GraphVertex *p_vertex_a, GraphVertex *p_vertex_b) const;
	Array get_edge_list(GraphVertex *p_vertex_a = nullptr, GraphVertex *p_vertex_b = nullptr) const;
	int get_edge_count() const { return graph->edges.size(); }

	// Connectivity
	Array find_connected_component(GraphVertex *p_vertex);
	Dictionary get_connected_components();
	bool is_strongly_connected();

	Array minimum_spanning_tree() const;
	Dictionary shortest_path_tree(GraphVertex *p_root) const;

	// Cleanup
	void clear();
	void clear_edges();

	// Iterator
	void set_iterator(const Ref<GraphIterator> &p_iterator);
	Ref<GraphIterator> get_iterator() const { return G; }

	void set_iterator_dfs() { G = G_dfs; }
	void set_iterator_bfs() { G = G_bfs; }

	Graph();
	~Graph();
};

class GraphVertex : public Object {
	GDCLASS(GraphVertex, Object);

	friend class Graph;
	friend struct GraphData;

	friend class GraphIterator;
	friend class GraphIteratorDFS;
	friend class GraphIteratorBFS;

	GraphData *graph = nullptr;

	HashMap<uint32_t, GraphVertex *> neighbors;
	Variant value;
	uint32_t id = 0;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	Ref<Graph> get_graph() const;

	Array get_neighbors() const;
	int get_neighbor_count() const { return neighbors.size(); }

	Array get_successors() const;
	int get_successor_count() const;

	Array get_predecessors() const;
	int get_predecessor_count() const;

	void set_value(const Variant &p_value) { value = p_value; }
	Variant get_value() const { return value; }

	uint32_t get_id() const { return id; } // Used by distance comparator.
};

class GraphEdge : public Object {
	GDCLASS(GraphEdge, Object);

	friend class Graph;
	friend struct GraphData;

	GraphData *graph = nullptr;

	GraphVertex *a = nullptr;
	GraphVertex *b = nullptr;
	bool directed = false;

	Variant value;
	uint32_t id = 0;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	GraphVertex *get_a() const { return a; }
	GraphVertex *get_b() const { return b; }

	bool is_loop() { return a == b; }
	bool is_directed() { return directed; }

	void set_value(const Variant &p_value) { value = p_value; }
	Variant get_value() const { return value; }
};

class GraphIterator : public Reference {
	GDCLASS(GraphIterator, Reference);

protected:
	GraphData *graph = nullptr;
	GraphVertex *root = nullptr;

	static void _bind_methods();

public:
	virtual void initialize(GraphVertex *p_root);
	virtual bool has_next() const;
	virtual GraphVertex *next();
};

class GraphIteratorDFS : public GraphIterator {
	GDCLASS(GraphIteratorDFS, GraphIterator);

public:
	struct Element {
		GraphVertex *parent;
		const uint32_t *neighbor = nullptr;

		Element(){};
		Element(GraphVertex *p_parent) {
			parent = p_parent;
			neighbor = p_parent->neighbors.next(nullptr);
		}
	};

protected:
	Stack<Element> stack;
	Set<GraphVertex *> visited;
	GraphVertex *next_vertex = nullptr;

	static void _bind_methods();
	void advance();

public:
	virtual void initialize(GraphVertex *p_root);
	virtual bool has_next() const;
	virtual GraphVertex *next();
};

class GraphIteratorBFS : public GraphIterator {
	GDCLASS(GraphIteratorBFS, GraphIterator);

protected:
	Queue<GraphVertex *> queue;
	Set<GraphVertex *> visited;

	static void _bind_methods();

public:
	virtual void initialize(GraphVertex *p_root);
	virtual bool has_next() const;
	virtual GraphVertex *next();
};

// Cast for bindings.

template <>
struct VariantCaster<GraphVertex *> {
	static _FORCE_INLINE_ GraphVertex *cast(const Variant &p_variant) {
		return (GraphVertex *)p_variant.operator Object *();
	}
};

template <>
struct VariantCaster<GraphEdge *> {
	static _FORCE_INLINE_ GraphEdge *cast(const Variant &p_variant) {
		return (GraphEdge *)p_variant.operator Object *();
	}
};
