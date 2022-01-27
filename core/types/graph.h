#pragma once

#include "core/list.h"
#include "core/reference.h"

class GraphVertex;
class GraphEdge;

class Graph : public Reference {
	GDCLASS(Graph, Reference);

	Map<GraphVertex *, List<GraphEdge *>> graph;

protected:
	static void _bind_methods();

	GraphEdge *_add_edge(const Variant &a, const Variant &b, real_t weight, bool bidirectional);

public:
	Array get_neighborhood(const GraphVertex *v);

	GraphVertex *add_vertex(const Variant &v);

	GraphEdge *add_edge(const Variant &a, const Variant &b, real_t weight = 1.0);
	GraphEdge *add_directed_edge(const Variant &a, const Variant &b, real_t weight = 1.0);
	GraphEdge *find_edge(GraphVertex *a, GraphVertex *b) const;
};

class GraphVertex : public Object {
	GDCLASS(GraphVertex, Object);

	friend class Graph;

	Variant value;

protected:
	static void _bind_methods();

public:
	void set_value(const Variant &p_value) { value = p_value; }
	Variant get_value() const { return value; }
};

class GraphEdge : public Object {
	GDCLASS(GraphEdge, Object);

	friend class Graph;

	GraphVertex *a = nullptr;
	GraphVertex *b = nullptr;
	bool bidirectional = true;

	real_t weight = 1.0;

protected:
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
