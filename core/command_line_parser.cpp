#include "command_line_parser.h"

#include "core/os/os.h"

// CommandLineOption

void CommandLineOption::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_names", "names"), &CommandLineOption::set_names);
	ClassDB::bind_method(D_METHOD("get_names"), &CommandLineOption::get_names);

	ClassDB::bind_method(D_METHOD("set_default_args", "args"), &CommandLineOption::set_default_args);
	ClassDB::bind_method(D_METHOD("get_default_args"), &CommandLineOption::get_default_args);

	ClassDB::bind_method(D_METHOD("set_allowed_args", "args"), &CommandLineOption::set_allowed_args);
	ClassDB::bind_method(D_METHOD("get_allowed_args"), &CommandLineOption::get_allowed_args);

	ClassDB::bind_method(D_METHOD("set_description", "description"), &CommandLineOption::set_description);
	ClassDB::bind_method(D_METHOD("get_description"), &CommandLineOption::get_description);

	ClassDB::bind_method(D_METHOD("set_category", "category"), &CommandLineOption::set_category);
	ClassDB::bind_method(D_METHOD("get_category"), &CommandLineOption::get_category);

	ClassDB::bind_method(D_METHOD("set_arg_text", "arg_text"), &CommandLineOption::set_arg_text);
	ClassDB::bind_method(D_METHOD("get_arg_text"), &CommandLineOption::get_arg_text);

	ClassDB::bind_method(D_METHOD("set_arg_count", "count"), &CommandLineOption::set_arg_count);
	ClassDB::bind_method(D_METHOD("get_arg_count"), &CommandLineOption::get_arg_count);

	ClassDB::bind_method(D_METHOD("set_hidden", "hidden"), &CommandLineOption::set_hidden);
	ClassDB::bind_method(D_METHOD("is_hidden"), &CommandLineOption::is_hidden);

	ClassDB::bind_method(D_METHOD("set_positional", "positional"), &CommandLineOption::set_positional);
	ClassDB::bind_method(D_METHOD("is_positional"), &CommandLineOption::is_positional);

	ClassDB::bind_method(D_METHOD("set_required", "required"), &CommandLineOption::set_required);
	ClassDB::bind_method(D_METHOD("is_required"), &CommandLineOption::is_required);

	ClassDB::bind_method(D_METHOD("set_multitoken", "multitoken"), &CommandLineOption::set_multitoken);
	ClassDB::bind_method(D_METHOD("is_multitoken"), &CommandLineOption::is_multitoken);

	ClassDB::bind_method(D_METHOD("set_as_meta", "meta"), &CommandLineOption::set_as_meta);
	ClassDB::bind_method(D_METHOD("is_meta"), &CommandLineOption::is_meta);

	ClassDB::bind_method(D_METHOD("add_name", "name"), &CommandLineOption::add_name);
	ClassDB::bind_method(D_METHOD("add_default_arg", "arg"), &CommandLineOption::add_default_arg);
	ClassDB::bind_method(D_METHOD("add_allowed_arg", "arg"), &CommandLineOption::add_allowed_arg);

	ADD_PROPERTY(PropertyInfo(Variant::POOL_STRING_ARRAY, "names"), "set_names", "get_names");
	ADD_PROPERTY(PropertyInfo(Variant::POOL_STRING_ARRAY, "default_args"), "set_default_args", "get_default_args");
	ADD_PROPERTY(PropertyInfo(Variant::POOL_STRING_ARRAY, "allowed_args"), "set_allowed_args", "get_allowed_args");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "category"), "set_category", "get_category");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "arg_text"), "set_arg_text", "get_arg_text");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "arg_count"), "set_arg_count", "get_arg_count");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "hidden"), "set_hidden", "is_hidden");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "positional"), "set_positional", "is_positional");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "required"), "set_required", "is_required");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "multitoken"), "set_multitoken", "is_multitoken");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "meta"), "set_as_meta", "is_meta");

	ADD_SIGNAL(MethodInfo("parsed", PropertyInfo(Variant::POOL_STRING_ARRAY, "values")));
}

void CommandLineOption::set_names(const PoolStringArray &p_names) {
	_names.resize(0);
	for (int i = 0; i < p_names.size(); ++i) {
		add_name(p_names[i]);
	}
}

void CommandLineOption::add_name(const String &p_name) {
	ERR_FAIL_COND_MSG(p_name.empty(), "Option name cannot be empty.");
	ERR_FAIL_COND_MSG(p_name.find_char(' ') != -1, "Option name cannot contain spaces: " + p_name);

	_names.push_back(p_name);
}

void CommandLineOption::add_default_arg(const String &p_arg) {
	_default_args.push_back(p_arg);
}

void CommandLineOption::add_allowed_arg(const String &p_arg) {
	_allowed_args.push_back(p_arg);
}

CommandLineOption::CommandLineOption(const PoolStringArray &p_names, int p_arg_count) :
		_names(p_names),
		_arg_count(p_arg_count) {}

// CommandLineHelpFormat

