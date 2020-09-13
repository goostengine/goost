#include "list.h"

bool ListData::erase(ListElement *p_I) {
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
	memdelete(p_I);
	size_cache--;

	return true;
}

ListElement *LinkedList::push_back(const Variant &value) {
	if (!_data) {
		_data = memnew(ListData);
		_data->first = nullptr;
		_data->last = nullptr;
		_data->size_cache = 0;
	}
	ListElement *n = memnew(ListElement);
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
	if (_data && _data->last) {
		remove(_data->last);
	}
}

ListElement *LinkedList::push_front(const Variant &value) {
	if (!_data) {
		_data = memnew(ListData);
		_data->first = nullptr;
		_data->last = nullptr;
		_data->size_cache = 0;
	}
	ListElement *n = memnew(ListElement);
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
	if (_data && _data->first) {
		remove(_data->first);
	}
}

bool LinkedList::remove(ListElement *p_I) {
	if (_data && p_I) {
		bool ret = _data->erase(p_I);
		if (_data->size_cache == 0) {
			memdelete(_data);
			_data = nullptr;
		}
		return ret;
	}
	return false;
};

ListElement *LinkedList::find(const Variant &p_value) {
	ListElement *it = get_front();
	while (it) {
		if (it->value == p_value) {
			return it;
		}
		it = it->get_next();
	}
	return nullptr;
}

bool LinkedList::erase(const Variant &p_value) {
	ListElement *I = find(p_value);
	return remove(I);
}

void LinkedList::swap(ListElement *p_A, ListElement *p_B) {
	ERR_FAIL_COND(!p_A || !p_B);
	ERR_FAIL_COND(p_A->data != _data);
	ERR_FAIL_COND(p_B->data != _data);

	if (p_A == p_B) {
		return;
	}
	ListElement *A_prev = p_A->prev_ptr;
	ListElement *A_next = p_A->next_ptr;
	ListElement *B_prev = p_B->prev_ptr;
	ListElement *B_next = p_B->next_ptr;

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
	ListElement *F = get_front();
	ListElement *B = get_back();
	for (int i = 0; i < s; i++) {
		SWAP(F->value, B->value);
		F = F->get_next();
		B = B->get_prev();
	}
}

void LinkedList::move_to_back(ListElement *p_I) {
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

void LinkedList::move_to_front(ListElement *p_I) {
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

void LinkedList::move_before(ListElement *p_A, ListElement *p_B) {
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
	ClassDB::bind_method(D_METHOD("get_front"), &LinkedList::get_front);
	ClassDB::bind_method(D_METHOD("get_back"), &LinkedList::get_back);

	ClassDB::bind_method(D_METHOD("push_back", "value"), &LinkedList::push_back);
	ClassDB::bind_method(D_METHOD("pop_back"), &LinkedList::pop_back);
	ClassDB::bind_method(D_METHOD("push_front", "value"), &LinkedList::push_front);
	ClassDB::bind_method(D_METHOD("pop_front"), &LinkedList::pop_front);

	ClassDB::bind_method(D_METHOD("find", "value"), &LinkedList::find);
	ClassDB::bind_method(D_METHOD("erase", "value"), &LinkedList::erase);
	ClassDB::bind_method(D_METHOD("remove", "element"), &LinkedList::remove);

	ClassDB::bind_method(D_METHOD("empty"), &LinkedList::empty);
	ClassDB::bind_method(D_METHOD("clear"), &LinkedList::clear);
	ClassDB::bind_method(D_METHOD("size"), &LinkedList::size);

	ClassDB::bind_method(D_METHOD("swap"), &LinkedList::swap);
	ClassDB::bind_method(D_METHOD("invert"), &LinkedList::invert);

	ClassDB::bind_method(D_METHOD("move_to_front"), &LinkedList::move_to_front);
	ClassDB::bind_method(D_METHOD("move_to_back"), &LinkedList::move_to_back);
	ClassDB::bind_method(D_METHOD("move_before"), &LinkedList::move_before);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "front"), "", "get_front");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "back"), "", "get_back");
}

void LinkedList::clear() {
	while (get_front()) {
		remove(get_front());
	}
}

void ListElement::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_next"), &ListElement::get_next);
	ClassDB::bind_method(D_METHOD("get_prev"), &ListElement::get_prev);

	ClassDB::bind_method(D_METHOD("set_value", "value"), &ListElement::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &ListElement::get_value);

	ClassDB::bind_method(D_METHOD("erase"), &ListElement::erase);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_value", "get_value");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "next"), "", "get_next");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "prev"), "", "get_prev");
}

LinkedList::~LinkedList() {
	clear();
	if (_data) {
		ERR_FAIL_COND(_data->size_cache);
		memdelete(_data);
	}
}
