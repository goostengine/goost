#pragma once

// Implementation based on Godot's `core/list.h`, which is mostly
// an instantiation of the `List<Variant>` template class for performance,
// which also allows to register relevant classes to scripting.

#include "core/reference.h"
#include "core/sort_array.h"

class ListNode;

struct ListData {
	ListNode *first = nullptr;
	ListNode *last = nullptr;
	int size_cache = 0;
	bool remove(ListNode *p_node);
};

class ListNode : public Object {
	GDCLASS(ListNode, Object);

protected:
	void _notification(int p_what);
	static void _bind_methods();

private:
	friend class LinkedList;
	friend struct ListData;

	Variant value;
	ListNode *next_ptr = nullptr;
	ListNode *prev_ptr = nullptr;
	ListData *data = nullptr;

public:
	ListNode *get_next() { return next_ptr; }
	ListNode *get_prev() { return prev_ptr; }

	Variant get_value() { return value; }
	void set_value(const Variant &p_value) { value = p_value; }

	virtual String to_string() { return String(value); }

	ListNode() {}
};

class LinkedList : public Reference {
	GDCLASS(LinkedList, Reference);

protected:
	static void _bind_methods();

private:
	ListData *_data = nullptr;

protected:
	// Custom iterator for scripting.
	ListNode *_iter_current = nullptr;
	Variant _iter_init(const Array &p_iter);
	Variant _iter_next(const Array &p_iter);
	Variant _iter_get(const Variant &p_iter);

public:
	void create_from(const Variant &p_value);

	ListNode *get_front() { return _data ? _data->first : 0; }
	ListNode *get_back() { return _data ? _data->last : 0; }

	ListNode *push_back(const Variant &value);
	void pop_back();
	ListNode *push_front(const Variant &value);
	void pop_front();

	Array get_nodes();
	Array get_elements(); // Alias for `get_nodes`.

	ListNode *insert_after(ListNode *p_node, const Variant &p_value);
	ListNode *insert_before(ListNode *p_node, const Variant &p_value);

	ListNode *find(const Variant &p_value);

	bool erase(const Variant &value);
	bool empty() const { return !_data || !_data->size_cache; }
	void clear();
	int size() const { return _data ? _data->size_cache : 0; }

	void swap(ListNode *p_node_A, ListNode *p_node_B);
	void invert();

	void move_to_back(ListNode *p_node);
	void move_to_front(ListNode *p_node);
	void move_before(ListNode *p_node_A, ListNode *p_node_B);

	void sort() {
		sort_custom<Comparator<Variant>>();
	}

	virtual String to_string();

	template <class C>
	void sort_custom_inplace() {
		if (size() < 2) {
			return;
		}
		ListNode *from = get_front();
		ListNode *current = from;
		ListNode *to = from;

		while (current) {
			ListNode *next = current->next_ptr;

			if (from != current) {
				current->prev_ptr = nullptr;
				current->next_ptr = from;

				ListNode *find = from;
				C less;
				while (find && less(find->value, current->value)) {
					current->prev_ptr = find;
					current->next_ptr = find->next_ptr;
					find = find->next_ptr;
				}
				if (current->prev_ptr) {
					current->prev_ptr->next_ptr = current;
				} else {
					from = current;
				}
				if (current->next_ptr) {
					current->next_ptr->prev_ptr = current;
				} else {
					to = current;
				}
			} else {
				current->prev_ptr = nullptr;
				current->next_ptr = nullptr;
			}
			current = next;
		}
		_data->first = from;
		_data->last = to;
	}

	template <class C>
	struct AuxiliaryComparator {
		C compare;
		bool operator()(const ListNode *a, const ListNode *b) const {
			return compare(a->value, b->value);
		}
	};

	template <class C>
	void sort_custom() {
		int s = size();
		if (s < 2) {
			return;
		}
		ListNode **aux_buffer = memnew_arr(ListNode *, s);

		int idx = 0;
		for (ListNode *E = get_front(); E; E = E->next_ptr) {
			aux_buffer[idx] = E;
			idx++;
		}
		SortArray<ListNode *, AuxiliaryComparator<C>> sort;
		sort.sort(aux_buffer, s);

		_data->first = aux_buffer[0];
		aux_buffer[0]->prev_ptr = nullptr;
		aux_buffer[0]->next_ptr = aux_buffer[1];

		_data->last = aux_buffer[s - 1];
		aux_buffer[s - 1]->prev_ptr = aux_buffer[s - 2];
		aux_buffer[s - 1]->next_ptr = nullptr;

		for (int i = 1; i < s - 1; i++) {
			aux_buffer[i]->prev_ptr = aux_buffer[i - 1];
			aux_buffer[i]->next_ptr = aux_buffer[i + 1];
		}
		memdelete_arr(aux_buffer);
	}
	LinkedList();
	~LinkedList();
};

template <>
struct VariantCaster<ListNode *> {
	static _FORCE_INLINE_ ListNode *cast(const Variant &p_variant) {
		if (p_variant.get_type() == Variant::OBJECT) {
			return Object::cast_to<ListNode>(p_variant);
		}
		return nullptr;
	}
};
