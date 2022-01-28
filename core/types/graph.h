#pragma once

#include "core/list.h"
#include "core/reference.h"

class GraphVertex;
class GraphEdge;

struct GraphData {
	Map<GraphVertex *, List<GraphEdge *>> data;
	void remove_vertex(GraphVertex *v);
	void remove_edge(GraphEdge *v);
};

class Graph : public Reference {
	GDCLASS(Graph, Reference);

	GraphData *graph = nullptr;

protected:
	static void _bind_methods();

	GraphEdge *_add_edge(const Variant &a, const Variant &b, real_t weight, bool bidirectional);

public:
	Array get_neighbors(GraphVertex *v);
	Array get_successors(GraphVertex *v);
	Array get_predecessors(GraphVertex *v);

	GraphVertex *add_vertex(const Variant &v);
	void remove_vertex(GraphVertex *v);
	bool has_vertex(GraphVertex *v) const { return graph->data.has(v); }
	Array get_vertex_list() const;
	int get_vertex_count() const { return graph->data.size(); }

	GraphEdge *add_edge(const Variant &a, const Variant &b, real_t weight = 1.0);
	GraphEdge *add_directed_edge(const Variant &a, const Variant &b, real_t weight = 1.0);
	void remove_edge(GraphEdge *e);
	GraphEdge *find_edge(GraphVertex *a, GraphVertex *b) const;
	Array get_edge_list(GraphVertex *a = nullptr, GraphVertex *b = nullptr) const;
	int get_edge_count() const;

	void clear();
	void clear_edges(); // TODO

	Graph();
	~Graph();
};

class GraphVertex : public Object {
	GDCLASS(GraphVertex, Object);

	friend class Graph;
	friend struct GraphData;
	GraphData *graph = nullptr;

	Variant value;

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

	GraphVertex *a = nullptr;
	GraphVertex *b = nullptr;
	bool bidirectional = true;

	real_t weight = 1.0;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	GraphVertex *get_vertex_a() const { return a; }
	GraphVertex *get_vertex_b() const { return b; }

	void set_weight(real_t p_weight) { weight = p_weight; }
	real_t get_weight() const { return weight; }
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