void CommandLineHelpFormat::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_header", "header"), &CommandLineHelpFormat::set_header);
	ClassDB::bind_method(D_METHOD("get_header"), &CommandLineHelpFormat::get_header);

	ClassDB::bind_method(D_METHOD("set_footer", "footer"), &CommandLineHelpFormat::set_footer);
	ClassDB::bind_method(D_METHOD("get_footer"), &CommandLineHelpFormat::get_footer);

	ClassDB::bind_method(D_METHOD("set_usage_title", "name"), &CommandLineHelpFormat::set_usage_title);
	ClassDB::bind_method(D_METHOD("get_usage_title"), &CommandLineHelpFormat::get_usage_title);

	ClassDB::bind_method(D_METHOD("set_left_pad", "size"), &CommandLineHelpFormat::set_left_pad);
	ClassDB::bind_method(D_METHOD("get_left_pad"), &CommandLineHelpFormat::get_left_pad);

	ClassDB::bind_method(D_METHOD("set_right_pad", "size"), &CommandLineHelpFormat::set_right_pad);
	ClassDB::bind_method(D_METHOD("get_right_pad"), &CommandLineHelpFormat::get_right_pad);

	ClassDB::bind_method(D_METHOD("set_line_length", "size"), &CommandLineHelpFormat::set_line_length);
	ClassDB::bind_method(D_METHOD("get_line_length"), &CommandLineHelpFormat::get_line_length);

	ClassDB::bind_method(D_METHOD("set_min_description_length", "size"), &CommandLineHelpFormat::set_min_description_length);
	ClassDB::bind_method(D_METHOD("get_min_description_length"), &CommandLineHelpFormat::get_min_description_length);

	ClassDB::bind_method(D_METHOD("set_autogenerate_usage", "generate"), &CommandLineHelpFormat::set_autogenerate_usage);
	ClassDB::bind_method(D_METHOD("is_usage_autogenerated"), &CommandLineHelpFormat::is_usage_autogenerated);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "header"), "set_header", "get_header");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "footer"), "set_footer", "get_footer");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "usage_title"), "set_usage_title", "get_usage_title");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "left_pad"), "set_left_pad", "get_left_pad");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "right_pad"), "set_right_pad", "get_right_pad");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "line_length"), "set_line_length", "get_line_length");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "min_description_length"), "set_min_description_length", "get_min_description_length");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "autogenerate_usage"), "set_autogenerate_usage", "is_usage_autogenerated");
}

// Represents detected option prefix.
struct CommandLineParser::ParsedPrefix {
	String string;
	bool is_short = false;

	_FORCE_INLINE_ bool exists() const {
		return !string.empty();
	}

	_FORCE_INLINE_ int length() const {
		return string.length();
	}
};

// CommandLineParser

// TODO: can be ported to PackedStringArray::find() method in Godot 4.0.
static int find_arg(const PoolStringArray &p_args, const String &p_arg) {
	for (int i = 0; i < p_args.size(); ++i) {
		if (p_args[i] == p_arg) {
			return i;
		}
	}
	return -1;
}

// TODO: can be ported to PackedStringArray::has() method in Godot 4.0.
static bool has_arg(const PoolStringArray &p_args, const String &p_arg) {
	for (int i = 0; i < p_args.size(); ++i) {
		if (p_args[i] == p_arg) {
			return true;
		}
	}
	return false;
}

// TODO: can be ported to PackedStringArray::join() method in Godot 4.0.
static String join_args(const PoolStringArray &p_args) {
	Vector<String> to_join;
	for (int i = 0; i < p_args.size(); ++i) {
		to_join.push_back(p_args[i]);
	}
	return String(",").join(to_join);
}

bool CommandLineParser::_are_options_valid() const {
	ERR_FAIL_COND_V_MSG(_short_prefixes.empty(), false, "Short prefixes cannot be empty");
	ERR_FAIL_COND_V_MSG(_long_prefixes.empty(), false, "Long prefixes cannot be empty");

	for (int i = 0; i < _options.size(); ++i) {
		const CommandLineOption *option = _options[i].ptr();
		const PoolStringArray default_args = option->get_default_args();

		ERR_FAIL_COND_V_MSG(option->is_positional() && option->get_arg_count() == 0, false,
				vformat("Option '%s' cannot be positional and take no arguments.", _to_string(option->get_names())));
		ERR_FAIL_COND_V_MSG(option->get_names().empty(), false, vformat("Option at index %d does not have any name.", i));

		ERR_FAIL_COND_V_MSG(!default_args.empty() && default_args.size() != option->get_arg_count(), false,
				vformat("Option '%s' has %d default arguments, but requires %d.", _to_string(option->get_names()), default_args.size(), option->get_arg_count()));
		ERR_FAIL_COND_V_MSG(!default_args.empty() && option->is_required(), false,
				vformat("Option '%s' cannot have default arguments and be required.", _to_string(option->get_names())));

		const PoolStringArray allowed_args = option->get_allowed_args();
		for (int j = 0; j < default_args.size(); ++j) {
			if (!allowed_args.empty() && find_arg(allowed_args, default_args[j]) == -1) {
				ERR_PRINT(vformat("Option '%s' cannot have default argument '%s', because it's not allowed.", _to_string(option->get_names()), default_args[j]));
				return false;
			}
		}

		// Compare with other options.
		// TODO: all of this can be ported to PackedStringArray::operator==() in Godot 4.0.
		for (int j = i + 1; j < _options.size(); ++j) {
			const CommandLineOption *another_option = _options[j].ptr();

			PoolStringArray opt_names = option->get_names();
			PoolStringArray another_opt_names = another_option->get_names();

			bool same_name = true;

			if (opt_names.size() != another_opt_names.size()) {
				same_name = false;
			} else {
				for (int k = 0; k < opt_names.size(); ++k) {
					if (opt_names[k] != another_opt_names[k]) {
						same_name = false;
						break;
					}
				}
			}
			ERR_FAIL_COND_V_MSG(same_name, false, vformat("Found several options with the same name: '%s' and '%s'.", _to_string(option->get_names()), _to_string(another_option->get_names())));
		}
	}
	return true;
}

