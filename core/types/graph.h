#pragma once

#include "core/reference.h"

#include "core/hash_map.h"
#include "core/local_vector.h"
#include "core/oa_hash_map.h"
#include "core/pair.h"

#include "core/types/templates/stack.h"

class GraphVertex;
class GraphEdge;
class GraphIterator;

struct GraphData {
	using EdgeKey = Pair<uint32_t, uint32_t>;
	using EdgeList = LocalVector<GraphEdge *, int>;

	struct EdgeKeyHasher {
		static _FORCE_INLINE_ uint32_t hash(const EdgeKey &key) {
			return key.first ^ key.second;
		}
	};
	struct EdgeKeyComparator {
		static bool compare(const EdgeKey &a, const EdgeKey &b) {
			return ((a.first == b.first) && (a.second == b.second)) ||
				   ((a.first == b.second) && (a.second == b.first));
		}
	};
	HashMap<uint32_t, GraphVertex *> vertices;
	HashMap<EdgeKey, EdgeList, EdgeKeyHasher, EdgeKeyComparator> edges;

	void remove_vertex(GraphVertex *p_vertex);
	void remove_edge(GraphEdge *p_vertex);

	_FORCE_INLINE_ EdgeList &get_edges(uint32_t a_id, uint32_t b_id);
};

class Graph : public Reference {
	GDCLASS(Graph, Reference);

	GraphData *graph = nullptr;

	uint32_t next_vertex_id = 1;
	uint32_t next_edge_id = 1;

	Ref<GraphIterator> default_iterator;
	Ref<GraphIterator> V;

protected:
	static void _bind_methods();
	GraphEdge *_add_edge(const Variant &p_a, const Variant &p_b, const Variant &p_value, bool p_directed);

public:
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
	bool is_strongly_connected();
	Dictionary get_connected_components();

	// Cleanup
	void clear();
	void clear_edges();

	// Custom iterator
	void set_iterator(const Ref<GraphIterator> &p_iterator);
	Ref<GraphIterator> get_iterator() const { return V; }

	Graph();
	~Graph();
};

class GraphVertex : public Object {
	GDCLASS(GraphVertex, Object);

	friend class Graph;
	friend struct GraphData;
	friend class GraphIterator;
	friend class GraphIteratorDFS;

	GraphData *graph = nullptr;

	HashMap<uint32_t, GraphVertex *> neighbors;
	Variant value;
	uint32_t id;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	Array get_neighbors() const;
	int get_neighbor_count() const { return neighbors.size(); }

	Array get_successors() const;
	int get_successor_count() const;

	Array get_predecessors() const;
	int get_predecessor_count() const;

	void set_value(const Variant &p_value) { value = p_value; }
	Variant get_value() const { return value; }
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
	uint32_t id;

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

	void advance();

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
