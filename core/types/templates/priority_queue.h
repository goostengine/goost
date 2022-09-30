#pragma once

template <class T>
struct PriorityQueueMinHeapComparator {
	_FORCE_INLINE_ bool operator()(const T &a, const T &b) const { return (a < b); }
};

template <class T>
struct PriorityQueueMaxHeapComparator {
	_FORCE_INLINE_ bool operator()(const T &a, const T &b) const { return (a > b); }
};

template <typename T, class Comparator=PriorityQueueMinHeapComparator<T>>
class PriorityQueue {
	LocalVector<T> vector;

	_FORCE_INLINE_ unsigned int parent(int i) const {
		return (i - 1) / 2;
	}
	_FORCE_INLINE_ unsigned int left(int i) const {
		return i * 2 + 1;
	}
	_FORCE_INLINE_ unsigned int right(int i) const {
		return i * 2 + 2;
	}
	_FORCE_INLINE_ void swap(int i, int j) {
		T tmp = vector[i];
		vector[i] = vector[j];
		vector[j] = tmp;
	}
	void bubble_up(int i) {
		while (i > 0 && compare(vector[i], vector[parent(i)])) {
			swap(i, parent(i));
			i = parent(i);
		}
	}
	void sift_down(int i) {
		while (left(i) < vector.size()) {
			int c;
			if (right(i) > vector.size() - 1) {
				c = left(i);
			} else if (compare(vector[left(i)], vector[right(i)])) {
				c = left(i);
			} else {
				c = right(i);
			}
			if (compare(vector[c], vector[i])) {
				swap(c, i);
			}
			i = c;
		}
	}

public:
	Comparator compare;

	void initialize(const LocalVector<T> &p_elements) {
		vector.clear();
		if (p_elements.empty()) {
			return;
		}
		vector = p_elements;

		int i = vector.size() / 2;
		while (i >= 0) {
			sift_down(i--);
		}
	}
	void insert(const T &p_value) {
		vector.push_back(p_value);
		bubble_up(vector.size() - 1);
	}
	void update(const T &p_value) {
		int i = vector.find(p_value);
		ERR_FAIL_COND(i < 0);
		bubble_up(i);
	}
	T pop() {
		T root = vector[0];
		const int n = vector.size() - 1;
		vector[0] = vector[n];
		vector.resize(n);
		sift_down(0);
		return root;
	}
	_FORCE_INLINE_ bool is_empty() const {
		return vector.empty();
	}
	_FORCE_INLINE_ T top() {
		ERR_FAIL_COND_V(vector.empty(), T());
		return vector[0];
	}

	PriorityQueue() {}
	PriorityQueue(const LocalVector<T> &p_elements) {
		initialize(p_elements);
	}
};
