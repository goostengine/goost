
def can_build(env, platform):
    return True


def configure(env):
    from SCons.Script import Variables, BoolVariable, Help
    from builtin import components

    opts = Variables()
    for name in components:
        opts.Add(BoolVariable("goost_%s_enabled" % (name), "Build %s core component." % (name), True))

    opts.Add("goost_scale_factor", "The precision used for converting between integer and float coordinates.", "1e5")

    opts.Update(env)
    
    def help_format(env, opt, help, default, actual, aliases):
        if opt == "goost_scale_factor":
            fmt = "\n%s: %s.\n    default: %s (based on CMP_EPSILON)\n    actual: %s\n"
        else:
            fmt = "\n%s: %s.\n    default: %s\n    actual: %s\n"
        return fmt % (opt, help, default, actual)
    
    opts.FormatVariableHelpText = help_format
    Help(opts.GenerateHelpText(env))


def get_doc_classes():
    return [
        "GeometryExtension2D"
        "PolyBooleanParameters2D"
        "PolyDecompParameters2D"
        "PolyOffsetParameters2D"
        "PolyNode2D"
        "ImageExtension"
        "ImageBlender"
        "ImageIndexed"
        "ImageIndexed"
        "ShapeCast2D"
    ]


def get_doc_path():
    return "doc"


def get_icons_path():
    return "editor/icons"    
