#include "variant_map.h"

void VariantMap::create(int p_width, int p_height) {
	clear();
	resize(p_width, p_height);
}

void VariantMap::create_from_data(int p_width, int p_height, Array p_data) {
	ERR_FAIL_COND_MSG(p_data.empty(), "Array is empty.");
	ERR_FAIL_COND_MSG(p_data.size() != p_width * p_height, "Element count mismatch in the Array to create a VariantMap.");

	create(p_width, p_height);

	Variant *dest = data.ptrw();
	for (int i = 0; i < p_data.size(); ++i) {
		dest[i] = p_data[i];
	}
}

void VariantMap::resize(int p_new_width, int p_new_height) {
	ERR_FAIL_COND(p_new_width <= 0);
	ERR_FAIL_COND(p_new_height <= 0);

	if (p_new_width == width && p_new_height == height) {
		return;
	}
	Error err = data.resize(p_new_width * p_new_height);
	if (err != OK) {
		return;
	}
	width = p_new_width;
	height = p_new_height;
}

void VariantMap::set_element(int p_x, int p_y, const Variant &p_value) {
#ifdef DEBUG_ENABLED
	ERR_FAIL_COND(p_x < 0);
	ERR_FAIL_COND(p_y < 0);
	ERR_FAIL_COND(p_x >= width);
	ERR_FAIL_COND(p_y >= height);
#endif
	uint32_t ofs = p_y * width + p_x;
	return data.write[ofs] = p_value;
}

Variant VariantMap::get_element(int p_x, int p_y) {
#ifdef DEBUG_ENABLED
	ERR_FAIL_COND_V(p_x < 0, Variant());
	ERR_FAIL_COND_V(p_y < 0, Variant());
	ERR_FAIL_COND_V(p_x >= width, Variant());
	ERR_FAIL_COND_V(p_y >= height, Variant());
#endif
	uint32_t ofs = p_y * width + p_x;
	return data[ofs];
}

void VariantMap::set_cell(const Vector2 &p_pos, const Variant &p_value) {
	set_element(p_pos.x, p_pos.y, p_value);
}

Variant VariantMap::get_cell(const Vector2 &p_pos) {
	return get_element(p_pos.x, p_pos.y);
}

Variant VariantMap::get_cell_or_null(const Vector2 &p_pos) {
	if (!has_cell(p_pos)) {
		return Variant();
	}
	return get_element(p_pos.x, p_pos.y);
}

bool VariantMap::has_cell(const Vector2 &p_pos) {
	return p_pos.x >= 0 && p_pos.y >= 0 && p_pos.x < width && p_pos.y < height;
}

void VariantMap::fill(const Variant &p_value) {
	int idx = 0;
	const int count = width * height;
	Variant *dest = data.ptrw();
	while (idx < count) {
		dest[idx++] = p_value;
	}
}

void VariantMap::clear() {
	data.clear();
	width = 0;
	height = 0;
}

Variant VariantMap::_iter_init(const Array &p_iter) {
	_iter_current = 0;
	return _iter_current < width * height;
}

Variant VariantMap::_iter_next(const Array &p_iter) {
	_iter_current++;
	return _iter_current < width * height;
}

Variant VariantMap::_iter_get(const Variant &p_iter) {
#ifdef DEBUG_ENABLED
	ERR_FAIL_COND_V(p_iter.get_type() != Variant::INT, Variant());
	int size = p_iter;
	ERR_FAIL_INDEX_V(size, data.size(), Variant());
#endif
	return data[_iter_current];
}

void VariantMap::_set_data(const Dictionary &p_data) {
	ERR_FAIL_COND(!p_data.has("width"));
	ERR_FAIL_COND(!p_data.has("height"));
	ERR_FAIL_COND(!p_data.has("data"));

	int w = p_data["width"];
	int h = p_data["height"];
	Array arr = p_data["data"];

	create_from_data(w, h, arr);
}

Dictionary VariantMap::_get_data() const {
	Dictionary ret;
	ret["width"] = width;
	ret["height"] = height;
	Array arr;
	for (int i = 0; i < data.size(); ++i) {
		arr.push_back(data[i]);
	}
	ret["data"] = arr;
	return ret;
}

String VariantMap::to_string() {
	if (is_empty()) {
		return "[]";
	}
	String str;
	for (int y = 0; y < height; ++y) {
		str += "[";
		for (int x = 0; x < width; ++x) {
			uint32_t ofs = y * width + x;
			str += String(data[ofs]);
			if (x != width - 1) {
				str += ", ";
			} else {
				str += "]";
			}
		}
		str += "\n";
	}
	return str;
}

void VariantMap::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create", "width", "height"), &VariantMap::create);
	ClassDB::bind_method(D_METHOD("create_from_data", "width", "height", "data"), &VariantMap::create_from_data);
	ClassDB::bind_method(D_METHOD("resize", "new_width", "new_height"), &VariantMap::resize);

	ClassDB::bind_method(D_METHOD("set_element", "x", "y", "value"), &VariantMap::set_element);
	ClassDB::bind_method(D_METHOD("get_element", "x", "y"), &VariantMap::get_element);

	ClassDB::bind_method(D_METHOD("set_cell", "position", "value"), &VariantMap::set_cell);
	ClassDB::bind_method(D_METHOD("get_cell", "position"), &VariantMap::get_cell);
	ClassDB::bind_method(D_METHOD("get_cell_or_null", "position"), &VariantMap::get_cell_or_null);
	ClassDB::bind_method(D_METHOD("has_cell", "position"), &VariantMap::has_cell);

	ClassDB::bind_method(D_METHOD("fill", "with_value"), &VariantMap::fill);

	ClassDB::bind_method(D_METHOD("get_width"), &VariantMap::get_width);
	ClassDB::bind_method(D_METHOD("get_height"), &VariantMap::get_height);
	ClassDB::bind_method(D_METHOD("get_size"), &VariantMap::get_size);

	ClassDB::bind_method(D_METHOD("is_empty"), &VariantMap::is_empty);
	ClassDB::bind_method(D_METHOD("clear"), &VariantMap::clear);

	ClassDB::bind_method(D_METHOD("_iter_init"), &VariantMap::_iter_init);
	ClassDB::bind_method(D_METHOD("_iter_get"), &VariantMap::_iter_get);
	ClassDB::bind_method(D_METHOD("_iter_next"), &VariantMap::_iter_next);

	ClassDB::bind_method(D_METHOD("_set_data", "data"), &VariantMap::_set_data);
	ClassDB::bind_method(D_METHOD("_get_data"), &VariantMap::_get_data);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "_set_data", "_get_data");
}
