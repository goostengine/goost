#include "random.h"

#include "core/local_vector.h"
#include "core/method_bind_ext.gen.inc"

Random *Random::singleton = nullptr;

uint32_t Random::get_number() {
	return randi();
}

real_t Random::get_value() {
	return randf();
}

Color Random::get_color() {
	Color color;
	// Pick not too pale and not too dark color.
	color.set_hsv(randf(), randf_range(0.5, 1.0), randf_range(0.5, 1.0));
	return color;
}

bool Random::get_condition() {
	return randf() >= 0.5;
}

Variant Random::range(const Variant &p_from, const Variant &p_to) {
	ERR_FAIL_COND_V_MSG(p_from.get_type() != p_to.get_type(), Variant(), "Incompatible types.");

	switch (p_from.get_type()) {
		case Variant::INT: {
			int from = p_from;
			int to = p_to;
			return randi_range(from, to);
		} break;
		case Variant::REAL: {
			real_t from = p_from;
			real_t to = p_to;
			return randf_range(from, to);
		} break;
		default: {
			Variant ret;
			Variant::interpolate(p_from, p_to, randf(), ret);
			return ret;
		}
	}
	return Variant();
}

Color Random::color_hsv(float h_min, float h_max, float s_min, float s_max, float v_min, float v_max, float a_min, float a_max) {
	Color color;
	color.set_hsv(
			randf_range(h_min, h_max),
			randf_range(s_min, s_max),
			randf_range(v_min, v_max),
			randf_range(a_min, a_max));
	return color;
}

Color Random::color_rgb(float r_min, float r_max, float g_min, float g_max, float b_min, float b_max, float a_min, float a_max) {
	return Color(
			randf_range(r_min, r_max),
			randf_range(g_min, g_max),
			randf_range(b_min, b_max),
			randf_range(a_min, a_max));
}

Variant Random::pick(const Variant &p_from) {
	switch (p_from.get_type()) {
		case Variant::STRING: {
			String str = p_from;
			ERR_FAIL_COND_V_MSG(str.empty(), Variant(), "String is empty.");
			return str.substr(randi() % str.length(), 1); // Not size().
		} break;
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY:
		case Variant::ARRAY: {
			Array arr = p_from;
			ERR_FAIL_COND_V_MSG(arr.empty(), Variant(), "Array is empty.");
			return arr[randi() % arr.size()];
		} break;
		case Variant::DICTIONARY: {
			Dictionary dict = p_from;
			ERR_FAIL_COND_V_MSG(dict.empty(), Variant(), "Dictionary is empty.");
			return dict.get_value_at_index(randi() % dict.size());
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: the type must be indexable.");
		}
	}
	return Variant();
}

Variant Random::pop(const Variant &p_from) {
	switch (p_from.get_type()) {
		case Variant::STRING: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: String is passed by value.");
		} break;
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: Pool*Arrays are passed by value rather than reference.");
		} break;
		case Variant::ARRAY: {
			Array arr = p_from;
			ERR_FAIL_COND_V_MSG(arr.empty(), Variant(), "Array is empty.");

			int idx = randi() % arr.size();
			Variant c = arr[idx];
			// Remove unordered for performance reasons.
			arr[idx] = arr.back();
			arr.pop_back();

			return c;
		} break;
		case Variant::DICTIONARY: {
			Dictionary dict = p_from;
			ERR_FAIL_COND_V_MSG(dict.empty(), Variant(), "Dictionary is empty.");

			int idx = randi() % dict.size();
			Variant c = dict.get_value_at_index(idx);
			dict.erase(dict.get_key_at_index(idx));

			return c;
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: the type must be indexable.");
		}
	}
	return Variant();
}

