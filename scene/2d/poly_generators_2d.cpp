#include "poly_generators_2d.h"

#include "core/core_string_names.h"
#include "scene/scene_string_names.h"

#include "goost/core/math/geometry/2d/goost_geometry_2d.h"

// PolyCircle2D

void PolyCircle2D::set_radius(real_t p_radius) {
	ERR_FAIL_COND(p_radius <= 0);
	radius = p_radius;
	_queue_update();
	_change_notify("radius");
}

void PolyCircle2D::set_max_error(real_t p_max_error) {
	ERR_FAIL_COND(p_max_error <= 0);
	max_error = p_max_error;
	_queue_update();
	_change_notify("max_error");
}

Vector<Vector<Point2>> PolyCircle2D::_build_outlines() {
	Vector<Vector<Point2>> outlines;
	outlines.push_back(GoostGeometry2D::circle(radius, max_error));
	return outlines;
}

void PolyCircle2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &PolyCircle2D::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &PolyCircle2D::get_radius);

	ClassDB::bind_method(D_METHOD("set_max_error", "max_error"), &PolyCircle2D::set_max_error);
	ClassDB::bind_method(D_METHOD("get_max_error"), &PolyCircle2D::get_max_error);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius", PROPERTY_HINT_RANGE, "0.01,1024.0,0.01,or_greater"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_error", PROPERTY_HINT_RANGE, "0.01,5.0,0.01,or_greater"), "set_max_error", "get_max_error");
}

// PolyEllipse2D

void PolyEllipse2D::set_width(real_t p_width) {
	ERR_FAIL_COND(p_width <= 0);
	width = p_width;
	_queue_update();
	_change_notify("width");
}

void PolyEllipse2D::set_height(real_t p_height) {
	ERR_FAIL_COND(p_height <= 0);
	height = p_height;
	_queue_update();
	_change_notify("height");
}

void PolyEllipse2D::set_max_error(real_t p_max_error) {
	ERR_FAIL_COND(p_max_error <= 0);
	max_error = p_max_error;
	_queue_update();
	_change_notify("max_error");
}

Vector<Vector<Point2>> PolyEllipse2D::_build_outlines() {
	Vector<Vector<Point2>> outlines;
	outlines.push_back(GoostGeometry2D::ellipse(width, height, max_error));
	return outlines;
}

void PolyEllipse2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_width", "width"), &PolyEllipse2D::set_width);
	ClassDB::bind_method(D_METHOD("get_width"), &PolyEllipse2D::get_width);

	ClassDB::bind_method(D_METHOD("set_height", "height"), &PolyEllipse2D::set_height);
	ClassDB::bind_method(D_METHOD("get_height"), &PolyEllipse2D::get_height);

	ClassDB::bind_method(D_METHOD("set_max_error", "max_error"), &PolyEllipse2D::set_max_error);
	ClassDB::bind_method(D_METHOD("get_max_error"), &PolyEllipse2D::get_max_error);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "width", PROPERTY_HINT_RANGE, "0.01,1024.0,0.01,or_greater"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "height", PROPERTY_HINT_RANGE, "0.01,1024.0,0.01,or_greater"), "set_height", "get_height");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_error", PROPERTY_HINT_RANGE, "0.01,5.0,0.01,or_greater"), "set_max_error", "get_max_error");
}

// PolyCapsule2D

void PolyCapsule2D::set_radius(real_t p_radius) {
	ERR_FAIL_COND(p_radius <= 0.0);
	radius = p_radius;
	_queue_update();
	_change_notify("radius");
}

void PolyCapsule2D::set_height(real_t p_height) {
	ERR_FAIL_COND(p_height < 0.0);
	height = p_height;
	_queue_update();
	_change_notify("height");
}

void PolyCapsule2D::set_max_error(real_t p_max_error) {
	ERR_FAIL_COND(p_max_error <= 0.0);
	max_error = p_max_error;
	_queue_update();
	_change_notify("max_error");
}

Vector<Vector<Point2>> PolyCapsule2D::_build_outlines() {
	Vector<Vector<Point2>> outlines;
	outlines.push_back(GoostGeometry2D::capsule(radius, height, max_error));
	return outlines;
}

