#!/usr/bin/env python3

def can_build(env, platform):
	return True

def configure(env):
	pass

def is_enabled():
	# Disabled by default being experimental at the moment.
	# Enable manually with `module_gdscript_transpiler_enabled=yes` option.
	return False