Array Random::choices(const Variant &p_from, int p_count, const PoolIntArray &p_weights, bool p_is_cumulative) {
	int sum = 0;
	LocalVector<int, int> cumulative_weights;
	LocalVector<int, int> weights;
	LocalVector<int, int> indices;
	Array weighted_choices;

	if ((p_from.get_type() == Variant::DICTIONARY) && p_weights.empty()) {
		Dictionary dict = p_from;
		Array w = dict.values();
		for (int i = 0; i < w.size(); i++) {
			weights.push_back(w[i]);
		}
	} else {
		for (int i = 0; i < p_weights.size(); i++) {
			weights.push_back(p_weights[i]);
		}
	}

	if (!weights.empty()) {
		if (p_is_cumulative) {
			sum = weights[weights.size() - 1];
			cumulative_weights = weights;
		} else {
			for (int i = 0; i < weights.size(); i++) {
				if (weights[i] < 0) {
					ERR_FAIL_V_MSG(Array(), "Weights must be positive integers.");
				} else {
					sum += weights[i];
					cumulative_weights.push_back(sum);
				}
			}
		}

		for (int i = 0; i < p_count; i++) {
			int left = 0;
			int right = weights.size();
			int random_number = randi() % sum;
			// Bisect.
			while (left < right) {
				int mid = (left + right) / 2;
				if (cumulative_weights[mid] < random_number) {
					left = mid + 1;
				} else {
					right = mid;
				}
			}
			indices.push_back(left);
		}
	}

	switch (p_from.get_type()) {
		case Variant::STRING: {
			String str = p_from;
			ERR_FAIL_COND_V_MSG(str.empty(), Variant(), "String is empty.");
			if (weights.empty()) {
				for (int i = 0; i < p_count; i++) {
					weighted_choices.push_back(str.substr((randi() % str.length()), 1));
				}
			} else {
				ERR_FAIL_COND_V_MSG(str.length() != weights.size(), Variant(), "Size of weights does not match.");
				for (int i = 0; i < p_count; i++) {
					weighted_choices.push_back(str.substr(indices[i], 1));
				}
			}
			return weighted_choices;
		} break;
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY:
		case Variant::ARRAY: {
			Array arr = p_from;
			ERR_FAIL_COND_V_MSG(arr.empty(), Variant(), "Array is empty.");

			if (weights.empty()) {
				for (int i = 0; i < p_count; i++) {
					weighted_choices.push_back(arr[randi() % arr.size()]);
				}
			} else {
				ERR_FAIL_COND_V_MSG(arr.size() != weights.size(), Variant(), "Size of weights does not match.");
				for (int i = 0; i < p_count; i++) {
					weighted_choices.push_back(arr[indices[i]]);
				}
			}
			return weighted_choices;
		} break;
		case Variant::DICTIONARY: {
			Dictionary dict = p_from;
			ERR_FAIL_COND_V_MSG(dict.empty(), Variant(), "Dictionary is empty.");
			for (int i = 0; i < p_count; i++) {
				weighted_choices.push_back(dict.get_key_at_index(indices[i]));
			}
			return weighted_choices;
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: the type must be indexable.");
		}
	}
	return Array();
}

void Random::shuffle(Array p_array) {
	if (p_array.size() < 2) {
		return;
	}
	for (int i = p_array.size() - 1; i > 0; --i) {
		const uint32_t j = randi() % (i + 1);
		const Variant tmp = p_array[i];
		p_array[i] = p_array[j];
		p_array[j] = tmp;
	}
}

bool Random::decision(float probability) {
	return randf() <= probability;
}

void Random::_bind_methods() {
	ClassDB::bind_method(D_METHOD("new_instance"), &Random::new_instance);

	ClassDB::bind_method(D_METHOD("get_value"), &Random::get_value);
	ClassDB::bind_method(D_METHOD("get_number"), &Random::get_number);
	ClassDB::bind_method(D_METHOD("get_color"), &Random::get_color);
	ClassDB::bind_method(D_METHOD("get_condition"), &Random::get_condition);

	ClassDB::bind_method(D_METHOD("color_hsv", "hue_min", "hue_max", "saturation_min", "saturation_max", "value_min", "value_max", "alpha_min", "alpha_max"),
			&Random::color_hsv, DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(1.0), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("color_rgb", "red_min", "red_max", "green_min", "green_max", "blue_min", "blue_max", "alpha_min", "alpha_max"),
			&Random::color_rgb, DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(1.0), DEFVAL(1.0));

	ClassDB::bind_method(D_METHOD("range", "from", "to"), &Random::range);
	ClassDB::bind_method(D_METHOD("pick", "from"), &Random::pick);
	ClassDB::bind_method(D_METHOD("pop", "from"), &Random::pop);
	ClassDB::bind_method(D_METHOD("choices", "from", "count", "weights", "cumulative"), &Random::choices, DEFVAL(1), DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("shuffle", "array"), &Random::shuffle);
	ClassDB::bind_method(D_METHOD("decision", "probability"), &Random::decision);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "number"), "", "get_number");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "value"), "", "get_value");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "", "get_color");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "condition"), "", "get_condition");

	// Default values are non-deterministic, override those for documentation purposes.
	ADD_PROPERTY_DEFAULT("number", 37);
	ADD_PROPERTY_DEFAULT("value", 0.5);
	ADD_PROPERTY_DEFAULT("color", Color(0, 0, 1));
	ADD_PROPERTY_DEFAULT("condition", true);
	// Have to override in base class as well.
	ADD_PROPERTY_DEFAULT("seed", 0);
	ADD_PROPERTY_DEFAULT("state", 0);
}
