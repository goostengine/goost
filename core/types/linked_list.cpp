#include "linked_list.h"

bool ListData::remove(ListNode *p_I) {
	ERR_FAIL_COND_V(!p_I, false);
	ERR_FAIL_COND_V(p_I->data != this, false);

	if (first == p_I) {
		first = p_I->next_ptr;
	}
	if (last == p_I) {
		last = p_I->prev_ptr;
	}
	if (p_I->prev_ptr) {
		p_I->prev_ptr->next_ptr = p_I->next_ptr;
	}
	if (p_I->next_ptr) {
		p_I->next_ptr->prev_ptr = p_I->prev_ptr;
	}
	// Deletion is handled by user code after NOTIFICATION_PREDELETE, not here.
	// memdelete(p_I);
	size_cache--;

	p_I->next_ptr = nullptr;
	p_I->prev_ptr = nullptr;
	p_I->data = nullptr;

	return true;
}

void LinkedList::create_from(const Variant &p_value) {
	clear();
	switch (p_value.get_type()) {
		case Variant::NIL: {
			// Do nothing.
		} break;
		case Variant::DICTIONARY: {
			List<Variant> keys;
			Dictionary dict = p_value;
			dict.get_key_list(&keys);
			for (List<Variant>::Element *E = keys.front(); E; E = E->next()) {
				ListNode *n = push_back(E->get()); // Key.
				n->set_meta("value", dict[E->get()]); // Value.
			}
		} break;
		case Variant::ARRAY:
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY: {
			Array arr = p_value;
			for (int i = 0; i < arr.size(); ++i) {
				push_back(arr[i]);
			}
		} break;
		default: {
			push_back(p_value);
		}
	}
}

ListNode *LinkedList::push_back(const Variant &value) {
	ListNode *n = memnew(ListNode);
	n->value = value;

	n->prev_ptr = _data->last;
	n->next_ptr = 0;
	n->data = _data;

	if (_data->last) {
		_data->last->next_ptr = n;
	}
	_data->last = n;

	if (!_data->first) {
		_data->first = n;
	}
	_data->size_cache++;

	return n;
}

void LinkedList::pop_back() {
	if (_data->last) {
		memdelete(_data->last);
	}
}

ListNode *LinkedList::push_front(const Variant &value) {
	ListNode *n = memnew(ListNode);
	n->value = value;
	n->prev_ptr = 0;
	n->next_ptr = _data->first;
	n->data = _data;

	if (_data->first) {
		_data->first->prev_ptr = n;
	}
	_data->first = n;

	if (!_data->last) {
		_data->last = n;
	}
	_data->size_cache++;

	return n;
}

void LinkedList::pop_front() {
	if (_data->first) {
		memdelete(_data->first);
	}
}

Array LinkedList::get_nodes() {
	Array nodes;
	ListNode *it = get_front();
	while (it) {
		nodes.push_back(it);
		it = it->get_next();
	}
	return nodes;
}

Array LinkedList::get_elements() {
	return get_nodes();
}

ListNode *LinkedList::insert_after(ListNode *p_node, const Variant &p_value) {
	ERR_FAIL_COND_V(!p_node, nullptr);
	ERR_FAIL_COND_V(p_node->data != _data, nullptr);

	ListNode *n = memnew(ListNode);
	n->value = p_value;
	n->prev_ptr = p_node;
	n->next_ptr = p_node->next_ptr;
	n->data = _data;

	if (!p_node->next_ptr) {
		_data->last = n;
	} else {
		p_node->next_ptr->prev_ptr = n;
	}
	p_node->next_ptr = n;

	_data->size_cache++;

	return n;
}

ListNode *LinkedList::insert_before(ListNode *p_node, const Variant &p_value) {
	ERR_FAIL_COND_V(!p_node, nullptr);
	ERR_FAIL_COND_V(p_node->data != _data, nullptr);

	ListNode *n = memnew(ListNode);
	n->value = p_value;
	n->prev_ptr = p_node->prev_ptr;
	n->next_ptr = p_node;
	n->data = _data;

	if (!p_node->prev_ptr) {
		_data->first = n;
	} else {
		p_node->prev_ptr->next_ptr = n;
	}
	p_node->prev_ptr = n;

	_data->size_cache++;

	return n;
}

