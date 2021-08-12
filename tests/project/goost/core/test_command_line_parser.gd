extends "res://addons/gut/test.gd"

var cmd: CommandLineParser
var opt: CommandLineOption

func before_each():
	cmd = CommandLineParser.new()


func test_parse():
    var input: CommandLineOption = cmd.add_option("input", "Path to a file.", "file.txt")
    input.add_name("i") # Add short name, if needed.
    print(cmd.get_help_text())

    watch_signals(input)

    # Parse with long prefix.
    var err = cmd.parse(["--input", "my_file.txt"])
    if err:
        print(cmd.get_error_text())
    assert_eq(err, OK)

    assert_signal_emitted(input, "parsed")

    var i = cmd.find_option("input")
    assert_eq(input, i)

    assert_eq(cmd.get_value(i), "my_file.txt")

    # Parse with short prefix.
    assert_eq(cmd.parse(["-i", "my_file.txt"]), OK)


func test_parse_multiple_options():
    var input = cmd.add_option("input", "Path to a file.")
    var verbose = cmd.add_option("verbose", "Run in verbose mode.", "no", ["yes", "no"])
    var debug = cmd.add_option("debug", "Run in debug mode.", "yes", ["yes", "no"])

    assert_true(input.default_args.empty())
    assert_true("no" in verbose.default_args)
    assert_true("yes" in debug.default_args)

    var err = cmd.parse(["--input", "path", "--debug", "no", "--verbose", "yes"])
    if err:
        print(cmd.get_error_text())
    assert_eq(err, OK)

    assert_eq(cmd.get_value(input), "path")
    assert_eq(cmd.get_value(verbose), "yes")
    assert_eq(cmd.get_value(debug), "no")

    Engine.print_error_messages = false

    err = cmd.parse(["--input", "path", "--debug", "nono", "--verbose", "yeess"])
    assert_eq(err, ERR_PARSE_ERROR, "Should detect invalid arguments.")

    Engine.print_error_messages = true


func test_same_options():
    var _opt
    _opt = cmd.add_option("verbose")
    _opt = cmd.add_option("verbose")

    Engine.print_error_messages = false

    var err = cmd.parse(["--verbose=yes"])
    assert_eq(err, ERR_PARSE_ERROR)

    Engine.print_error_messages = true


func test_same_options_multitoken():
    var k = cmd.add_option("k")
    k.multitoken = true

    var err = cmd.parse(["-k -k -k -k"])
    assert_eq(err, OK)


# func test_get_prefix():
#     cmd.long_prefixes = ["--", "--no-"]

#     var debug = cmd.add_option("debug")
#     debug.arg_count = 0
#     var no_debug = cmd.add_option("no-debug")
#     no_debug.arg_count = 0

#     assert_eq(cmd.parse(["--debug"]), OK)
#     assert_true(cmd.is_set(debug))
#     assert_false(cmd.is_set(no_debug))
    
#     assert_eq(cmd.parse(["--no-debug"]), OK)
#     assert_true(cmd.is_set(no_debug))
#     assert_false(cmd.is_set(debug))
    

func test_long_prefixes_precedence():
    var debug = cmd.add_option("debug")
    debug.arg_count = 0

    cmd.long_prefixes = ["--no-", "--"]

    assert_eq(cmd.parse(["--debug"]), OK)
    assert_true(cmd.is_set(debug))
    assert_eq(cmd.get_prefix(debug), "--")

    assert_eq(cmd.parse(["--no-debug"]), OK)
    assert_true(cmd.is_set(debug))
    assert_eq(cmd.get_prefix(debug), "--no-")
    
    cmd.long_prefixes = ["--", "--no-"] # Swap precedence.

    assert_eq(cmd.parse(["--debug"]), OK)
    assert_true(cmd.is_set(debug))
    assert_eq(cmd.get_prefix(debug), "--")

    assert_eq(cmd.parse(["--no-debug"]), ERR_PARSE_ERROR,
            "Precedence of `--` prefix is higher than `--no-`, should fail.")
    assert_false(cmd.is_set(debug))


