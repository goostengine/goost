#include "shape_cast_2d.h"

#include "scene/2d/collision_object_2d.h"
#include "scene/resources/circle_shape_2d.h"
#include "scene/2d/physics_body_2d.h"
#include "core/engine.h"
#include "servers/physics_2d_server.h"
#include "core/core_string_names.h"

void ShapeCast2D::set_cast_to(const Vector2 &p_point) {
	cast_to = p_point;
	if (is_inside_tree() && (Engine::get_singleton()->is_editor_hint() || get_tree()->is_debugging_collisions_hint()))
		update();
}

Vector2 ShapeCast2D::get_cast_to() const {
	return cast_to;
}

void ShapeCast2D::set_margin(real_t p_margin) {
	margin = p_margin;
}

real_t ShapeCast2D::get_margin() const {
	return margin;
}

void ShapeCast2D::set_max_results(int p_max_results) {
	max_results = p_max_results;
}

int ShapeCast2D::get_max_results() const {
	return max_results;
}

void ShapeCast2D::set_collision_mask(uint32_t p_mask) {
	collision_mask = p_mask;
}

uint32_t ShapeCast2D::get_collision_mask() const {

	return collision_mask;
}

void ShapeCast2D::set_collision_mask_bit(int p_bit, bool p_value) {

	uint32_t mask = get_collision_mask();
	if (p_value)
		mask |= 1 << p_bit;
	else
		mask &= ~(1 << p_bit);
	set_collision_mask(mask);
}

bool ShapeCast2D::get_collision_mask_bit(int p_bit) const {

	return get_collision_mask() & (1 << p_bit);
}

int ShapeCast2D::get_collision_count() const {
	return result.size();
}

bool ShapeCast2D::is_colliding() const {
	return collided;
}

Object *ShapeCast2D::get_collider(int p_idx) const {
	ERR_FAIL_INDEX_V_MSG(p_idx, result.size(), NULL, "No collider found.");
	
	if (result[p_idx].collider_id == 0)
		return NULL;

	return ObjectDB::get_instance(result[p_idx].collider_id);
}

int ShapeCast2D::get_collider_shape(int p_idx) const {
	ERR_FAIL_INDEX_V_MSG(p_idx, result.size(), -1, "No collider shape found.");
	return result[p_idx].shape;
}

Vector2 ShapeCast2D::get_collision_point(int p_idx) const {
	ERR_FAIL_INDEX_V_MSG(p_idx, result.size(), Vector2(), "No collision point found.");
	return result[p_idx].point;
}

Vector2 ShapeCast2D::get_collision_normal(int p_idx) const {
	ERR_FAIL_INDEX_V_MSG(p_idx, result.size(), Vector2(), "No collision normal found.");
	return result[p_idx].normal;
}

real_t ShapeCast2D::get_closest_collision_safe_distance() const {
	return collision_safe_distance;
}

real_t ShapeCast2D::get_closest_collision_unsafe_distance() const {
	return collision_unsafe_distance;
}

Object *ShapeCast2D::get_closest_collider() const {
	ERR_FAIL_COND_V_MSG(result.empty(), NULL, "Shape cast has not collided with anything yet.");
	return ObjectDB::get_instance(result[0].collider_id);
}

int ShapeCast2D::get_closest_collider_shape() const {
	ERR_FAIL_COND_V_MSG(result.empty(), -1, "Shape cast has not collided with anything yet.");
	return result[0].shape;
}

Vector2 ShapeCast2D::get_closest_collision_point() const {
	ERR_FAIL_COND_V_MSG(result.empty(), Vector2(), "Shape cast has not collided with anything yet.");
	return result[0].point;
}

Vector2 ShapeCast2D::get_closest_collision_normal() const {
	ERR_FAIL_COND_V_MSG(result.empty(), Vector2(), "Shape cast has not collided with anything yet.");
	return result[0].normal;
}

void ShapeCast2D::set_enabled(bool p_enabled) {
	enabled = p_enabled;
	update();
	if (is_inside_tree() && !Engine::get_singleton()->is_editor_hint())
		set_physics_process_internal(p_enabled);
	if (!p_enabled)
		collided = false;
}

bool ShapeCast2D::is_enabled() const {
	return enabled;
}

void ShapeCast2D::set_shape(const Ref<Shape2D> &p_shape) {
	shape = p_shape;
	if (p_shape.is_valid()) {
		shape->connect(CoreStringNames::get_singleton()->changed, this, "update");
		shape_rid = shape->get_rid();
	}
	update();
}

