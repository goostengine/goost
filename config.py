import goost


def get_name():
    return "goost"


def can_build(env, platform):
    return True


def configure(env):
    from SCons.Script import Variables, BoolVariable, Help

    opts = Variables()

    # Config.
    components_config = {}
    components_enabled_by_default = True
    classes_config = {}
    classes_enabled_by_default = True

    # From `custom.py` file.
    try:
        import custom
        if hasattr(custom, "components"):
            components_config = custom.components
        if hasattr(custom, "components_enabled_by_default"):
            components_enabled_by_default = custom.components_enabled_by_default
        if hasattr(custom, "classes"):
            classes_config = custom.classes
        if hasattr(custom, "classes_enabled_by_default"):
            classes_enabled_by_default = custom.classes_enabled_by_default
    except ImportError:
        pass

    # From command-line (CLI arguments override arguments specified via file).
    opts.Add(BoolVariable("goost_components_enabled",
            "Set to `no` to disable all components by default, and enable each component of interest manually", True))

    # Get a list of all components and add them, regardless of configuration.
    for name in goost.get_components()["enabled"]: # All enabled by default.
        opts.Add(BoolVariable("goost_%s_enabled" % (name), "Build %s component." % (name), True))

    # Math/Geometry.
    opts.Add("goost_scale_factor", "The precision used for converting between integer and float coordinates.", "1e5")

    def help_format(env, opt, help, default, actual, aliases):
        if opt == "goost_scale_factor":
            fmt = "\n%s: %s.\n    default: %s (based on CMP_EPSILON)\n    actual: %s\n"
        else:
            fmt = "\n%s: %s.\n    default: %s\n    actual: %s\n"
        return fmt % (opt, help, default, actual)

    opts.FormatVariableHelpText = help_format

    # Must update environment to override `components_config` from CLI/file.
    # Do not call this method afterwards as the environment is going to be
    # updated manually. If you need to add more options not related to
    # components/classes, add them above.
    opts.Update(env)

    components = configure_components(env, components_config, components_enabled_by_default)
    classes = configure_classes(env, classes_config, classes_enabled_by_default)

    if env["verbose"]:
        for class_name in classes["enabled"]:
            # Report rightmost child components only.
            for component_name in reversed(goost.get_class_components(class_name)):
                skip = False
                if component_name in components["disabled"]:
                    print("Goost: Skipping class `%s`, because component `%s` is disabled."
                            % (class_name, component_name))
                    skip = True
                if skip:
                    break

    # Generate help text.
    Help(opts.GenerateHelpText(env))


def configure_components(env, config, enabled_by_default):
    from SCons.Script import ARGUMENTS

    if "goost_components_enabled" in ARGUMENTS:
        # Override from command-line.
        enabled_by_default = env["goost_components_enabled"]

    for name in goost.get_components()["enabled"]: # All enabled by default.
        c = "goost_%s_enabled" % name
        if c in ARGUMENTS:
            # Override from command-line.
            config[name] = env[c]

    # Get both enabled and disabled components based on configuration now,
    # which were collected from either file or command-line interface.
    components = goost.get_components(config, enabled_by_default)
    for name in components["enabled"]:
        env["goost_%s_enabled" % name] = True
    for name in components["disabled"]:
        env["goost_%s_enabled" % name] = False

    def disable_child_components(name):
        for child_name in goost.get_child_components(name):
            if child_name in to_disable:
                continue
            print("Goost: Disabling `%s` component (part of `%s`)." % (child_name, name))
            env["goost_%s_enabled" % child_name] = False
            to_disable.append(child_name)
            components["enabled"].remove(child_name)
 
    def enable_parent_components(name):
        for parent_name in goost.get_parent_components(name):
            if parent_name in to_enable:
                continue
            print("Goost: Enabling `%s` component (parent of `%s`)." % (parent_name, name))
            env["goost_%s_enabled" % parent_name] = True
            to_enable.append(parent_name)
            components["disabled"].remove(parent_name)

    if enabled_by_default:
        # Disable child components, if any.
        to_disable = components["disabled"]
        for name in components["disabled"]:
            disable_child_components(name)
        components["disabled"] = to_disable
    else:
        # Enable parent components, if any.
        # This is needed because otherwise child components won't build at all.
        to_enable = components["enabled"]
        for name in components["enabled"]:
            enable_parent_components(name)
        components["enabled"] = to_enable

    # Finally, define all enabled and disabled components
    # in the environment for easier parsing in SCsub.
    env["goost_components_enabled"] = components["enabled"]
    env["goost_components_disabled"] = components["disabled"]

    return components


def configure_classes(env, config, enabled_by_default):
    # Individual classes (for when configuring components is not enough).
    # Can only be configured via `custom.py` file.
    classes = goost.get_classes(config, enabled_by_default)

    env["goost_classes_enabled"] = classes["enabled"]
    env["goost_classes_disabled"] = classes["disabled"]

    return classes


def get_doc_classes():
    return goost.classes.keys()


def get_doc_path():
    return "doc"


def get_icons_path():
    return "editor/icons"    


def get_modules_path():
    return "modules"