void CommandLineParser::_read_default_args() {
	for (int i = 0; i < _options.size(); ++i) {
		const CommandLineOption *option = _options[i].ptr();
		if (!_parsed_values.has(option)) {
			const PoolStringArray default_args = option->get_default_args();
			if (!default_args.empty()) {
				_parsed_values[option] = default_args;
			}
		}
	}
}

int CommandLineParser::_validate_arguments(int p_current_idx) {
	const String &current_arg = _args[p_current_idx];
	const ParsedPrefix prefix = _parse_prefix(current_arg);

	if (!prefix.exists()) {
		return _validate_positional(current_arg, p_current_idx);
	}
	if (_allow_adjacent) {
		const int separator = current_arg.find("=", prefix.length());
		if (separator != -1) {
			return _validate_adjacent(current_arg, prefix.string, separator);
		}
	}
	if (prefix.is_short) {
		return _validate_short(current_arg, prefix.string, p_current_idx);
	}
	return _validate_long(current_arg, prefix.string, p_current_idx);
}

int CommandLineParser::_validate_positional(const String &p_arg, int p_current_idx) {
	for (int i = 0; i < _options.size(); ++i) {
		const CommandLineOption *option = _options[i].ptr();

		if (option->is_positional() && (option->is_multitoken() || !_parsed_values.has(option))) {
			const int args_taken = _validate_option_args(option, _to_string(option->get_names()), p_current_idx);
			if (args_taken > 0) {
				_save_parsed_option(option, p_current_idx, args_taken);
			}
			return args_taken;
		}
	}
	// No unparsed positional option found.
	_error_text = vformat(RTR("Unexpected argument: '%s'."), p_arg);
	return -1;
}

int CommandLineParser::_validate_adjacent(const String &p_arg, const String &p_prefix, int p_separator) {
	if (unlikely(p_separator == p_arg.length() - 1)) {
		_error_text = vformat(RTR("Missing argument after '%s"), p_arg);
		return -1;
	}
	const String &option_name = p_arg.substr(p_prefix.length(), p_separator - p_prefix.length());
	const CommandLineOption *option = _validate_option(option_name, p_prefix);
	if (unlikely(!option)) {
		return -1;
	}
	if (option->get_arg_count() != 1 && option->get_arg_count() != -1) {
		_error_text = vformat(RTR("Argument separator '=' can be used only for single argument, but option '%s' accepts %d arguments"), option_name, option->get_arg_count());
		return -1;
	}
	const String &value = p_arg.substr(p_separator + 1);
	if (unlikely(!_validate_option_arg(option, p_prefix + option_name, value))) {
		return -1;
	}
	_save_parsed_option(option, p_prefix, value);
	return 1;
}

int CommandLineParser::_validate_short(const String &p_arg, const String &p_prefix, int p_current_idx) {
	// Take each symbol as a option (to allow arguments like -aux).
	for (int i = p_prefix.length(); i < p_arg.length(); i++) {
		if (unlikely(!_allow_compound && i == p_prefix.length() + 1)) {
			// With compound arguments disabled, the loop should only execute once.
			_error_text = vformat(RTR("Unexpected text '%s' after '%s'"), p_arg.right(p_prefix.length() + 1), p_arg.left(p_prefix.length() + 1));
			return -1;
		}
		const String option_name = String::chr(p_arg[i]);
		const CommandLineOption *option = _validate_option(option_name, p_prefix);
		if (unlikely(!option)) {
			return -1;
		}
		if (option->get_arg_count() != 0) {
			const String sticky_arg = p_arg.substr(i + 1); // Handle sticky arguments (e.g. -ovalue), empty if not present.
			const String display_name = p_prefix + option_name;
			if (!sticky_arg.empty()) {
				// Validate sticky argument first if present.
				if (unlikely(!_allow_sticky)) {
					_error_text = vformat(RTR("Missing space between '%s' and '%s"), p_prefix + option_name, sticky_arg);
					return -1;
				}
				if (unlikely(!_validate_option_arg(option, display_name, sticky_arg))) {
					return -1;
				}
			}
			int args_taken = _validate_option_args(option, display_name, p_current_idx + 1, !sticky_arg.empty());
			if (args_taken != -1) {
				_save_parsed_option(option, p_prefix, p_current_idx + 1, args_taken, sticky_arg);
				++args_taken; // Count option as taken argument.
			}
			return args_taken;
		}
		_save_parsed_option(option, p_prefix);
	}
	return 1;
}