Ref<Shape2D> ShapeCast2D::get_shape() const {
	return shape;
}

void ShapeCast2D::set_exclude_parent_body(bool p_exclude_parent_body) {

	if (exclude_parent_body == p_exclude_parent_body)
		return;

	exclude_parent_body = p_exclude_parent_body;

	if (!is_inside_tree())
		return;

	if (Object::cast_to<CollisionObject2D>(get_parent())) {
		if (exclude_parent_body)
			exclude.insert(Object::cast_to<CollisionObject2D>(get_parent())->get_rid());
		else
			exclude.erase(Object::cast_to<CollisionObject2D>(get_parent())->get_rid());
	}
}

bool ShapeCast2D::get_exclude_parent_body() const {

	return exclude_parent_body;
}

void ShapeCast2D::_notification(int p_what) {

	switch (p_what) {

		case NOTIFICATION_ENTER_TREE: {

			if (enabled && !Engine::get_singleton()->is_editor_hint())
				set_physics_process_internal(true);
			else
				set_physics_process_internal(false);

			if (Object::cast_to<CollisionObject2D>(get_parent())) {
				if (exclude_parent_body)
					exclude.insert(Object::cast_to<CollisionObject2D>(get_parent())->get_rid());
				else
					exclude.erase(Object::cast_to<CollisionObject2D>(get_parent())->get_rid());
			}
		} break;
		case NOTIFICATION_EXIT_TREE: {

			if (enabled)
				set_physics_process_internal(false);

		} break;

		case NOTIFICATION_DRAW: {
#ifdef TOOLS_ENABLED
			if (!Engine::get_singleton()->is_editor_hint() && !get_tree()->is_debugging_collisions_hint()) {
				break;
			}
			if (shape.is_null()) {
				break;
			}
			Color draw_col = get_tree()->get_debug_collisions_color();
			if (!enabled) {
				float g = draw_col.get_v();
				draw_col.r = g;
				draw_col.g = g;
				draw_col.b = g;
			}
			// Draw continuos chain of shapes along the cast.
			const int steps = MAX(2, cast_to.length() / shape->get_rect().get_size().length() * 4);
			for (int i = 0; i <= steps; ++i) {
				draw_set_transform(Vector2().linear_interpolate(cast_to, real_t(i) / steps), 0.0, Size2(1, 1));
				shape->draw(get_canvas_item(), draw_col);
			}
			draw_set_transform(Vector2(), 0.0, Size2(1, 1));
			
			// Draw an arrow indicating where the ShapeCast is pointing to.
			if (cast_to != Vector2()) {
				Transform2D xf;
				xf.rotate(cast_to.angle());
				xf.translate(Vector2(cast_to.length(), 0));
				
				draw_line(Vector2(), cast_to, draw_col, 2);
				Vector<Vector2> pts;
				float tsize = 8;
				pts.push_back(xf.xform(Vector2(tsize, 0)));
				pts.push_back(xf.xform(Vector2(0, Math_SQRT12 * tsize)));
				pts.push_back(xf.xform(Vector2(0, -Math_SQRT12 * tsize)));
				Vector<Color> cols;
				for (int i = 0; i < 3; i++)
					cols.push_back(draw_col);

				draw_primitive(pts, cols, Vector<Vector2>());
			}
#endif
		} break;
		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			if (!enabled)
				break;
			_update_shapecast_state();
		} break;
	}
}

void ShapeCast2D::_update_shapecast_state() {
	ERR_FAIL_COND_MSG(shape.is_null(), "Invalid shape.");
	
	Ref<World2D> w2d = get_world_2d();
	ERR_FAIL_COND(w2d.is_null());

	Physics2DDirectSpaceState *dss = Physics2DServer::get_singleton()->space_get_direct_state(w2d->get_space());
	ERR_FAIL_COND(!dss);

	Transform2D gt = get_global_transform();
	bool process_intersections = true;
	
	if (cast_to != Vector2()) {
		Vector2 to = gt.basis_xform(cast_to);
		
		bool can_move = dss->cast_motion(shape_rid, gt, to, margin, 
				collision_safe_distance, collision_unsafe_distance, 
				exclude, collision_mask, collide_with_bodies, collide_with_areas);
				
		gt.set_origin(gt.get_origin() + to * collision_unsafe_distance);
		
		process_intersections = !can_move || collision_unsafe_distance < 1.0;
	}
	if (process_intersections) {
		result.clear();
		
		bool intersected = true;
		Set<RID> checked = exclude;
		
		while (intersected && result.size() < max_results) {
			Physics2DDirectSpaceState::ShapeRestInfo sri;
			intersected = dss->rest_info(shape_rid, gt, Vector2(), margin, &sri, 
					checked, collision_mask, collide_with_bodies, collide_with_areas);
			if (intersected) {
				result.push_back(sri);
				checked.insert(sri.rid);
			}
		}
	}
	collided = process_intersections && !result.empty();
}