func test_prefix_and_names_collision():
    var debug = cmd.add_option("debug")
    debug.arg_count = 0

    cmd.long_prefixes = ["--debug"]
    assert_eq(cmd.parse(["--debug"]), ERR_PARSE_ERROR,
            "Option should not be recognized: prefix and name are the same.")

    cmd.long_prefixes = ["++"]
    assert_eq(cmd.parse(["++"]), ERR_PARSE_ERROR,
            "Option should not be recognized: prefix and name are the same.")


func test_default_not_allowed_arg():
    var filetype = cmd.add_option("filetype", "Input to file")
    filetype.add_default_arg("png")
    filetype.add_allowed_arg("jpg")

    Engine.print_error_messages = false

    assert_eq(cmd.parse([]), ERR_PARSE_ERROR, "Default argument `png` is not allowed one, should fail.")
    assert_eq(cmd.get_value(filetype), "")

    Engine.print_error_messages = true


func test_unrecognized_options():
    var _input = cmd.add_option("input", "Input to file", "file.txt")

    Engine.print_error_messages = false

    assert_eq(cmd.parse(["--debug"]), ERR_PARSE_ERROR, "Parsing unknown options is not supported.")

    Engine.print_error_messages = true


func test_get_help_with_required_option():
    var help = cmd.add_help_option()
    assert_true(help.meta)
    var version = cmd.add_version_option()
    assert_true(version.meta)

    var input = cmd.add_option("input", "Input to file")
    input.required = true

    assert_eq(cmd.parse(["--help"]), OK,
            "Parsing with required option should not fail if `help` option is found.")

    assert_eq(cmd.parse(["--version"]), OK,
            "Parsing with required option should not fail if `version` option is found.")


func add_test_option(arg_count):
    opt = CommandLineOption.new()
    opt.names = ["i", "input"]
    opt.arg_count = arg_count
    cmd.append_option(opt)


func test_builtin_options():
    var o: CommandLineOption

    o = cmd.add_help_option()
    assert_eq(cmd.get_option_count(), 1)
    assert_eq(cmd.get_option(0), o)

    o = cmd.add_version_option()
    assert_eq(cmd.get_option_count(), 2)
    assert_eq(cmd.get_option(1), o)

    assert_eq(cmd.parse([]), OK)


func test_options():
    add_test_option(1)

    var count = cmd.get_option_count()
    assert_eq(count, 1)

    cmd.remove_option(0)
    assert_eq(cmd.get_option_count(), count - 1)

    cmd.append_option(opt)
    assert_eq(cmd.get_option_count(), count)

    assert_eq(cmd.find_option(opt.names[0]), opt, "Should find option by existing name/alias.")
    assert_eq(cmd.find_option(opt.names[1]), opt, "Should find option by existing name/alias.")


func test_validation():
    add_test_option(1)

    opt.default_args = ["a"]

    assert_eq(cmd.parse([]), OK)

    Engine.print_error_messages = false

    var arg_count = opt.arg_count
    opt.arg_count = arg_count + 1
    assert_eq(cmd.parse([]), ERR_PARSE_ERROR, "Requires more args, but only one default value was given.")
    opt.arg_count = arg_count

    var names = opt.names
    opt.names = []
    assert_eq(cmd.parse([]), ERR_PARSE_ERROR, "No names, should fail.")
    opt.names = names

    var default_args = opt.default_args
    opt.default_args = ["a", "b"]
    assert_eq(cmd.parse([]), ERR_PARSE_ERROR, "Different number of arguments and default arguments, should fail.")
    opt.default_args = default_args

    var required = opt.required
    opt.required = not required
    assert_eq(cmd.parse([]), ERR_PARSE_ERROR, "Required and have default arguments, should fail.")
    opt.required = required

    cmd.append_option(opt)
    assert_eq(cmd.parse([]), ERR_PARSE_ERROR, "Same name, should fail.")
    cmd.remove_option(1)

    Engine.print_error_messages = true

    assert_eq(cmd.parse([]), OK,
            "Parsing with a valid option (after reverting all changes) should be successful.")


