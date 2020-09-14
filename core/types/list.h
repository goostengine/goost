#ifndef GOOST_LIST_H
#define GOOST_LIST_H

// Implementation based on Godot's `core/list.h`, which is mostly
// an instantiation of the `List<Variant>` template class for performance,
// which also allows to register relevant classes to scripting.

#include "core/reference.h"
#include "core/sort_array.h"

class ListElement;

struct ListData {
	ListElement *first = nullptr;
	ListElement *last = nullptr;
	int size_cache = 0;
	bool erase(ListElement *p_I);
};

class ListElement : public Object {
	GDCLASS(ListElement, Object);

protected:
	static void _bind_methods();

private:
	friend class LinkedList;
	friend struct ListData;

	Variant value;
	ListElement *next_ptr = nullptr;
	ListElement *prev_ptr = nullptr;
	ListData *data = nullptr;

public:
	ListElement *get_next() { return next_ptr; }
	ListElement *get_prev() { return prev_ptr; }

	Variant get_value() { return value; }
	void set_value(const Variant &p_value) { value = p_value; }

	void erase() { data->erase(this); }

	ListElement() {}
};

class LinkedList : public Reference {
	GDCLASS(LinkedList, Reference);

protected:
	static void _bind_methods();

private:
	ListData *_data = nullptr;

protected:
	// Custom iterator for scripting.
	ListElement *_iter_current = nullptr;
	Variant _iter_init(const Array &p_iter);
	Variant _iter_next(const Array &p_iter);
	Variant _iter_get(const Variant &p_iter);

public:
	ListElement *get_front() { return _data ? _data->first : 0; }
	ListElement *get_back() { return _data ? _data->last : 0; }

	ListElement *push_back(const Variant &value);
	void pop_back();
	ListElement *push_front(const Variant &value);
	void pop_front();

	Array get_elements();

	ListElement *insert_after(ListElement *p_element, const Variant &p_value);
	ListElement *insert_before(ListElement *p_element, const Variant &p_value);

	ListElement *find(const Variant &p_value);

	bool remove(ListElement *p_I);
	bool erase(const Variant &value);
	bool empty() const { return !_data || !_data->size_cache; }
	void clear();
	int size() const { return _data ? _data->size_cache : 0; }

	void swap(ListElement *p_A, ListElement *p_B);
	void invert();

	void move_to_back(ListElement *p_I);
	void move_to_front(ListElement *p_I);
	void move_before(ListElement *p_A, ListElement *p_B);

	void sort() {
		sort_custom<Comparator<Variant>>();
	}

	template <class C>
	void sort_custom_inplace() {
		if (size() < 2)
			return;

		ListElement *from = get_front();
		ListElement *current = from;
		ListElement *to = from;

		while (current) {
			ListElement *next = current->next_ptr;

			if (from != current) {
				current->prev_ptr = nullptr;
				current->next_ptr = from;

				ListElement *find = from;
				C less;
				while (find && less(find->value, current->value)) {
					current->prev_ptr = find;
					current->next_ptr = find->next_ptr;
					find = find->next_ptr;
				}

				if (current->prev_ptr)
					current->prev_ptr->next_ptr = current;
				else
					from = current;

				if (current->next_ptr)
					current->next_ptr->prev_ptr = current;
				else
					to = current;
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
		bool operator()(const ListElement *a, const ListElement *b) const {
			return compare(a->value, b->value);
		}
	};

	template <class C>
	void sort_custom() {
		int s = size();
		if (s < 2)
			return;

		ListElement **aux_buffer = memnew_arr(ListElement *, s);

		int idx = 0;
		for (ListElement *E = get_front(); E; E = E->next_ptr) {
			aux_buffer[idx] = E;
			idx++;
		}

		SortArray<ListElement *, AuxiliaryComparator<C>> sort;
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
	LinkedList() {}
	~LinkedList();
};

template <>
struct VariantCaster<ListElement *> {
	static _FORCE_INLINE_ ListElement *cast(const Variant &p_variant) {
		return (ListElement *)p_variant.operator Object *();
	}
};

#endif // GOOST_LIST_H
