#include "poly_collision_shape_2d.h"

#include "core/engine.h"
#include "scene/2d/collision_object_2d.h"
#include "scene/resources/concave_polygon_shape_2d.h"
#include "scene/resources/convex_polygon_shape_2d.h"

void PolyCollisionShape2D::_apply_shapes() {
	parent->shape_owner_clear_shapes(owner_id);
	if (shapes.empty()) {
		return;
	}
	switch (build_mode) {
		case BUILD_TRIANGLES:
		case BUILD_CONVEX: {
			for (int i = 0; i < shapes.size(); i++) {
				Ref<ConvexPolygonShape2D> convex = memnew(ConvexPolygonShape2D);
				convex->set_points(shapes[i]);
				parent->shape_owner_add_shape(owner_id, convex);
			}
		} break;
		case BUILD_CONCAVE: {
			for (int i = 0; i < shapes.size(); i++) {
				const Vector<Point2> &polygon = shapes[i];
				Ref<ConcavePolygonShape2D> concave = memnew(ConcavePolygonShape2D);

				PoolVector<Vector2> segments;
				segments.resize(polygon.size() * 2);
				PoolVector<Vector2>::Write w = segments.write();

				for (int i = 0; i < polygon.size(); i++) {
					w[(i << 1) + 0] = polygon[i];
					w[(i << 1) + 1] = polygon[(i + 1) % polygon.size()];
				}
				w.release();
				concave->set_segments(segments);

				parent->shape_owner_add_shape(owner_id, concave);
			}
		} break;
	}
}

void PolyCollisionShape2D::_update_in_shape_owner(bool p_xform_only) {
	parent->shape_owner_set_transform(owner_id, get_transform());
	if (p_xform_only) {
		return;
	}
	parent->shape_owner_set_disabled(owner_id, disabled);
	parent->shape_owner_set_one_way_collision(owner_id, one_way_collision);
	parent->shape_owner_set_one_way_collision_margin(owner_id, one_way_collision_margin);
}

void PolyCollisionShape2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PARENTED: {
			parent = Object::cast_to<CollisionObject2D>(get_parent());
			if (parent) {
				owner_id = parent->create_shape_owner(this);
				// Shapes are applied in NOTIFICATION_READY, not here.
				_update_in_shape_owner();
			}
		} break;
		case NOTIFICATION_READY: {
			_apply_shapes();
			_update_in_shape_owner();
		} break;
		case NOTIFICATION_ENTER_TREE: {
			if (parent) {
				_update_in_shape_owner();
			}
		} break;
		case NOTIFICATION_LOCAL_TRANSFORM_CHANGED: {
			if (parent) {
				_update_in_shape_owner(true);
			}
		} break;
		case NOTIFICATION_UNPARENTED: {
			if (parent) {
				parent->remove_shape_owner(owner_id);
			}
			owner_id = 0;
			parent = nullptr;
		} break;
	}
}

void PolyCollisionShape2D::set_disabled(bool p_disabled) {
	disabled = p_disabled;
	update();
	if (parent) {
		parent->shape_owner_set_disabled(owner_id, p_disabled);
	}
}

void PolyCollisionShape2D::set_one_way_collision(bool p_enable) {
	one_way_collision = p_enable;
	update();
	if (parent) {
		parent->shape_owner_set_one_way_collision(owner_id, p_enable);
	}
}

void PolyCollisionShape2D::set_one_way_collision_margin(float p_margin) {
	one_way_collision_margin = p_margin;
	if (parent) {
		parent->shape_owner_set_one_way_collision_margin(owner_id, one_way_collision_margin);
	}
}

void PolyCollisionShape2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_disabled", "disabled"), &PolyCollisionShape2D::set_disabled);
	ClassDB::bind_method(D_METHOD("is_disabled"), &PolyCollisionShape2D::is_disabled);

	ClassDB::bind_method(D_METHOD("set_one_way_collision", "enabled"), &PolyCollisionShape2D::set_one_way_collision);
	ClassDB::bind_method(D_METHOD("is_one_way_collision_enabled"), &PolyCollisionShape2D::is_one_way_collision_enabled);

	ClassDB::bind_method(D_METHOD("set_one_way_collision_margin", "margin"), &PolyCollisionShape2D::set_one_way_collision_margin);
	ClassDB::bind_method(D_METHOD("get_one_way_collision_margin"), &PolyCollisionShape2D::get_one_way_collision_margin);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "disabled"), "set_disabled", "is_disabled");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "one_way_collision"), "set_one_way_collision", "is_one_way_collision_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "one_way_collision_margin", PROPERTY_HINT_RANGE, "0,128,0.1"), "set_one_way_collision_margin", "get_one_way_collision_margin");
}

PolyCollisionShape2D::PolyCollisionShape2D() {
	set_notify_local_transform(true);
}
