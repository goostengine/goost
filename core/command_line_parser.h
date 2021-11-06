#pragma once

#include "core/ordered_hash_map.h"
#include "core/reference.h"

class CommandLineOption : public Reference {
	GDCLASS(CommandLineOption, Reference);

	// Names for the options. e.g --help or -h.
	PoolStringArray _names;
	// List of default values for each argument, empty if any value is allowed.
	PoolStringArray _default_args;
	// List of allowed values for each argument, empty if any value is allowed.
	PoolStringArray _allowed_args;
	// Option's description that will be displayed in help.
	String _description;
	// Option's category, options sharing the same category are grouped together in help.
	String _category;
	// Name for the option's arguments that will be displayed in help.
	String _arg_text = RTR("<arg>");
	// Make the option visible in help.
	bool _hidden = false;
	// If true, arguments can be specified without an option name.
	bool _positional = false;
	// If true, argument must always be provided.
	bool _required = false;
	// If true, the option can be specified several times.
	bool _multitoken = false;
	// Arguments count required for the option, -1 for all arguments left.
	int _arg_count = 1;
	// Options marked as meta, such as --help, --version etc.
	// Allows to skip some validation checks such as required options.
	bool _meta = false; 

protected:
	static void _bind_methods();

public:
	void set_names(const PoolStringArray &p_names);
	PoolStringArray get_names() const { return _names; }

	void set_default_args(const PoolStringArray &p_args) { _default_args = p_args; }
	PoolStringArray get_default_args() const { return _default_args; }

	void set_allowed_args(const PoolStringArray &p_args) { _allowed_args = p_args; }
	PoolStringArray get_allowed_args() const { return _allowed_args; }

	void set_description(const String &p_description) { _description = p_description; }
	String get_description() const { return _description; }

	void set_category(const String &p_category) { _category = p_category; }
	String get_category() const { return _category; }

	void set_arg_text(const String &p_arg_text) { _arg_text = p_arg_text; }
	String get_arg_text() const { return _arg_text; }

	void set_arg_count(int p_count) { _arg_count = p_count; }
	int get_arg_count() const { return _arg_count; }

	void set_hidden(bool p_hidden) { _hidden = p_hidden; }
	bool is_hidden() const { return _hidden; }

	void set_positional(bool p_positional) { _positional = p_positional; }
	bool is_positional() const { return _positional; }

	void set_required(bool p_required) { _required = p_required; }
	bool is_required() const { return _required; }

	void set_multitoken(bool p_multitoken) { _multitoken = p_multitoken; }
	bool is_multitoken() const { return _multitoken; }

	void set_as_meta(bool p_meta) { _meta = p_meta; }
	bool is_meta() const { return _meta; }

	// Utility methods.
	void add_name(const String &p_name);
	void add_default_arg(const String &p_arg);
	void add_allowed_arg(const String &p_arg);

	CommandLineOption() = default;
	explicit CommandLineOption(const PoolStringArray &p_names, int p_arg_count = 1);
};

class CommandLineHelpFormat : public Reference {
	GDCLASS(CommandLineHelpFormat, Reference);

	String _help_header;
	String _help_footer;
	String _usage_title;

	int _left_help_pad = 2;
	int _right_help_pad = 4;
	int _help_line_length = 80;
	int _min_description_length = _help_line_length / 2;

	bool _autogenerate_usage = true;

protected:
	static void _bind_methods();

public:
	void set_header(const String &p_header) { _help_header = p_header; }
	String get_header() const { return _help_header; }

	void set_footer(const String &p_footer) { _help_footer = p_footer; }
	String get_footer() const { return _help_footer; }

	void set_usage_title(const String &p_title) { _usage_title = p_title; }
	String get_usage_title() const { return _usage_title; }

	void set_left_pad(int p_size) { _left_help_pad = p_size; }
	int get_left_pad() const { return _left_help_pad; }

	void set_right_pad(int p_size) { _right_help_pad = p_size; }
	int get_right_pad() const { return _right_help_pad; }

	void set_line_length(int p_length) { _help_line_length = p_length; }
	int get_line_length() const { return _help_line_length; }

	void set_min_description_length(int p_length) { _min_description_length = p_length; }
	int get_min_description_length() const { return _min_description_length; }

	void set_autogenerate_usage(bool p_generate) { _autogenerate_usage = p_generate; }
	bool is_usage_autogenerated() const { return _autogenerate_usage; }
};

class CommandLineParser : public Reference {
	GDCLASS(CommandLineParser, Reference);

	struct ParsedPrefix;

	Vector<Ref<CommandLineOption>> _options;

	PoolStringArray _args;
	PoolStringArray _forwarding_args;

	PoolStringArray _long_prefixes;
	PoolStringArray _short_prefixes;

	Map<const CommandLineOption *, PoolStringArray> _parsed_values;
	Map<const CommandLineOption *, PoolStringArray> _parsed_prefixes;
	Map<const CommandLineOption *, int> _parsed_count;

	String _error_text;