int CommandLineParser::_validate_long(const String &p_arg, const String &p_prefix, int p_current_idx) {
	const CommandLineOption *option = _validate_option(p_arg.substr(p_prefix.length()), p_prefix);
	if (unlikely(!option)) {
		return -1;
	}
	int args_taken = _validate_option_args(option, p_arg, p_current_idx + 1);
	if (args_taken != -1) {
		_save_parsed_option(option, p_prefix, p_current_idx + 1, args_taken);
		++args_taken; // Count option as taken argument.
	}
	return args_taken;
}

const CommandLineOption *CommandLineParser::_validate_option(const String &p_name, const String &p_prefix) {
	const CommandLineOption *option = find_option(p_name).ptr();
	if (unlikely(!option)) {
		_error_text = vformat(RTR("'%s' is not a valid option."), p_prefix + p_name);
		// Try to suggest the correct option.
		const String similar_name = _find_most_similar(p_name);
		if (!similar_name.empty()) {
			_error_text += "\n";
			_error_text += vformat(RTR("Perhaps you wanted to use: '%s'."), p_prefix + similar_name);
		}
		return nullptr;
	}
	if (unlikely(!option->is_multitoken() && _parsed_values.has(option))) {
		_error_text = vformat(RTR("Option '%s' has been specified more than once."), p_prefix + p_name);
		return nullptr;
	}
	return option;
}

int CommandLineParser::_validate_option_args(const CommandLineOption *p_option, const String &p_display_name, int p_current_idx, bool p_skip_first) {
	int validated_arg_count = 0;
	int available_args = _args.size() - p_current_idx;
	if (!_forwarding_args.empty()) {
		available_args -= _forwarding_args.size() + 1; // Exclude forwarded args with separator.
	}

	// Get all arguments left if specified value less then 0.
	const int arg_count = p_option->get_arg_count() < 0 ? available_args : MIN(available_args, p_option->get_arg_count() - p_skip_first);
	for (int i = 0; i < arg_count; ++i) {
		const String &arg = _args[p_current_idx + i];

		// Stop parsing on new option.
		if (_parse_prefix(arg).exists()) {
			break;
		}
		if (unlikely(!_validate_option_arg(p_option, p_display_name, arg))) {
			return -1;
		}
		++validated_arg_count;
	}

	// The option has a certain number of required arguments, but got less.
	if (unlikely(p_option->get_arg_count() >= 0 && p_option->get_arg_count() != validated_arg_count + p_skip_first)) {
		_error_text = vformat(RTR("Option '%s' expects %d arguments, but %d was provided."),
				p_display_name, p_option->get_arg_count(), validated_arg_count + p_skip_first);
		return -1;
	}
	// Option that takes all arguments left should always have at least one.
	if (unlikely(p_option->get_arg_count() < 0 && validated_arg_count == 0)) {
		_error_text = vformat(RTR("Option '%s' expects at least one argument."), p_display_name);
		return -1;
	}

	return validated_arg_count;
}

bool CommandLineParser::_validate_option_arg(const CommandLineOption *p_option, const String &p_display_name, const String &p_arg) {
	if (unlikely(!p_option->get_allowed_args().empty() && !has_arg(p_option->get_allowed_args(), p_arg))) {
		_error_text = vformat(RTR("Argument '%s' cannot be used for '%s', possible values: %s."), p_arg, p_display_name, join_args(p_option->get_allowed_args()));
		return false;
	}
	return true;
}

void CommandLineParser::_save_parsed_option(const CommandLineOption *p_option, const String &p_prefix, int p_idx, int p_arg_count, const String &p_additional_value) {
	_parsed_count[p_option] += 1;
	if (!p_prefix.empty()) {
		_parsed_prefixes[p_option].push_back(p_prefix);
	}
	PoolStringArray &values = _parsed_values[p_option];
	if (!p_additional_value.empty()) {
		values.push_back(p_additional_value);
	}
	for (int i = p_idx; i < p_idx + p_arg_count; ++i) {
		values.push_back(_args[i]);
	}
}

void CommandLineParser::_save_parsed_option(const CommandLineOption *p_option, const String &p_prefix, const String &p_value) {
	_save_parsed_option(p_option, p_prefix, 0, 0, p_value);
}

void CommandLineParser::_save_parsed_option(const CommandLineOption *p_option, int p_idx, int p_arg_count) {
	_save_parsed_option(p_option, String(), p_idx, p_arg_count);
}

