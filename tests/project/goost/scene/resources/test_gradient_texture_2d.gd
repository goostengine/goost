extends "res://addons/gut/test.gd"


func test_linear():
    var texture = GradientTexture2D.new()
    var gradient = Gradient.new()
    texture.gradient = gradient

    yield(texture, "changed")

    var image = texture.get_data()
    assert_not_null(image)

    image.lock()
    assert_eq(image.get_pixel(0, 0), Color.black)
    assert_eq(image.get_pixel(texture.get_width() - 1, 0), Color.white)
    image.unlock()


func test_radial():
    var texture = GradientTexture2D.new()
    var gradient = Gradient.new()
    texture.gradient = gradient
    texture.fill = GradientTexture2D.FILL_RADIAL
    texture.fill_from = Vector2(0.5, 0.5)
    texture.fill_to = Vector2(0.5, 0)

    yield(texture, "changed")

    var image = texture.get_data()
    assert_not_null(image)
    
    image.lock()
    assert_eq(image.get_pixel(0, 0), Color.white)
    assert_ne(image.get_pixel(texture.get_width() - 1, 0), Color.black)
    image.unlock()


func test_gradient_single_color():
    var texture = GradientTexture2D.new()
    var gradient = Gradient.new()
    for i in gradient.get_point_count():
        gradient.remove_point(i)
    texture.gradient = gradient

    yield(texture, "changed")

    var image = texture.get_data()
    assert_not_null(image)

    image.lock()
    assert_eq(image.get_pixel(0, 0), Color.white)
    assert_eq(image.get_pixel(texture.get_width() - 1, 0), Color.white)
    image.unlock()
