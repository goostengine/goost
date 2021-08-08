extends "res://addons/gut/test.gd"

var cmd: CommandLineParser
var opt: CommandLineOption

func before_each():
	cmd = CommandLineParser.new()


func test_parse():
    var args = ["--input", "path"]

    var input = CommandLineOption.new()
    input.names = ["input", "i"]

    cmd.add_option(input)
    var err = cmd.parse(args)
    if err:
        print(cmd.get_error_text())
    assert_eq(err, OK)

    var i = cmd.find_option("input")
    assert_eq(input, i)

    assert_eq(cmd.get_value(i), "path")


func add_test_option(arg_count):
    opt = CommandLineOption.new()
    opt.names = ["i", "input"]
    opt.arg_count = arg_count
    cmd.add_option(opt)


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

    cmd.add_option(opt)
    assert_eq(cmd.get_option_count(), count)

    assert_eq(cmd.find_option(opt.names[0]), opt, "Should find option by existing name/alias.")
    assert_eq(cmd.find_option(opt.names[1]), opt, "Should find option by existing name/alias.")

    
func test_validation():
    add_test_option(1)

    opt.default_args = ["a"]

    assert_eq(cmd.parse([]), OK, "Parsing with a valid default argument should succeed.")

    Engine.print_error_messages = false
    
    var arg_count = opt.arg_count
    opt.arg_count = arg_count + 1
    assert_eq(cmd.parse([]), ERR_INVALID_DECLARATION,
            "Expected error: Option requires more arguments, but only one default value was given.")
    opt.arg_count = arg_count

    var names = opt.names
    opt.names = []
    assert_eq(cmd.parse([]), ERR_INVALID_DECLARATION,
            "Expected error: Parsing with an option which does not have any names should fail.")
    opt.names = names
    
    var default_args = opt.default_args
    opt.default_args = ["a", "b"]
    assert_eq(cmd.parse([]), ERR_INVALID_DECLARATION,
            "Expected error: Parsing with an option which has different number of arguments and default arguments should fail.")
    opt.default_args = default_args

    var required = opt.required
    opt.required = not required
    assert_eq(cmd.parse([]), ERR_INVALID_DECLARATION,
            "Parsing with an option which is required and have default arguments should fail.")
    opt.required = required

    # TODO: checkers here

    cmd.add_option(opt)
    assert_eq(cmd.parse([]), ERR_INVALID_DECLARATION,
            "Parsing with multiple options that have the same name should fail.")
    cmd.remove_option(1)

    Engine.print_error_messages = true

    assert_eq(cmd.parse([]), OK,
            "Parsing with a valid option (after reverting all changes) should be successful.")

        
func test_forwarding_args():
    assert_eq(cmd.parse(["--", "arg1", "arg2"]), ERR_INVALID_DATA,
            "Parsing forwarded arguments should fail if disabled.")

    cmd.allow_forwarding_args = true
    assert_eq(cmd.parse(["--"]), OK, "Forwarding zero arguments should succeed.")
    assert_true(cmd.get_forwarded_args().empty())

    assert_eq(cmd.parse(["--", "arg1", "arg2"]), OK,
            "Forwarding two arguments should succeed with `allow_forwarding_args = true`")
    assert_eq(cmd.get_forwarded_args(), PoolStringArray(["arg1", "arg2"]))


func test_short_options():
    add_test_option(0)

    assert_eq(cmd.parse(["-u"]), ERR_INVALID_DATA, "Unknown option, should fail.")
    assert_eq(cmd.parse(["-i", "-i"]), ERR_INVALID_DATA, "Same options, should fail.")

    var new_opt = CommandLineOption.new()
    new_opt.names = ["a"]
    new_opt.arg_count = 0
    cmd.add_option(new_opt)

    assert_eq(cmd.parse(["-a", "-i"]), OK, "Two compound options, should succeed.")
    assert_eq(cmd.parse(["-i", "-a"]), OK, "Two compound options, should succeed")
    assert_true(cmd.is_set(opt), "Compound, should succeed")
    assert_true(cmd.is_set(new_opt), "Compound, should succeed")

    cmd.allow_compound = false

    assert_eq(cmd.parse(["-a", "-i"]), OK, "Should fail, not allowed if `allow_compound = false`.")


func test_long_options():
    add_test_option(0)

    assert_eq(cmd.parse(["--test"]), ERR_INVALID_DATA, "Unknown option, should fail.")
    assert_eq(cmd.parse(["--input", "--input"]), ERR_INVALID_DATA, "Same options, should fail.")
    assert_eq(cmd.parse(["--input", "value"]), ERR_INVALID_DATA, "Option should not accept arguments.")

    opt.multitoken = true
    assert_eq(cmd.parse(["--input", "--input"]), OK, "Same multiple options should be allowed with `multitoken = true`.")
    assert_eq(cmd.get_occurrence_count(opt), 2)

    assert_eq(cmd.parse(["--input=path"]), ERR_INVALID_DATA, "Option should not accept arguments")


func test_one_arg():
    add_test_option(1)

    var new_opt = CommandLineOption.new()
    new_opt.names = ["a"]
    new_opt.arg_count = 0
    cmd.add_option(new_opt)

    assert_eq(cmd.parse(["-ai", "arg"]), OK, "Compound, should succeed.")
    print(cmd.get_error_text())
    assert_eq(cmd.get_value(opt), "arg")

    assert_true(cmd.is_set(new_opt))

    Engine.print_error_messages = false
    assert_eq(cmd.get_value(new_opt), "", "Option should not accept arguments.")
    Engine.print_error_messages = true

    cmd.allow_compound = false
    assert_eq(cmd.parse(["-ai", "arg"]), ERR_INVALID_DATA, "Compound options should fail if `allow_compound = false`.")

    assert_eq(cmd.parse(["-iarg"]), OK, "Sticky, should succeed.")

    cmd.allow_sticky = false
    assert_eq(cmd.parse(["-iarg"]), ERR_INVALID_DATA, "Compound options should fail if `allow_sticky = false`.")