String CommandLineParser::_get_usage(const Vector<Pair<const CommandLineOption *, String>> &p_printable_options, const String &p_title) const {
	String usage = vformat(RTR("Usage: %s"), p_title.empty() ? OS::get_singleton()->get_executable_path().get_file() : p_title);
	if (_contains_optional_options(p_printable_options)) {
		usage += ' ' + RTR("[options]");
	}

	for (int i = 0; i < p_printable_options.size(); ++i) {
		const CommandLineOption *option = p_printable_options[i].first;
		if (!option->is_required()) {
			continue;
		}
		const PoolStringArray names = option->get_names();
		usage += ' ';
		if (option->is_positional()) {
			usage += '[';
		}
		usage += _get_prefixed_longest_name(names);
		if (option->is_positional()) {
			usage += ']';
		}
		if (option->get_arg_count() != 0) {
			const String arg_text = option->get_arg_text();
			if (!arg_text.empty()) {
				usage += ' ' + arg_text;
				if (option->get_arg_count() < 0 || option->is_multitoken()) {
					usage += "...";
				}
			}
		}
	}
	return usage;
}

String CommandLineParser::_get_options_description(const OrderedHashMap<String, PoolStringArray> &p_categories_data) const {
	String description;
	for (OrderedHashMap<String, PoolStringArray>::ConstElement E = p_categories_data.front(); E; E = E.next()) {
		const String &category = E.key();
		const PoolStringArray &lines = E.value();

		description += '\n'; // Add a blank line for readability.
		if (!category.empty()) {
			description += '\n' + category + ":";
		}
		for (int j = 0; j < lines.size(); ++j) {
			description += '\n' + lines[j];
		}
	}
	return description;
}

String CommandLineParser::_to_string(const PoolStringArray &p_names) const {
	String string;
	for (int i = 0; i < p_names.size(); ++i) {
		if (i != 0) {
			string += ", ";
		}
		const PoolStringArray &prefixes = p_names[i].length() == 1 ? _short_prefixes : _long_prefixes;
		for (int j = 0; j < prefixes.size(); ++j) {
			if (j != 0) {
				string += ", ";
			}
			string += prefixes[j] + p_names[i];
		}
	}
	return string;
}

String CommandLineParser::_get_prefixed_longest_name(const PoolStringArray &p_names) const {
	int longest_idx = 0;
	for (int i = 0, longest_size = 0; i < p_names.size(); ++i) {
		const int current_size = p_names[i].size();
		if (current_size > longest_size) {
			longest_size = current_size;
			longest_idx = i;
		}
	}
	if (p_names[longest_idx].size() > 0) {
		return _long_prefixes[0] + p_names[longest_idx];
	}
	return _short_prefixes[0] + p_names[longest_idx];
}

CommandLineParser::ParsedPrefix CommandLineParser::_parse_prefix(const String &p_arg) const {
	// Check if argument is a negative number.
	if (p_arg.is_valid_float()) {
		return ParsedPrefix();
	}
	// Find longest prefix to match to properly distinguish between prefixes
	// such as `--` and `--no-`, so that shorter prefixes don't prematurely
	// match option names that have longer prefixes.
	int longest_idx = -1;
	int longest_size = 0;
	for (int i = 0; i < _long_prefixes.size(); ++i) {
		const String &prefix = _long_prefixes[i];
		if (p_arg.begins_with(prefix)) {
			if (prefix.length() > longest_size) {
				longest_idx = i;
				longest_size = prefix.length();
			}
		}
	}
	if (longest_idx != -1) {
		return ParsedPrefix{ _long_prefixes[longest_idx], false };
	}
	for (int i = 0; i < _short_prefixes.size(); ++i) {
		if (p_arg.begins_with(_short_prefixes[i])) {
			return ParsedPrefix{ _short_prefixes[i], true };
		}
	}
	return ParsedPrefix();
}

String CommandLineParser::_find_most_similar(const String &p_name) const {
	if (p_name.length() == 1) {
		// Do not search for short names.
		return String();
	}
	String most_similar;
	float max_similarity = _similarity_bias; // Start with this value to avoid returning unrelated names.

	for (int i = 0; i < _options.size(); ++i) {
		const PoolStringArray flags = _options[i]->get_names();
		for (int j = 0; j < flags.size(); ++j) {
			float similarity = flags[j].similarity(p_name);
			if (max_similarity < similarity) {
				most_similar = flags[j];
				max_similarity = similarity;
			}
		}
	}
	return most_similar;
}

bool CommandLineParser::_contains_optional_options(const Vector<Pair<const CommandLineOption *, String>> &p_printable_options) {
	for (int i = 0; i < p_printable_options.size(); ++i) {
		if (!p_printable_options[i].first->is_required()) {
			return true;
		}
	}
	return false;
}

