#pragma once

#include "core/local_vector.h"

// Currently just a simple implementation/wrapper over vector, for depth-first search.
template <class T, class U = uint32_t>
class Stack {
	LocalVector<T, U> container;
	U index = 0;

public:
	_FORCE_INLINE_ T &top() {
		return container[index - 1];
	}
	_FORCE_INLINE_ void push(const T &p_element) {
		if (index == container.size()) {
			container.push_back(p_element);
		} else {
			container[index] = p_element;
		}
		index++;
	}
	_FORCE_INLINE_ T &pop() {
		T &element = container[index - 1];
		index--;
		return element;
	}
	void reserve(U p_capacity) {
		container.reserve(p_capacity);
	}
	void clear() {
		container.clear();
		index = 0;
	}
	_FORCE_INLINE_ bool is_empty() const {
		return index == 0;
	}
	_FORCE_INLINE_ U size() const {
		return index;
	}
};