ListNode *LinkedList::find(const Variant &p_value) {
	ListNode *it = get_front();
	while (it) {
		if (it->value == p_value) {
			return it;
		}
		it = it->get_next();
	}
	return nullptr;
}

bool LinkedList::erase(const Variant &p_value) {
	ListNode *I = find(p_value);
	if (I) {
		memdelete(I);
		return true;
	}
	return false;
}

void LinkedList::swap(ListNode *p_A, ListNode *p_B) {
	ERR_FAIL_COND(!p_A || !p_B);
	ERR_FAIL_COND(p_A->data != _data);
	ERR_FAIL_COND(p_B->data != _data);

	if (p_A == p_B) {
		return;
	}
	ListNode *A_prev = p_A->prev_ptr;
	ListNode *A_next = p_A->next_ptr;
	ListNode *B_prev = p_B->prev_ptr;
	ListNode *B_next = p_B->next_ptr;

	if (A_prev) {
		A_prev->next_ptr = p_B;
	} else {
		_data->first = p_B;
	}
	if (B_prev) {
		B_prev->next_ptr = p_A;
	} else {
		_data->first = p_A;
	}
	if (A_next) {
		A_next->prev_ptr = p_B;
	} else {
		_data->last = p_B;
	}
	if (B_next) {
		B_next->prev_ptr = p_A;
	} else {
		_data->last = p_A;
	}
	p_A->prev_ptr = A_next == p_B ? p_B : B_prev;
	p_A->next_ptr = B_next == p_A ? p_B : B_next;
	p_B->prev_ptr = B_next == p_A ? p_A : A_prev;
	p_B->next_ptr = A_next == p_B ? p_A : A_next;
}

void LinkedList::invert() {
	int s = size() / 2;
	ListNode *F = get_front();
	ListNode *B = get_back();
	for (int i = 0; i < s; i++) {
		SWAP(F->value, B->value);
		F = F->get_next();
		B = B->get_prev();
	}
}

void LinkedList::move_to_back(ListNode *p_I) {
	ERR_FAIL_COND(!p_I);
	ERR_FAIL_COND(p_I->data != _data);

	if (!p_I->next_ptr) {
		return;
	}
	if (_data->first == p_I) {
		_data->first = p_I->next_ptr;
	}
	if (_data->last == p_I) {
		_data->last = p_I->prev_ptr;
	}
	if (p_I->prev_ptr) {
		p_I->prev_ptr->next_ptr = p_I->next_ptr;
	}
	p_I->next_ptr->prev_ptr = p_I->prev_ptr;

	_data->last->next_ptr = p_I;
	p_I->prev_ptr = _data->last;
	p_I->next_ptr = nullptr;
	_data->last = p_I;
}

void LinkedList::move_to_front(ListNode *p_I) {
	ERR_FAIL_COND(!p_I);
	ERR_FAIL_COND(p_I->data != _data);

	if (!p_I->prev_ptr) {
		return;
	}
	if (_data->first == p_I) {
		_data->first = p_I->next_ptr;
	}
	if (_data->last == p_I) {
		_data->last = p_I->prev_ptr;
	}
	p_I->prev_ptr->next_ptr = p_I->next_ptr;

	if (p_I->next_ptr) {
		p_I->next_ptr->prev_ptr = p_I->prev_ptr;
	}
	_data->first->prev_ptr = p_I;
	p_I->next_ptr = _data->first;
	p_I->prev_ptr = nullptr;
	_data->first = p_I;
}

void LinkedList::move_before(ListNode *p_A, ListNode *p_B) {
	ERR_FAIL_COND(!p_A || !p_B);
	ERR_FAIL_COND(p_A->data != _data);
	ERR_FAIL_COND(p_B->data != _data);

	if (p_A->prev_ptr) {
		p_A->prev_ptr->next_ptr = p_A->next_ptr;
	} else {
		_data->first = p_A->next_ptr;
	}
	if (p_A->next_ptr) {
		p_A->next_ptr->prev_ptr = p_A->prev_ptr;
	} else {
		_data->last = p_A->prev_ptr;
	}
	p_A->next_ptr = p_B;
	if (!p_B) {
		p_A->prev_ptr = _data->last;
		_data->last = p_A;
		return;
	}
	p_A->prev_ptr = p_B->prev_ptr;

	if (p_B->prev_ptr) {
		p_B->prev_ptr->next_ptr = p_A;
	} else {
		_data->first = p_A;
	}
	p_B->prev_ptr = p_A;
}