void CommandLineParser::_bind_methods() {
	ClassDB::bind_method(D_METHOD("parse", "args"), &CommandLineParser::parse);

	ClassDB::bind_method(D_METHOD("add_option", "name", "description", "default_value", "allowed_values"), &CommandLineParser::add_option, DEFVAL(""), DEFVAL(""), DEFVAL(PoolStringArray()));
	ClassDB::bind_method(D_METHOD("add_help_option"), &CommandLineParser::add_help_option);
	ClassDB::bind_method(D_METHOD("add_version_option"), &CommandLineParser::add_version_option);

	ClassDB::bind_method(D_METHOD("append_option", "option"), &CommandLineParser::append_option);
	ClassDB::bind_method(D_METHOD("set_option", "index", "option"), &CommandLineParser::set_option);
	ClassDB::bind_method(D_METHOD("get_option", "index"), &CommandLineParser::get_option);
	ClassDB::bind_method(D_METHOD("get_option_count"), &CommandLineParser::get_option_count);
	ClassDB::bind_method(D_METHOD("remove_option", "index"), &CommandLineParser::remove_option);
	ClassDB::bind_method(D_METHOD("find_option", "name"), &CommandLineParser::find_option);

	ClassDB::bind_method(D_METHOD("is_set", "option"), &CommandLineParser::is_set);

	ClassDB::bind_method(D_METHOD("get_value", "option"), &CommandLineParser::get_value);
	ClassDB::bind_method(D_METHOD("get_value_list", "option"), &CommandLineParser::get_value_list);

	ClassDB::bind_method(D_METHOD("get_prefix", "option"), &CommandLineParser::get_prefix);
	ClassDB::bind_method(D_METHOD("get_prefix_list", "option"), &CommandLineParser::get_prefix_list);

	ClassDB::bind_method(D_METHOD("get_occurrence_count", "option"), &CommandLineParser::get_occurrence_count);

	ClassDB::bind_method(D_METHOD("get_forwarding_args"), &CommandLineParser::get_forwarding_args);
	ClassDB::bind_method(D_METHOD("get_args"), &CommandLineParser::get_args);

	ClassDB::bind_method(D_METHOD("get_help_text", "format"), &CommandLineParser::get_help_text, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("get_error_text"), &CommandLineParser::get_error_text);

	ClassDB::bind_method(D_METHOD("clear"), &CommandLineParser::clear);

	ClassDB::bind_method(D_METHOD("set_long_prefixes", "prefixes"), &CommandLineParser::set_long_prefixes);
	ClassDB::bind_method(D_METHOD("get_long_prefixes"), &CommandLineParser::get_long_prefixes);

	ClassDB::bind_method(D_METHOD("set_short_prefixes", "prefixes"), &CommandLineParser::set_short_prefixes);
	ClassDB::bind_method(D_METHOD("get_short_prefixes"), &CommandLineParser::get_short_prefixes);

	ClassDB::bind_method(D_METHOD("set_similarity_bias", "bias"), &CommandLineParser::set_similarity_bias);
	ClassDB::bind_method(D_METHOD("get_similarity_bias"), &CommandLineParser::get_similarity_bias);

	ClassDB::bind_method(D_METHOD("set_allow_forwarding_args", "allow"), &CommandLineParser::set_allow_forwarding_args);
	ClassDB::bind_method(D_METHOD("are_forwarding_args_allowed"), &CommandLineParser::are_forwarding_args_allowed);

	ClassDB::bind_method(D_METHOD("set_allow_adjacent", "allow"), &CommandLineParser::set_allow_adjacent);
	ClassDB::bind_method(D_METHOD("is_adjacent_allowed"), &CommandLineParser::is_adjacent_allowed);

	ClassDB::bind_method(D_METHOD("set_allow_sticky", "allow"), &CommandLineParser::set_allow_sticky);
	ClassDB::bind_method(D_METHOD("is_sticky_allowed"), &CommandLineParser::is_sticky_allowed);

	ClassDB::bind_method(D_METHOD("set_allow_compound", "allow"), &CommandLineParser::set_allow_compound);
	ClassDB::bind_method(D_METHOD("is_compound_allowed"), &CommandLineParser::is_compound_allowed);

	ADD_PROPERTY(PropertyInfo(Variant::POOL_STRING_ARRAY, "long_prefixes"), "set_long_prefixes", "get_long_prefixes");
	ADD_PROPERTY(PropertyInfo(Variant::POOL_STRING_ARRAY, "short_prefixes"), "set_short_prefixes", "get_short_prefixes");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "similarity_bias"), "set_similarity_bias", "get_similarity_bias");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_forwarding_args"), "set_allow_forwarding_args", "are_forwarding_args_allowed");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_adjacent"), "set_allow_adjacent", "is_adjacent_allowed");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_sticky"), "set_allow_sticky", "is_sticky_allowed");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_compound"), "set_allow_compound", "is_compound_allowed");
}

