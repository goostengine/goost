#pragma once

#include "core/resource.h"
#include "core/variant.h"

class Map2D : public Resource {
	GDCLASS(Map2D, Resource);

protected:
	static void _bind_methods();

private:
	Vector<Variant> data;
	int width = 0;
	int height = 0;

protected:
	// Custom iterator for scripting.
	int _iter_current = 0;
	Variant _iter_init(const Array &p_iter);
	Variant _iter_next(const Array &p_iter);
	Variant _iter_get(const Variant &p_iter);

	// Storage.
	void _set_data(const Dictionary &p_data);
	Dictionary _get_data() const;

public:
	void create(int p_width, int p_height);
	void create_from_data(int p_width, int p_height, Array p_data);
	void resize(int p_new_width, int p_new_height);

	_FORCE_INLINE_ void set_element(int p_x, int p_y, const Variant &p_value);
	_FORCE_INLINE_ Variant get_element(int p_x, int p_y);

	void set_cell(const Vector2 &p_pos, const Variant &p_value);
	Variant get_cell(const Vector2 &p_pos);
	Variant get_cell_or_null(const Vector2 &p_pos);
	bool has_cell(const Vector2 &p_pos);

	void fill(const Variant &p_value);

	int get_width() const { return width; }
	int get_height() const { return height; }
	Vector2 get_size() const { return Vector2(width, height); }

	bool is_empty() const { return width == 0 && height == 0; }
	void clear();

	virtual String to_string();
};