void PolyCapsule2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &PolyCapsule2D::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &PolyCapsule2D::get_radius);

	ClassDB::bind_method(D_METHOD("set_height", "height"), &PolyCapsule2D::set_height);
	ClassDB::bind_method(D_METHOD("get_height"), &PolyCapsule2D::get_height);

	ClassDB::bind_method(D_METHOD("set_max_error", "max_error"), &PolyCapsule2D::set_max_error);
	ClassDB::bind_method(D_METHOD("get_max_error"), &PolyCapsule2D::get_max_error);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius", PROPERTY_HINT_RANGE, "0.01,1024.0,0.01,or_greater"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "height", PROPERTY_HINT_RANGE, "0.0,1024.0,0.01,or_greater"), "set_height", "get_height");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_error", PROPERTY_HINT_RANGE, "0.01,5.0,0.01,or_greater"), "set_max_error", "get_max_error");
}

// PolyRectangle2D

void PolyRectangle2D::set_extents(const Vector2 &p_extents) {
	ERR_FAIL_COND(p_extents.x <= 0.0);
	ERR_FAIL_COND(p_extents.y <= 0.0);
	extents = p_extents;
	_queue_update();
	_change_notify("extents");
}

Vector<Vector<Point2>> PolyRectangle2D::_build_outlines() {
	Vector<Vector<Point2>> outlines;
	outlines.push_back(GoostGeometry2D::rectangle(extents));
	return outlines;
}

void PolyRectangle2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_extents", "extents"), &PolyRectangle2D::set_extents);
	ClassDB::bind_method(D_METHOD("get_extents"), &PolyRectangle2D::get_extents);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "extents"), "set_extents", "get_extents");
}

// PolyPath2D

void PolyPath2D::add_child_notify(Node *p_child) {
	Node2D::add_child_notify(p_child);

	Path2D *path = Object::cast_to<Path2D>(p_child);
	if (!path) {
		return;
	}
	Ref<Curve2D> curve = path->get_curve();
	paths.insert(path->get_instance_id(), curve);
	path->connect(SceneStringNames::get_singleton()->visibility_changed, this, "_queue_update");

	if (curve.is_valid()) {
		curve->connect(CoreStringNames::get_singleton()->changed, this, "_queue_update");
	}
	_queue_update();
}

void PolyPath2D::remove_child_notify(Node *p_child) {
	Node2D::remove_child_notify(p_child);

	Path2D *path = Object::cast_to<Path2D>(p_child);
	if (!path) {
		return;
	}
	Ref<Curve2D> curve = path->get_curve();
	paths.erase(path->get_instance_id());
	path->disconnect(SceneStringNames::get_singleton()->visibility_changed, this, "_queue_update");

	if (curve.is_valid()) {
		if (curve->is_connected(CoreStringNames::get_singleton()->changed, this, "_queue_update")) {
			curve->disconnect(CoreStringNames::get_singleton()->changed, this, "_queue_update");
		}
	}
	_queue_update();
}

void PolyPath2D::set_buffer_offset(real_t p_buffer_offset) {
	buffer_offset = MAX(0.0, p_buffer_offset);
	_queue_update();
	_change_notify("buffer_offset");
}

void PolyPath2D::set_buffer_parameters(const Ref<PolyOffsetParameters2D> &p_buffer_parameters) {
	if (buffer_parameters.is_valid()) {
		if (buffer_parameters->is_connected(CoreStringNames::get_singleton()->changed, this, "_queue_update")) {
			buffer_parameters->disconnect(CoreStringNames::get_singleton()->changed, this, "_queue_update");
		}
	}
	buffer_parameters = p_buffer_parameters;

	if (buffer_parameters.is_valid()) {
		buffer_parameters->connect(CoreStringNames::get_singleton()->changed, this, "_queue_update");
	}
	_queue_update();
	_change_notify("buffer_parameters");
}

void PolyPath2D::set_tessellation_stages(int p_tessellation_stages) {
	tessellation_stages = MAX(1, p_tessellation_stages);
	_queue_update();
	_change_notify("tessellation_stages");
}

void PolyPath2D::set_tessellation_tolerance_degrees(float p_tessellation_tolerance_degrees) {
	tessellation_tolerance_degrees = CLAMP(p_tessellation_tolerance_degrees, 0.0f, 180.0f);
	_queue_update();
	_change_notify("tessellation_tolerance_degrees");
}