Error CommandLineParser::parse(const PoolStringArray &p_args) {
	_args = p_args;
	_forwarding_args.resize(0);
	_parsed_values.clear();
	_parsed_prefixes.clear();
	_parsed_count.clear();

	if (unlikely(!_are_options_valid())) {
		_error_text = RTR("Option parser was defined with incorrect options.");
		return ERR_PARSE_ERROR;
	}

	int arg_count = _args.size();
	if (_allow_forwarding_args) {
		int separator_pos = find_arg(_args, "--");
		if (separator_pos != -1) {
			// Separator available.
			arg_count = separator_pos;
			if (separator_pos + 1 != _args.size()) {
				// Arguments after available.
				_forwarding_args = _args.subarray(separator_pos + 1, _args.size() - 1);
			}
		}
	}

	for (int i = 0; i < arg_count;) {
		const int taken_arguments = _validate_arguments(i);
		if (unlikely(taken_arguments == -1)) {
			return ERR_PARSE_ERROR;
		}
		i += taken_arguments;
	}

	_read_default_args();

	bool meta_options_parsed = false;
	for (int i = 0; i < _options.size(); ++i) {
		const CommandLineOption *option = _options[i].ptr();
		if (_parsed_values.has(option) && option->is_meta()) {
			meta_options_parsed = true;
			break;
		}
	}
	if (!meta_options_parsed) {
		for (int i = 0; i < _options.size(); ++i) {
			const CommandLineOption *option = _options[i].ptr();
			if (unlikely(option->is_required() && !_parsed_values.has(option))) {
				_error_text = vformat(RTR("Option '%s' is required but missing."), _to_string(option->get_names()));
				return ERR_PARSE_ERROR;
			}
		}
	}
	for (int i = 0; i < _options.size(); ++i) {
		CommandLineOption *option = _options.get(i).ptr();
		const Map<const CommandLineOption *, PoolStringArray>::Element *E = _parsed_values.find(option);
		if (E) {
			option->emit_signal("parsed", E->value());
		}
	}
	return OK;
}

void CommandLineParser::append_option(const Ref<CommandLineOption> &p_option) {
	ERR_FAIL_COND(p_option.is_null());
	_options.push_back(p_option);
}

int CommandLineParser::get_option_count() const {
	return _options.size();
}

Ref<CommandLineOption> CommandLineParser::get_option(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, _options.size(), nullptr);
	return _options[p_idx];
}

void CommandLineParser::set_option(int p_idx, const Ref<CommandLineOption> &p_option) {
	ERR_FAIL_INDEX(p_idx, _options.size());
	_options.set(p_idx, p_option);
}

void CommandLineParser::remove_option(int p_idx) {
	ERR_FAIL_INDEX(p_idx, _options.size());
	_options.remove(p_idx);
}

Ref<CommandLineOption> CommandLineParser::find_option(const String &p_name) const {
	for (int i = 0; i < _options.size(); ++i) {
		if (has_arg(_options[i]->get_names(), p_name)) {
			return _options[i];
		}
	}
	return Ref<CommandLineOption>();
}

Ref<CommandLineOption> CommandLineParser::add_option(const String &p_name, const String &p_description, const String &p_default_value, const PoolStringArray &p_allowed_values) {
	ERR_FAIL_COND_V_MSG(p_name.empty(), Ref<CommandLineOption>(), "Option name cannot be empty.");

	Ref<CommandLineOption> option = memnew(CommandLineOption);
	option->add_name(p_name);
	option->set_description(p_description);

	if (!p_default_value.empty()) {
		option->add_default_arg(p_default_value);
	}
	if (!p_allowed_values.empty()) {
		option->set_allowed_args(p_allowed_values);
	}
	_options.push_back(option);

	return option;
}

Ref<CommandLineOption> CommandLineParser::add_help_option() {
	PoolVector<String> names;
	names.push_back("h");
	names.push_back("help");

	Ref<CommandLineOption> option = memnew(CommandLineOption(names, 0));
	option->set_category("General");
	option->set_description("Display this help message.");
	option->set_as_meta(true);
	append_option(option);

	return option;
}

Ref<CommandLineOption> CommandLineParser::add_version_option() {
	PoolVector<String> names;
	names.push_back("v");
	names.push_back("version");

	Ref<CommandLineOption> option = memnew(CommandLineOption(names, 0));
	option->set_category("General");
	option->set_description("Display version information.");
	option->set_as_meta(true);
	append_option(option);

	return option;
}

bool CommandLineParser::is_set(const Ref<CommandLineOption> &p_option) const {
	ERR_FAIL_COND_V(p_option.is_null(), false);
	return _parsed_values.has(p_option.ptr());
}

String CommandLineParser::get_value(const Ref<CommandLineOption> &p_option) const {
	ERR_FAIL_COND_V(p_option.is_null(), String());
	ERR_FAIL_COND_V_MSG(p_option->get_arg_count() == 0, String(), vformat("Option '%s' does not accept arguments.", _to_string(p_option->get_names())));

	const PoolStringArray args = get_value_list(p_option);
	if (args.empty()) {
		return String();
	}
	return args[0];
}