void ShapeCast2D::force_shapecast_update() {
	_update_shapecast_state();
}

void ShapeCast2D::add_exception_rid(const RID &p_rid) {

	exclude.insert(p_rid);
}

void ShapeCast2D::add_exception(const Object *p_object) {

	ERR_FAIL_NULL(p_object);
	const CollisionObject2D *co = Object::cast_to<CollisionObject2D>(p_object);
	if (!co)
		return;
	add_exception_rid(co->get_rid());
}

void ShapeCast2D::remove_exception_rid(const RID &p_rid) {

	exclude.erase(p_rid);
}

void ShapeCast2D::remove_exception(const Object *p_object) {

	ERR_FAIL_NULL(p_object);
	const CollisionObject2D *co = Object::cast_to<CollisionObject2D>(p_object);
	if (!co)
		return;
	remove_exception_rid(co->get_rid());
}

void ShapeCast2D::clear_exceptions() {

	exclude.clear();
}

void ShapeCast2D::set_collide_with_areas(bool p_clip) {

	collide_with_areas = p_clip;
}

bool ShapeCast2D::is_collide_with_areas_enabled() const {

	return collide_with_areas;
}

void ShapeCast2D::set_collide_with_bodies(bool p_clip) {

	collide_with_bodies = p_clip;
}

bool ShapeCast2D::is_collide_with_bodies_enabled() const {

	return collide_with_bodies;
}

Array ShapeCast2D::_get_collision_result() const {
	Array ret;

	for (int i = 0; i < result.size(); ++i) {
		const Physics2DDirectSpaceState::ShapeRestInfo &sri = result[i];
		
		Dictionary col;
		col["point"] = sri.point;
		col["normal"] = sri.normal;
		col["rid"] = sri.rid;
		col["collider"] = ObjectDB::get_instance(sri.collider_id);
		col["collider_id"] = sri.collider_id;
		col["shape"] = sri.shape;
		col["linear_velocity"] = sri.linear_velocity;
		col["metadata"] = sri.metadata;
		
		ret.push_back(col);
	}
	return ret;
}

String ShapeCast2D::get_configuration_warning() const {

	String warning = Node2D::get_configuration_warning();

	if (shape.is_null()) {
		if (!warning.empty()) {
			warning += "\n\n";
		}
		warning += TTR("This node cannot interact with other objects unless a Shape2D is assigned.");
	}

	return warning;
}

