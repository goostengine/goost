#pragma once

#include "core/hash_map.h"
#include "core/list.h"
#include "core/pair.h"
#include "core/reference.h"

class GraphVertex;
class GraphEdge;

struct GraphData {
	using EdgeKey = Pair<uint32_t, uint32_t>;

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
	HashMap<EdgeKey, List<GraphEdge *>, EdgeKeyHasher, EdgeKeyComparator> edges;

	void remove_vertex(GraphVertex *p_vertex);
	void remove_edge(GraphEdge *p_vertex);
};

class Graph : public Reference {
	GDCLASS(Graph, Reference);

	GraphData *graph = nullptr;

	uint32_t next_vertex_id = 1;
	uint32_t next_edge_id = 1;

protected:
	static void _bind_methods();

	GraphEdge *_add_edge(const Variant &p_a, const Variant &p_b, const Variant &p_value, bool p_directed);

public:
	// Vertex API
	GraphVertex *add_vertex(const Variant &p_value);
	void remove_vertex(GraphVertex *p_vertex);
	bool has_vertex(GraphVertex *p_vertex) const;
	Array get_vertex_list() const;
	int get_vertex_count() const { return graph->vertices.size(); }

	Array get_neighbors(GraphVertex *p_vertex);
	Array get_successors(GraphVertex *p_vertex);
	Array get_predecessors(GraphVertex *p_vertex);

	// Edge API
	GraphEdge *add_edge(const Variant &a, const Variant &b, const Variant &p_value);
	GraphEdge *add_directed_edge(const Variant &p_a, const Variant &p_b, const Variant &p_value);
	void remove_edge(GraphEdge *e);
	GraphEdge *find_edge(GraphVertex *a, GraphVertex *b) const;
	bool has_edge(GraphVertex *a, GraphVertex *b) const;
	Array get_edge_list(GraphVertex *a = nullptr, GraphVertex *b = nullptr) const;
	int get_edge_count() const { return graph->edges.size(); }

	// Cleanup
	void clear();
	void clear_edges();

	Graph();
	~Graph();
};

class GraphVertex : public Object {
	GDCLASS(GraphVertex, Object);

	friend class Graph;
	friend struct GraphData;

	GraphData *graph = nullptr;
	HashMap<uint32_t, GraphVertex *> neighbors;

	Variant value;
	uint32_t id;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_value(const Variant &p_value) { value = p_value; }
	Variant get_value() const { return value; }
};

class GraphEdge : public Object {
	GDCLASS(GraphEdge, Object);

	friend class Graph;
	friend struct GraphData;
	GraphData *graph = nullptr;

	Variant value;
	uint32_t id;

	GraphVertex *a = nullptr;
	GraphVertex *b = nullptr;
	bool directed = false;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	GraphVertex *get_vertex_a() const { return a; }
	GraphVertex *get_vertex_b() const { return b; }

	bool is_loop() { return a == b; }
	bool is_directed() { return directed; }

	void set_value(const Variant &p_value) { value = p_value; }
	Variant get_value() const { return value; }
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
