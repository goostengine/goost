#pragma once

#include "core/local_vector.h"

// Currently just a simple implementation/wrapper over vector, for breadth-first search.
template <class T, class U = uint32_t>
class Queue {
	LocalVector<T, U> container;
	U front = 0;
	U back = 0;

public:
	_FORCE_INLINE_ void push_back(const T &p_element) {
		container.push_back(p_element);
		back++;
	}
	_FORCE_INLINE_ T &pop_front() {
		return container[front++];
	}
	_FORCE_INLINE_ bool is_empty() const {
		return front == back;
	}
	_FORCE_INLINE_ U size() const {
		return container.size();
	}
	void reserve(U p_capacity) {
		container.reserve(p_capacity);
	}
	void clear() {
		container.clear();
		front = 0;
        back = 0;
	}
};
