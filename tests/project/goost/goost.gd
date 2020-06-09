class_name Goost

# This avoids the many warnings when we load an image from `res://` path which
# unnecessarily pollute the output, so just globalize those paths to OS paths.
static func image_load_no_warning(filepath):
	var input = Image.new()
	input.load(ProjectSettings.globalize_path(filepath))
	return input
