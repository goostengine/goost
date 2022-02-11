#pragma once

#include "core/map.h"
#include "core/vector.h"
/*
	Based on Godot's DisjointSet structure:
	https://github.com/godotengine/godot/blob/11e09e59d/core/math/disjoint_set.h
	- Assumes duplicate elements are not added to the set for performance reasons.
	- Supports FIND operation.
*/
template <typename T, class C = Comparator<T>, class AL = DefaultAllocator>
class UnionFind {
public:
	struct Element {
		T object;
		Element *parent = nullptr;
		int rank = 0;
	};
	using Container = Map<T, Element *, C, AL>;

protected:
	_FORCE_INLINE_ Element *get_element(T object);
	Element *get_parent(Element *element);

public:
	_FORCE_INLINE_ Element *insert(T object); // Make set.
	void merge(T a, T b); // Union.
	Element *find(T object); // Find.

	void get_representatives(Vector<T> &out_roots);
	void get_members(Vector<T> &out_members, T representative);

	~UnionFind();

private:
	Container elements;
};

template <typename T, class C, class AL>
typename UnionFind<T, C, AL>::Element *UnionFind<T, C, AL>::get_parent(Element *element) {
	if (element->parent != element) {
		element->parent = get_parent(element->parent);
	}
	return element->parent;
}

template <typename T, class C, class AL>
typename UnionFind<T, C, AL>::Element *UnionFind<T, C, AL>::find(T object) {
	return get_parent(get_element(object));
}

template <typename T, class C, class AL>
typename UnionFind<T, C, AL>::Element *UnionFind<T, C, AL>::get_element(T object) {
	typename Container::Element *E = elements.find(object);
	ERR_FAIL_NULL_V(E, nullptr);
	return E->value();
}

template <typename T, class C, class AL>
typename UnionFind<T, C, AL>::Element *UnionFind<T, C, AL>::insert(T object) {
	Element *new_element = memnew_allocator(Element, AL);
	new_element->object = object;
	new_element->parent = new_element;
	elements.insert(object, new_element);

	return new_element;
}

template <typename T, class C, class AL>
void UnionFind<T, C, AL>::merge(T a, T b) {
	Element *x = get_element(a);
	Element *y = get_element(b);

	Element *x_root = get_parent(x);
	Element *y_root = get_parent(y);

	// Already in the same set.
	if (x_root == y_root) {
		return;
	}
	// Not in the same set, merge.
	if (x_root->rank < y_root->rank) {
		SWAP(x_root, y_root);
	}
	// Merge smaller set into larger set.
	y_root->parent = x_root;
	if (x_root->rank == y_root->rank) {
		x_root->rank++;
	}
}

template <typename T, class C, class AL>
void UnionFind<T, C, AL>::get_representatives(Vector<T> &out_representatives) {
	for (typename Container::Element *E = elements.front(); E; E = E->next()) {
		Element *element = E->value();
		if (element->parent == element) {
			out_representatives.push_back(element->object);
		}
	}
}

template <typename T, class C, class AL>
void UnionFind<T, C, AL>::get_members(Vector<T> &out_members, T representative) {
	typename Container::Element *rep_E = elements.find(representative);
	ERR_FAIL_COND(rep_E == nullptr);

	Element *rep_element = rep_E->value();
	ERR_FAIL_COND(rep_element->parent != rep_element);

	for (typename Container::Element *E = elements.front(); E; E = E->next()) {
		Element *parent = get_parent(E->value());
		if (parent == rep_element) {
			out_members.push_back(E->key());
		}
	}
}

template <typename T, class C, class AL>
UnionFind<T, C, AL>::~UnionFind() {
	for (typename Container::Element *E = elements.front(); E; E = E->next()) {
		memdelete_allocator<Element, AL>(E->value());
	}
}
