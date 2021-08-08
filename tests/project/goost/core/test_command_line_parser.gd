extends "res://addons/gut/test.gd"

var parser: CommandLineParser

func before_each():
	parser = CommandLineParser.new()


func add_test_option(arg_count):
    var opt = CommandLineOption.new()
    opt.names = ["i", "input"]
    opt.arg_count = arg_count
    parser.add_option(opt)


func test_builtin_options():
    var opt: CommandLineOption

    opt = parser.add_help_option()
    assert_eq(parser.get_option_count(), 1)
    assert_eq(parser.get_option(0), opt)

    opt = parser.add_version_option()
    assert_eq(parser.get_option_count(), 2)
    assert_eq(parser.get_option(1), opt)

    var err = parser.parse([])
    assert_eq(err, OK)