	float _similarity_bias = 0.3;

	bool _allow_forwarding_args = false;
	bool _allow_adjacent = true;
	bool _allow_sticky = true;
	bool _allow_compound = true;

	// Parser main helpers.
	bool _are_options_valid() const;
	void _read_default_args();
	int _validate_arguments(int p_current_idx); // Returns number of arguments taken, -1 on validation error.

	// Helpers for the function above that parse a specific case.
	int _validate_positional(const String &p_arg, int p_current_idx);
	int _validate_adjacent(const String &p_arg, const String &p_prefix, int p_separator);
	int _validate_short(const String &p_arg, const String &p_prefix, int p_current_idx);
	int _validate_long(const String &p_arg, const String &p_prefix, int p_current_idx);

	// Validation helpers.
	const CommandLineOption *_validate_option(const String &p_name, const String &p_prefix);
	int _validate_option_args(const CommandLineOption *p_option, const String &p_display_name, int p_current_idx, bool p_skip_first = false);
	bool _validate_option_arg(const CommandLineOption *p_option, const String &p_display_name, const String &p_arg);

	// Save information about parsed option.
	void _save_parsed_option(const CommandLineOption *p_option, const String &p_prefix, int p_idx, int p_arg_count, const String &p_additional_value = String());
	void _save_parsed_option(const CommandLineOption *p_option, const String &p_prefix, const String &p_value = String());
	void _save_parsed_option(const CommandLineOption *p_option, int p_idx, int p_arg_count);

	// Help text printers.
	String _get_usage(const Vector<Pair<const CommandLineOption *, String>> &p_printable_options, const String &p_title) const;
	String _get_options_description(const OrderedHashMap<String, PoolStringArray> &p_categories_data) const;

	// Other utilies.
	String _to_string(const PoolStringArray &p_names) const; // Returns all option names separated by commas with all prefix variants.
	String _get_prefixed_longest_name(const PoolStringArray &p_names) const; // Returns longest name with first available prefix (long or short).
	ParsedPrefix _parse_prefix(const String &p_arg) const;
	String _find_most_similar(const String &p_name) const;
	static bool _contains_optional_options(const Vector<Pair<const CommandLineOption *, String>> &p_printable_options);

protected:
	static void _bind_methods();

public:
	Error parse(const PoolStringArray &p_args);

	Ref<CommandLineOption> add_option(const String &p_name, const String &p_description = "", const String &p_default_value = "", const PoolStringArray &p_allowed_values = PoolStringArray());
	Ref<CommandLineOption> add_help_option();
	Ref<CommandLineOption> add_version_option();

	void append_option(const Ref<CommandLineOption> &p_option);
	void set_option(int p_idx, const Ref<CommandLineOption> &p_option);
	Ref<CommandLineOption> get_option(int p_idx) const;
	int get_option_count() const;
	void remove_option(int p_idx);
	Ref<CommandLineOption> find_option(const String &p_name) const;

	bool is_set(const Ref<CommandLineOption> &p_option) const;

	String get_value(const Ref<CommandLineOption> &p_option) const;
	PoolStringArray get_value_list(const Ref<CommandLineOption> &p_option) const;

	String get_prefix(const Ref<CommandLineOption> &p_option) const;
	PoolStringArray get_prefix_list(const Ref<CommandLineOption> &p_option) const;

	int get_occurrence_count(const Ref<CommandLineOption> &p_option) const;

	PoolStringArray get_forwarding_args() const { return _forwarding_args; }
	PoolStringArray get_args() const { return _args; }

	String get_help_text(const Ref<CommandLineHelpFormat> &p_format = Ref<CommandLineHelpFormat>()) const;
	String get_error_text() const { return _error_text; }

	void set_long_prefixes(const PoolStringArray &p_prefixes) { _long_prefixes = p_prefixes; }
	PoolStringArray get_long_prefixes() const { return _long_prefixes; }

	void set_short_prefixes(const PoolStringArray &p_prefixes) { _short_prefixes = p_prefixes; }
	PoolStringArray get_short_prefixes() const { return _short_prefixes; }

	void set_similarity_bias(float p_bias) { _similarity_bias = CLAMP(p_bias, 0.0f, 1.0f); }
	float get_similarity_bias() const { return _similarity_bias; }

	void set_allow_forwarding_args(bool p_allow) { _allow_forwarding_args = p_allow; }
	bool are_forwarding_args_allowed() const { return _allow_forwarding_args; }

	void set_allow_adjacent(bool p_allow) { _allow_adjacent = p_allow; }
	bool is_adjacent_allowed() const { return _allow_adjacent; }

	void set_allow_sticky(bool p_allow) { _allow_sticky = p_allow; }
	bool is_sticky_allowed() const { return _allow_sticky; }

	void set_allow_compound(bool p_allow) { _allow_compound = p_allow; }
	bool is_compound_allowed() const { return _allow_compound; }

	void clear();

	CommandLineParser() {
		_long_prefixes.push_back("--");
		_short_prefixes.push_back("-");
	};
};