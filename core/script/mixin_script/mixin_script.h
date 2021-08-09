#pragma once

// Based on unreleased and subsequently removed MixinScript support
// in previous versions of Godot: https://github.com/godotengine/godot/pull/8718

#include "core/os/thread_safe.h"
#include "core/script_language.h"

class MixinScript;

class Mixin : public Object {
	GDCLASS(Mixin, Object)

	friend class MixinScript;

	Object *real_owner = nullptr;

public:
	static void _bind_methods();

	Object* get_owner() const { return real_owner; }

	virtual Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);
	virtual void call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount);
	virtual void call_multilevel_reversed(const StringName &p_method, const Variant **p_args, int p_argcount);
};

class MixinScriptInstance : public ScriptInstance {
	friend class MixinScript;

	Vector<ScriptInstance *> instances;
	Object *object = nullptr;
	MixinScript *owner = nullptr;

public:
	virtual bool set(const StringName &p_name, const Variant &p_value);
	virtual bool get(const StringName &p_name, Variant &r_ret) const;
	virtual void get_property_list(List<PropertyInfo> *p_properties) const;

	virtual void get_method_list(List<MethodInfo> *p_list) const;
	virtual bool has_method(const StringName &p_method) const;
	virtual Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);
	virtual void call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount);
	virtual void notification(int p_notification);

	virtual Ref<Script> get_script() const { return Ref<MixinScript>(owner); }

	virtual ScriptLanguage *get_language();
	virtual ~MixinScriptInstance();

public:
	Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid) const;
	virtual MultiplayerAPI::RPCMode get_rpc_mode(const StringName &p_method) const;
	virtual MultiplayerAPI::RPCMode get_rset_mode(const StringName &p_variable) const;
};

class MixinScript : public Script {
	GDCLASS(MixinScript, Script)
	RES_BASE_EXTENSION("ms");

	_THREAD_SAFE_CLASS_

	friend class MixinScriptInstance;
	friend class MixinScriptLanguage;

	StringName base_class_name;

	Vector<Ref<Script> > mixins;
	Vector<Mixin *> mixin_instances;
	Map<Object *, MixinScriptInstance *> instances;

protected:
	static void _bind_methods();

public:
	void remove_instance(Object *p_object);
	virtual bool can_instance() const { return true; }

	virtual StringName get_instance_base_type() const;
	virtual ScriptInstance *instance_create(Object *p_this);
	virtual bool instance_has(const Object *p_this) const;

	virtual bool has_source_code() const { return false; }
	virtual String get_source_code() const { return ""; }
	virtual void set_source_code(const String &p_code) {};
	virtual Error reload(bool p_keep_state = false);

	virtual bool is_tool() const;
	virtual bool is_valid() const;

	virtual bool inherits_script(const Ref<Script> &p_script) const;

	virtual String get_node_type() const { return ""; }

	void add_mixin(const Ref<Script> &p_script);
	void remove_mixin(int p_idx);
	void set_mixin(int p_idx, const Ref<Script> &p_script);
	void move_mixin(int p_pos, const Ref<Script> &p_script);
	void insert_mixin(int p_pos, const Ref<Script> &p_script);
	Ref<Script> get_mixin(int p_idx) const;
	int get_mixin_count() const { return mixins.size(); };
	void clear_mixins();

	void set_mixins(const Array &p_mixins);
	Array get_mixins() const;

	virtual ScriptLanguage *get_language() const;

	MixinScript();
	~MixinScript();

	virtual Ref<Script> get_base_script() const;
	virtual bool has_method(const StringName &p_method) const;
	virtual MethodInfo get_method_info(const StringName &p_method) const;
	virtual bool has_script_signal(const StringName &p_signal) const;
	virtual void get_script_signal_list(List<MethodInfo> *r_signals) const;
	virtual bool get_property_default_value(const StringName &p_property, Variant &r_value) const;
	virtual void get_script_method_list(List<MethodInfo> *p_list) const;
	virtual void get_script_property_list(List<PropertyInfo> *p_list) const;
	virtual void update_exports();
};

class MixinScriptLanguage : public ScriptLanguage {

	static MixinScriptLanguage *singleton;

public:
	static _FORCE_INLINE_ MixinScriptLanguage *get_singleton() { return singleton; }
	virtual String get_name() const { return "MixinScript"; }

	/* LANGUAGE FUNCTIONS */
	virtual void init() {}
	virtual String get_type() const { return "MixinScript"; }
	virtual String get_extension() const { return "ms"; }
	virtual Error execute_file(const String &p_path) { return OK; }
	virtual void finish() {}

	/* EDITOR FUNCTIONS */
	virtual void get_reserved_words(List<String> *p_words) const {}
	virtual bool is_control_flow_keyword(String p_keyword) const { return false; }
	virtual void get_comment_delimiters(List<String> *p_delimiters) const {}
	virtual void get_string_delimiters(List<String> *p_delimiters) const {}
	virtual Ref<Script> get_template(const String &p_class_name, const String &p_base_class_name) const;
	virtual bool validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path = "", List<String> *r_fn = nullptr, List<ScriptLanguage::Warning> *r_warnings = nullptr, Set<int> *r_safe_lines = nullptr) const { return true; }
	virtual Script *create_script() const;
	virtual bool has_named_classes() const { return false; }
	virtual bool supports_builtin_mode() const { return true; }
	virtual int find_function(const String &p_function, const String &p_code) const { return -1; }
	virtual String make_function(const String &p_class, const String &p_name, const PoolStringArray &p_args) const { return ""; }

	/* DEBUGGER FUNCTIONS */
	virtual String debug_get_error() const { return ""; }
	virtual int debug_get_stack_level_count() const { return 0; }
	virtual int debug_get_stack_level_line(int p_level) const { return 0; }
	virtual String debug_get_stack_level_function(int p_level) const { return ""; }
	virtual String debug_get_stack_level_source(int p_level) const { return ""; }
	virtual void debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) {}
	virtual void debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) {}
	virtual void debug_get_globals(List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) {}
	virtual String debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems = -1, int p_max_depth = -1) { return ""; }

	/* LOADER FUNCTIONS */
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual void get_public_functions(List<MethodInfo> *p_functions) const {};

	MixinScriptLanguage();
	virtual ~MixinScriptLanguage();

public:
	void auto_indent_code(String &p_code, int p_from_line, int p_to_line) const {}
	void add_global_constant(const StringName &p_variable, const Variant &p_value) {}
	void reload_all_scripts() {}
	void reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) {}
	void get_public_constants(List<Pair<String, Variant> > *p_constants) const {}
	void profiling_start() {}
	void profiling_stop() {}
	int profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max) { return 0; }
	int profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max) { return 0; }
};