void ShapeCast2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &ShapeCast2D::set_enabled);
	ClassDB::bind_method(D_METHOD("is_enabled"), &ShapeCast2D::is_enabled);
	
	ClassDB::bind_method(D_METHOD("set_shape", "shape"), &ShapeCast2D::set_shape);
	ClassDB::bind_method(D_METHOD("get_shape"), &ShapeCast2D::get_shape);
	
	ClassDB::bind_method(D_METHOD("set_cast_to", "local_point"), &ShapeCast2D::set_cast_to);
	ClassDB::bind_method(D_METHOD("get_cast_to"), &ShapeCast2D::get_cast_to);
	
	ClassDB::bind_method(D_METHOD("set_margin", "margin"), &ShapeCast2D::set_margin);
	ClassDB::bind_method(D_METHOD("get_margin"), &ShapeCast2D::get_margin);
	
	ClassDB::bind_method(D_METHOD("set_max_results", "max_results"), &ShapeCast2D::set_max_results);
	ClassDB::bind_method(D_METHOD("get_max_results"), &ShapeCast2D::get_max_results);

	ClassDB::bind_method(D_METHOD("is_colliding"), &ShapeCast2D::is_colliding);
	ClassDB::bind_method(D_METHOD("get_collision_count"), &ShapeCast2D::get_collision_count);
	
	ClassDB::bind_method(D_METHOD("force_shapecast_update"), &ShapeCast2D::force_shapecast_update);

	ClassDB::bind_method(D_METHOD("get_collider", "index"), &ShapeCast2D::get_collider);
	ClassDB::bind_method(D_METHOD("get_collider_shape", "index"), &ShapeCast2D::get_collider_shape);
	ClassDB::bind_method(D_METHOD("get_collision_point", "index"), &ShapeCast2D::get_collision_point);
	ClassDB::bind_method(D_METHOD("get_collision_normal", "index"), &ShapeCast2D::get_collision_normal);

	ClassDB::bind_method(D_METHOD("get_closest_collider"), &ShapeCast2D::get_closest_collider);
	ClassDB::bind_method(D_METHOD("get_closest_collider_shape"), &ShapeCast2D::get_closest_collider_shape);
	ClassDB::bind_method(D_METHOD("get_closest_collision_point"), &ShapeCast2D::get_closest_collision_point);
	ClassDB::bind_method(D_METHOD("get_closest_collision_normal"), &ShapeCast2D::get_closest_collision_normal);
	ClassDB::bind_method(D_METHOD("get_closest_collision_safe_distance"), &ShapeCast2D::get_closest_collision_safe_distance);
	ClassDB::bind_method(D_METHOD("get_closest_collision_unsafe_distance"), &ShapeCast2D::get_closest_collision_unsafe_distance);

	ClassDB::bind_method(D_METHOD("add_exception_rid", "rid"), &ShapeCast2D::add_exception_rid);
	ClassDB::bind_method(D_METHOD("add_exception", "node"), &ShapeCast2D::add_exception);

	ClassDB::bind_method(D_METHOD("remove_exception_rid", "rid"), &ShapeCast2D::remove_exception_rid);
	ClassDB::bind_method(D_METHOD("remove_exception", "node"), &ShapeCast2D::remove_exception);

	ClassDB::bind_method(D_METHOD("clear_exceptions"), &ShapeCast2D::clear_exceptions);

	ClassDB::bind_method(D_METHOD("set_collision_mask", "mask"), &ShapeCast2D::set_collision_mask);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &ShapeCast2D::get_collision_mask);

	ClassDB::bind_method(D_METHOD("set_collision_mask_bit", "bit", "value"), &ShapeCast2D::set_collision_mask_bit);
	ClassDB::bind_method(D_METHOD("get_collision_mask_bit", "bit"), &ShapeCast2D::get_collision_mask_bit);

	ClassDB::bind_method(D_METHOD("set_exclude_parent_body", "mask"), &ShapeCast2D::set_exclude_parent_body);
	ClassDB::bind_method(D_METHOD("get_exclude_parent_body"), &ShapeCast2D::get_exclude_parent_body);

	ClassDB::bind_method(D_METHOD("set_collide_with_areas", "enable"), &ShapeCast2D::set_collide_with_areas);
	ClassDB::bind_method(D_METHOD("is_collide_with_areas_enabled"), &ShapeCast2D::is_collide_with_areas_enabled);

	ClassDB::bind_method(D_METHOD("set_collide_with_bodies", "enable"), &ShapeCast2D::set_collide_with_bodies);
	ClassDB::bind_method(D_METHOD("is_collide_with_bodies_enabled"), &ShapeCast2D::is_collide_with_bodies_enabled);
	
	ClassDB::bind_method(D_METHOD("_get_collision_result"), &ShapeCast2D::_get_collision_result);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shape", PROPERTY_HINT_RESOURCE_TYPE, "Shape2D"), "set_shape", "get_shape");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "exclude_parent"), "set_exclude_parent_body", "get_exclude_parent_body");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "cast_to"), "set_cast_to", "get_cast_to");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "margin", PROPERTY_HINT_RANGE, "0,100,0.01"), "set_margin", "get_margin");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_results"), "set_max_results", "get_max_results");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_mask", "get_collision_mask");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "collision_result", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "", "_get_collision_result");
	ADD_GROUP("Collide With", "collide_with");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collide_with_areas", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collide_with_areas", "is_collide_with_areas_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collide_with_bodies", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collide_with_bodies", "is_collide_with_bodies_enabled");
}

ShapeCast2D::ShapeCast2D() {
	enabled = true;
	collided = false;
	margin = 0.0;
	collision_safe_distance = 1.0;
	collision_unsafe_distance = 1.0;
	max_results = 32;
	collision_mask = 1;
	cast_to = Vector2(0, 50);
	exclude_parent_body = true;
	collide_with_bodies = true;
	collide_with_areas = false;
}