func test_forwarding_args():
    assert_eq(cmd.parse(["--", "arg1", "arg2"]), ERR_PARSE_ERROR,
            "Parsing forwarded arguments should fail if disabled.")

    cmd.allow_forwarding_args = true
    assert_eq(cmd.parse(["--"]), OK, "Forwarding zero arguments should succeed.")
    assert_true(cmd.get_forwarding_args().empty())

    assert_eq(cmd.parse(["--", "arg1", "arg2"]), OK,
            "Forwarding two arguments should succeed with `allow_forwarding_args = true`")
    assert_eq(cmd.get_forwarding_args(), PoolStringArray(["arg1", "arg2"]))


func test_short_options():
    add_test_option(0)

    assert_eq(cmd.parse(["-u"]), ERR_PARSE_ERROR, "Unknown option, should fail.")
    assert_eq(cmd.parse(["-i", "-i"]), ERR_PARSE_ERROR, "Same options, should fail.")

    var new_opt = CommandLineOption.new()
    new_opt.names = ["a"]
    new_opt.arg_count = 0
    cmd.append_option(new_opt)

    assert_eq(cmd.parse(["-ai"]), OK, "Two compound options, should succeed.")
    assert_eq(cmd.parse(["-ia"]), OK, "Two compound options, should succeed")
    assert_true(cmd.is_set(opt), "Compound, should succeed")
    assert_true(cmd.is_set(new_opt), "Compound, should succeed")

    cmd.allow_compound = false

    assert_eq(cmd.parse(["-ai"]), ERR_PARSE_ERROR, "Should fail, not allowed if `allow_compound = false`.")


func test_short_option_not_short():
    var new_opt = CommandLineOption.new()
    new_opt.names = ["aaaaa", "a", "aaaa"]
    new_opt.arg_count = 0
    cmd.append_option(new_opt)

    assert_eq(cmd.parse(["-aaaa"]), ERR_PARSE_ERROR)


func test_long_options():
    var file = cmd.add_option("file")
    file.arg_count = 0

    assert_eq(cmd.parse(["--test"]), ERR_PARSE_ERROR, "Unknown option, should fail.")
    assert_eq(cmd.parse(["--file", "--file"]), ERR_PARSE_ERROR, "Same options, should fail.")
    assert_eq(cmd.parse(["--file", "value"]), ERR_PARSE_ERROR, "Option should not accept arguments.")


func test_one_arg():
    add_test_option(1)

    var new_opt = CommandLineOption.new()
    new_opt.names = ["a"]
    new_opt.arg_count = 0
    cmd.append_option(new_opt)

    assert_eq(cmd.parse(["-ai", "arg"]), OK, "Compound, should succeed.")
    assert_eq(cmd.get_value(opt), "arg")

    assert_true(cmd.is_set(new_opt))

    Engine.print_error_messages = false
    assert_eq(cmd.get_value(new_opt), "", "Option should not accept arguments.")
    Engine.print_error_messages = true

    cmd.allow_compound = false
    assert_eq(cmd.parse(["-ai", "arg"]), ERR_PARSE_ERROR, "Compound options should fail if `allow_compound = false`.")

    assert_eq(cmd.parse(["-iarg"]), OK, "Sticky, should succeed.")

    cmd.allow_sticky = false
    assert_eq(cmd.parse(["-iarg"]), ERR_PARSE_ERROR, "Compound options should fail if `allow_sticky = false`.")


func test_positional():
    add_test_option(-1) # Any number of values.
    opt.positional = true

    var sum = cmd.add_option("sum")
    sum.arg_count = 0

    assert_eq(cmd.parse(["1", "2", "3"]), OK)
    var values = cmd.get_value_list(opt)
    assert_eq(values.size(), 3)
    assert_eq(values[0], "1")
    assert_eq(values[1], "2")
    assert_eq(values[2], "3")
    assert_false(cmd.is_set(sum))

    assert_eq(cmd.parse(["--input", "1", "2", "3", "--sum"]), OK)
    values = cmd.get_value_list(opt)
    assert_eq(values.size(), 3)
    assert_eq(values[0], "1")
    assert_eq(values[1], "2")
    assert_eq(values[2], "3")
    assert_true(cmd.is_set(sum))

    if cmd.is_set(sum):
        var total = 0
        for v in values:
            total += str2var(v)
        assert_eq(total, 6)