void LinkedList::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_from", "value"), &LinkedList::create_from);

	ClassDB::bind_method(D_METHOD("get_front"), &LinkedList::get_front);
	ClassDB::bind_method(D_METHOD("get_back"), &LinkedList::get_back);

	ClassDB::bind_method(D_METHOD("push_back", "value"), &LinkedList::push_back);
	ClassDB::bind_method(D_METHOD("pop_back"), &LinkedList::pop_back);
	ClassDB::bind_method(D_METHOD("push_front", "value"), &LinkedList::push_front);
	ClassDB::bind_method(D_METHOD("pop_front"), &LinkedList::pop_front);

	ClassDB::bind_method(D_METHOD("get_nodes"), &LinkedList::get_nodes);
	ClassDB::bind_method(D_METHOD("get_elements"), &LinkedList::get_elements);

	ClassDB::bind_method(D_METHOD("insert_after", "node", "value"), &LinkedList::insert_after);
	ClassDB::bind_method(D_METHOD("insert_before", "node", "value"), &LinkedList::insert_before);

	ClassDB::bind_method(D_METHOD("find", "value"), &LinkedList::find);
	ClassDB::bind_method(D_METHOD("erase", "value"), &LinkedList::erase);

	ClassDB::bind_method(D_METHOD("empty"), &LinkedList::empty);
	ClassDB::bind_method(D_METHOD("clear"), &LinkedList::clear);
	ClassDB::bind_method(D_METHOD("size"), &LinkedList::size);

	ClassDB::bind_method(D_METHOD("swap", "node_A", "node_B"), &LinkedList::swap);
	ClassDB::bind_method(D_METHOD("invert"), &LinkedList::invert);

	ClassDB::bind_method(D_METHOD("move_to_front", "node"), &LinkedList::move_to_front);
	ClassDB::bind_method(D_METHOD("move_to_back", "node"), &LinkedList::move_to_back);
	ClassDB::bind_method(D_METHOD("move_before", "node", "before_node"), &LinkedList::move_before);

	ClassDB::bind_method(D_METHOD("sort"), &LinkedList::sort);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "front"), "", "get_front");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "back"), "", "get_back");

	ClassDB::bind_method(D_METHOD("_iter_init"), &LinkedList::_iter_init);
	ClassDB::bind_method(D_METHOD("_iter_get"), &LinkedList::_iter_get);
	ClassDB::bind_method(D_METHOD("_iter_next"), &LinkedList::_iter_next);
}

Variant LinkedList::_iter_init(const Array &p_iter) {
	_iter_current = get_front();
	return _iter_current != nullptr;
}

Variant LinkedList::_iter_next(const Array &p_iter) {
#ifdef DEBUG_ENABLED
	ERR_FAIL_NULL_V(_iter_current, Variant());
#endif
	_iter_current = _iter_current->get_next();
	return _iter_current != nullptr;
}

Variant LinkedList::_iter_get(const Variant &p_iter) {
#ifdef DEBUG_ENABLED
	ERR_FAIL_NULL_V(_iter_current, Variant());
#endif
	return _iter_current->get_value();
}

void LinkedList::clear() {
	while (get_front()) {
		memdelete(get_front());
	}
	_iter_current = nullptr;
}

String LinkedList::to_string() {
	String str("[");
	ListNode *it = get_front();
	while (it) {
		if (it != get_front()) {
			str += ", ";
		}
		str += String(it->get_value());
		it = it->get_next();
	}
	str += "]";
	return str;
}

void ListNode::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PREDELETE: {
			if (data) {
				data->remove(this);
			}
		} break;
	}
}

void ListNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_next"), &ListNode::get_next);
	ClassDB::bind_method(D_METHOD("get_prev"), &ListNode::get_prev);

	ClassDB::bind_method(D_METHOD("set_value", "value"), &ListNode::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &ListNode::get_value);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_value", "get_value");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "next"), "", "get_next");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "prev"), "", "get_prev");
}

LinkedList::LinkedList() {
	_data = memnew(ListData);
	_data->first = nullptr;
	_data->last = nullptr;
	_data->size_cache = 0;
}

LinkedList::~LinkedList() {
	clear();
	if (_data) {
		ERR_FAIL_COND(_data->size_cache);
		memdelete(_data);
	}
}