Vector<Vector<Point2>> PolyPath2D::_build_outlines() {
	Vector<Vector<Point2>> outlines;
	Vector<Vector<Point2>> to_deflate;

	// Do not use cached Curve2D here E->get(), it's only useful for keeping
	// track of signal connections during NOTIFICATION_INTERNAL_PROCESS.
	for (Map<ObjectID, Ref<Curve2D>>::Element *E = paths.front(); E; E = E->next()) {
		Object *obj = ObjectDB::get_instance(E->key());
		ERR_FAIL_NULL_V(obj, Vector<Vector<Point2>>());

		const Path2D *path = Object::cast_to<Path2D>(obj);
		ERR_FAIL_NULL_V(path, Vector<Vector<Point2>>());

		if (!path->is_visible_in_tree()) {
			continue;
		}
		const Ref<Curve2D> &curve = path->get_curve();
		if (curve.is_null() || curve->get_point_count() == 0) {
			continue;
		}
		// Tessellate!
		PoolVector2Array tessellated = curve->tessellate(tessellation_stages, tessellation_tolerance_degrees);
		Vector<Point2> polyline;
		for (int i = 0; i < tessellated.size(); ++i) {
			polyline.push_back(tessellated[i]);
		}
		to_deflate.push_back(polyline);
	}
	// Deflate!
	if (buffer_offset > 0.0) {
		outlines = PolyOffset2D::deflate_polylines(to_deflate, buffer_offset, buffer_parameters);
	} else {
		outlines = to_deflate;
	}
	return outlines;
}

void PolyPath2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			for (Map<ObjectID, Ref<Curve2D>>::Element *E = paths.front(); E; E = E->next()) {
				Object *obj = ObjectDB::get_instance(E->key());
				ERR_FAIL_NULL(obj);

				const Path2D *path = Object::cast_to<Path2D>(obj);
				ERR_FAIL_NULL(path);

				Ref<Curve2D> curve_cached = E->get();
				Ref<Curve2D> curve_current = path->get_curve();

				if (curve_current != curve_cached) {
					if (curve_cached.is_valid()) {
						if (curve_cached->is_connected(CoreStringNames::get_singleton()->changed, this, "_queue_update")) {
							curve_cached->disconnect(CoreStringNames::get_singleton()->changed, this, "_queue_update");
						}
					}
					paths[E->key()] = curve_current;
					if (curve_current.is_valid()) {
						curve_current->connect(CoreStringNames::get_singleton()->changed, this, "_queue_update");
					}
					_queue_update();
				}
			}
		} break;
	}
}

String PolyPath2D::get_configuration_warning() const {
	String warning = Node2D::get_configuration_warning();

	if (paths.empty()) {
		if (!warning.empty()) {
			warning += "/n/n";
		}
		warning += TTR("Add Path2D with Curve2D as a child to this node to define the path.");
	}
	return warning;
}

void PolyPath2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_buffer_offset", "buffer_offset"), &PolyPath2D::set_buffer_offset);
	ClassDB::bind_method(D_METHOD("get_buffer_offset"), &PolyPath2D::get_buffer_offset);

	ClassDB::bind_method(D_METHOD("set_buffer_parameters", "buffer_parameters"), &PolyPath2D::set_buffer_parameters);
	ClassDB::bind_method(D_METHOD("get_buffer_parameters"), &PolyPath2D::get_buffer_parameters);

	ClassDB::bind_method(D_METHOD("set_tessellation_stages", "tessellation_stages"), &PolyPath2D::set_tessellation_stages);
	ClassDB::bind_method(D_METHOD("get_tessellation_stages"), &PolyPath2D::get_tessellation_stages);

	ClassDB::bind_method(D_METHOD("set_tessellation_tolerance_degrees", "tessellation_tolerance_degrees"), &PolyPath2D::set_tessellation_tolerance_degrees);
	ClassDB::bind_method(D_METHOD("get_tessellation_tolerance_degrees"), &PolyPath2D::get_tessellation_tolerance_degrees);

	ADD_GROUP("Buffer", "buffer_");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "buffer_offset", PROPERTY_HINT_RANGE, "0.01,256.0,0.01,or_greater"), "set_buffer_offset", "get_buffer_offset");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "buffer_parameters", PROPERTY_HINT_RESOURCE_TYPE, "PolyOffsetParameters2D"), "set_buffer_parameters", "get_buffer_parameters");

	ADD_GROUP("Tessellation", "tessellation_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "tessellation_stages", PROPERTY_HINT_RANGE, "1,6,1"), "set_tessellation_stages", "get_tessellation_stages");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "tessellation_tolerance_degrees", PROPERTY_HINT_RANGE, "1.0,180.0,0.1"), "set_tessellation_tolerance_degrees", "get_tessellation_tolerance_degrees");
}
