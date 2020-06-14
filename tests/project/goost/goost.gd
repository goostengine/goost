class_name Goost

# Note: do not rely on the import process to load assets, as it requires 
# a sophisticated approach of importing the assets from the command line,
# which may not work reliably if the tests are run from within CI systems.

static func image_load(filepath):
	var image = Image.new()
	# Avoid the "loaded from res:// path" warning.
	image.load(ProjectSettings.globalize_path(filepath))
	return image
