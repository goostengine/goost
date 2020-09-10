#ifndef GOOST_LIST_H
#define GOOST_LIST_H

// Implementation based on Godot's `core/list.h`, which is mostly
// an instantiation of the `List<Variant>` template class for performance,
// which also allows to register relevant classes to scripting.

#include "core/object.h"
#include "core/sort_array.h"

class VariantListElement;

struct VariantListData {
	VariantListElement *first = nullptr;
	VariantListElement *last = nullptr;
	int size_cache = 0;
	bool erase(VariantListElement *p_I);
};

class VariantListElement : public Object {
	GDCLASS(VariantListElement, Object);

protected:
	static void _bind_methods();

private:
	friend class VariantList;
	friend struct VariantListData;

	Variant value;
	VariantListElement *next_ptr = nullptr;
	VariantListElement *prev_ptr = nullptr;
	VariantListData *data = nullptr;

public:
	VariantListElement *next();
	VariantListElement *prev();
	Variant get_value() { return value; }
	void set_value(const Variant &p_value) { value = p_value; }
	void erase() { data->erase(this); }

	VariantListElement() {}
};

class VariantList : public Object {
	GDCLASS(VariantList, Object);

protected:
	static void _bind_methods();

private:
	VariantListData *_data = nullptr;

public:
	VariantListElement *front();
	VariantListElement *back();
	VariantListElement *push_back(const Variant &value);
	void pop_back();
	VariantListElement *push_front(const Variant &value);
	void pop_front();

	VariantListElement *insert_after(VariantListElement *p_element, const Variant &p_value);
	VariantListElement *insert_before(VariantListElement *p_element, const Variant &p_value);

	VariantListElement *find(const Variant &p_val);

	bool erase(VariantListElement *p_I);
	// bool erase(const Variant &value); // TODO: rename to erase_first_found?
	bool empty() const;
	void clear();
	int size() const;

	void swap(VariantListElement *p_A, VariantListElement *p_B);

	void move_to_back(VariantListElement *p_I);

	void invert();

	void move_to_front(VariantListElement *p_I);
	void move_before(VariantListElement *value, VariantListElement *where);

	void sort() {
		sort_custom<Comparator<Variant>>();
	}

	template <class C>
	void sort_custom_inplace() {
		if (size() < 2)
			return;

		VariantListElement *from = front();
		VariantListElement *current = from;
		VariantListElement *to = from;

		while (current) {
			VariantListElement *next = current->next_ptr;

			if (from != current) {
				current->prev_ptr = nullptr;
				current->next_ptr = from;

				VariantListElement *find = from;
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
		bool operator()(const VariantListElement *a, const VariantListElement *b) const {
			return compare(a->value, b->value);
		}
	};

	template <class C>
	void sort_custom() {
		int s = size();
		if (s < 2)
			return;

		VariantListElement **aux_buffer = memnew_arr(VariantListElement *, s);

		int idx = 0;
		for (VariantListElement *E = front(); E; E = E->next_ptr) {
			aux_buffer[idx] = E;
			idx++;
		}

		SortArray<VariantListElement *, AuxiliaryComparator<C>> sort;
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
	// VariantList(const VariantList &p_list);
	VariantList() {}
};

template <>
struct VariantCaster<VariantListElement*> {
	static _FORCE_INLINE_ VariantListElement* cast(const Variant &p_variant) {
		return (VariantListElement*)p_variant.operator Object*();
	}
};

#endif // GOOST_LIST_H
