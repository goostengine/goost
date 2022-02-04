#pragma once

#include "core/local_vector.h"

// Currently just a simple implementation/wrapper over vector, for depth-first search.
template <class T>
class Stack {
	LocalVector<T> container;
	uint32_t back = 0;

public:
	_FORCE_INLINE_ void push_back(const T &p_element) {
		if (back == container.size()) {
			container.push_back(p_element);
		} else {
			container[back] = p_element;
		}
		back++;
	}
	_FORCE_INLINE_ T &pop_back() {
		T &element = container[back - 1];
		back--;
		return element;
	}
	void reserve(uint32_t p_capacity) {
		container.reserve(p_capacity);
	}
	_FORCE_INLINE_ bool is_empty() const {
		return back == 0;
	}
};