PoolStringArray CommandLineParser::get_value_list(const Ref<CommandLineOption> &p_option) const {
	ERR_FAIL_COND_V(p_option.is_null(), PoolStringArray());
	ERR_FAIL_COND_V_MSG(p_option->get_arg_count() == 0, PoolStringArray(), vformat("Option '%s' does not accept arguments.", _to_string(p_option->get_names())));
	const Map<const CommandLineOption *, PoolStringArray>::Element *E = _parsed_values.find(p_option.ptr());
	if (!E) {
		return PoolStringArray();
	}
	return E->value();
}

String CommandLineParser::get_prefix(const Ref<CommandLineOption> &p_option) const {
	ERR_FAIL_COND_V(p_option.is_null(), String());
	const PoolStringArray args = get_prefix_list(p_option);
	if (args.empty()) {
		return String();
	}
	return args[0];
}

PoolStringArray CommandLineParser::get_prefix_list(const Ref<CommandLineOption> &p_option) const {
	ERR_FAIL_COND_V(p_option.is_null(), PoolStringArray());
	const Map<const CommandLineOption *, PoolStringArray>::Element *E = _parsed_prefixes.find(p_option.ptr());
	if (!E) {
		return PoolStringArray();
	}
	return E->value();
}

int CommandLineParser::get_occurrence_count(const Ref<CommandLineOption> &p_option) const {
	ERR_FAIL_COND_V(p_option.is_null(), 0);
	const Map<const CommandLineOption *, int>::Element *E = _parsed_count.find(p_option.ptr());
	if (!E) {
		return 0;
	}
	return E->value();
}

String CommandLineParser::get_help_text(const Ref<CommandLineHelpFormat> &p_format) const {
	ERR_FAIL_COND_V_MSG(_short_prefixes.empty(), String(), "Short prefixes cannot be empty");
	ERR_FAIL_COND_V_MSG(_long_prefixes.empty(), String(), "Long prefixes cannot be empty");

	Ref<CommandLineHelpFormat> format = p_format;
	if (format.is_null()) {
		format.instance();
	}
	// Build the formated "-x, --xxxxx" and save the longest size to align the descriptions.
	int options_length = 0;
	Vector<Pair<const CommandLineOption *, String>> printable_options;
	for (int i = 0; i < _options.size(); ++i) {
		const CommandLineOption *option = _options[i].ptr();
		if (option->is_hidden()) {
			continue;
		}
		const PoolStringArray names = option->get_names();
		ERR_CONTINUE_MSG(names.empty(), vformat("Option at index %d does not have any name.", i));

		String line = _to_string(names);
		if (option->get_arg_count() != 0) {
			line += ' ' + option->get_arg_text();
		}
		options_length = MAX(options_length, line.length());
		printable_options.push_back(Pair<const CommandLineOption *, String>(option, line));
	}
	// Adjust max available line length from specified parameters.
	options_length = MIN(options_length + format->get_left_pad() + format->get_right_pad(), format->get_line_length() - format->get_min_description_length());
	const int descriptions_length = format->get_line_length() - options_length;

	// Fill categories and their data.
	OrderedHashMap<String, PoolStringArray> categories_data;
	for (int i = 0; i < printable_options.size(); ++i) {
		String line = printable_options[i].second.rpad(options_length - format->get_left_pad());
		line = line.lpad(line.length() + format->get_left_pad());
		if (line.length() > options_length) {
			// For long options, add a new padded line to display the description on a new line.
			line += '\n';
			line = line.rpad(line.length() + options_length);
		}

		const CommandLineOption *option = printable_options[i].first;
		int description_pos = line.length() + 1;
		line += option->get_description();
		if (!option->get_allowed_args().empty()) {
			line += vformat(RTR(" Allowed values: %s."), join_args(option->get_allowed_args()));
		}

		// Split long descriptions into multiply lines.
		while (line.length() - description_pos > descriptions_length) {
			int split_pos = line.rfind(" ", description_pos + descriptions_length); // Find last space to split by words.
			if (split_pos < description_pos) {
				// Word is too long, just split it at maximum size.
				split_pos = description_pos + descriptions_length - 1;
				line = line.insert(split_pos, "\n");
			} else {
				// Replace found space with line break.
				line.set(split_pos, '\n');
			}
			// Pad to the description column.
			line = line.insert(split_pos + 1, String().rpad(options_length));
			// Shift position to the next unprocessed line.
			description_pos = split_pos + 1 + options_length;
		}
		categories_data[option->get_category()].push_back(line);
	}
	// Start generating help.
	String help_text;
	if (!format->get_header().empty()) {
		help_text += format->get_header();
	}
	if (format->is_usage_autogenerated()) {
		help_text += '\n' + _get_usage(printable_options, format->get_usage_title());
	}
	help_text += _get_options_description(categories_data);
	if (!format->get_footer().empty()) {
		help_text += '\n' + format->get_footer();
	}
	help_text += '\n';
	return help_text;
}

void CommandLineParser::clear() {
	_options.clear();
	_args.resize(0);
	_forwarding_args.resize(0);
	_parsed_values.clear();
	_parsed_prefixes.clear();
	_error_text.clear();
}
