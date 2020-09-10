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

ListElement *LinkedList::front() {
	return _data ? _data->first : 0;
}

ListElement *LinkedList::back() {
	return _data ? _data->last : 0;
}

ListElement *LinkedList::push_back(const Variant &value) {
	if (!_data) {
		_data = memnew_allocator(ListData, DefaultAllocator);
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
		erase(_data->last);
	}
}

ListElement *LinkedList::push_front(const Variant &value) {
	if (!_data) {
		_data = memnew_allocator(ListData, DefaultAllocator);
		_data->first = nullptr;
		_data->last = nullptr;
		_data->size_cache = 0;
	}
	ListElement *n = memnew_allocator(ListElement, DefaultAllocator);
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
		erase(_data->first);
	}
}

bool LinkedList::erase(ListElement *p_I) {
	if (_data) {
		bool ret = _data->erase(p_I);
		if (_data->size_cache == 0) {
			memdelete_allocator<ListData, DefaultAllocator>(_data);
			_data = nullptr;
		}
		return ret;
	}
	return false;
};

void LinkedList::_bind_methods() {
	ClassDB::bind_method(D_METHOD("front"), &LinkedList::front);
	ClassDB::bind_method(D_METHOD("back"), &LinkedList::back);
	ClassDB::bind_method(D_METHOD("push_back", "value"), &LinkedList::push_back);
	ClassDB::bind_method(D_METHOD("pop_back"), &LinkedList::pop_back);
	ClassDB::bind_method(D_METHOD("push_front", "value"), &LinkedList::push_front);
	ClassDB::bind_method(D_METHOD("pop_front"), &LinkedList::pop_front);
	ClassDB::bind_method(D_METHOD("erase", "element"), &LinkedList::erase);
}

void ListElement::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_value", "value"), &ListElement::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &ListElement::get_value);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "set_value", "get_value");
}
