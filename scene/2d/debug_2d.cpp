#include "debug_2d.h"

#include "scene/scene_string_names.h"

Debug2D *Debug2D::singleton = nullptr;

void Debug2D::draw_polyline(const Vector<Point2> &p_polyline, const Color &p_color, real_t p_width) {
	ERR_FAIL_COND(p_polyline.size() < 2);

	DrawCommand c;
	c.type = DrawCommand::POLYLINE;
	c.args.push_back(p_polyline);
	c.args.push_back(p_color);
	c.args.push_back(p_width);
	commands.push_back(c);
}

void Debug2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			canvas_item = memnew(Node2D);
			canvas_item->connect(SceneStringNames::get_singleton()->draw, this, "_on_canvas_item_draw");
			canvas_item->set_name("Canvas");
			add_child(canvas_item);
		} break;
		case NOTIFICATION_EXIT_TREE: {
		} break;
		case NOTIFICATION_PROCESS: {
			// canvas_item->update();
		} break;
	}
}

void Debug2D::set_snapshot(int p_snapshot) {
	snapshot = p_snapshot;
	update();
}

void Debug2D::commit() {
	DrawCommand c;
	c.type = DrawCommand::COMMIT;
	commands.push_back(c);
}

void Debug2D::update() {
	canvas_item->update();
}

void Debug2D::clear() {
	commands.clear();
	snapshot = -1;
	update();
}

void Debug2D::_flush_commands() {
	int snapshot_idx = 0;

	for (int i = 0; i < commands.size(); ++i) {
		const DrawCommand &c = commands[i];
		switch (c.type) {
			case DrawCommand::POLYLINE: {
				const PoolVector2Array &polyline = c.args[0];
				const Color &color = c.args[1];
				const real_t width = c.args[2];
				for (int i = 0; i < polyline.size() - 1; ++i) {
					canvas_item->draw_line(polyline[i], polyline[i + 1], color, width, true);
				}
			} break;
			case DrawCommand::COMMIT: {
				++snapshot_idx;
			} break;
		}
		if (snapshot >= 0 && snapshot_idx > snapshot) {
			// Stop drawing at this point.
			break;
		}
	}
}

void Debug2D::_on_canvas_item_draw() {
	_flush_commands();
}

void Debug2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_canvas_item_draw"), &Debug2D::_on_canvas_item_draw);

	ClassDB::bind_method(D_METHOD("draw_polyline", "polyline", "color", "width"), &Debug2D::draw_polyline, DEFVAL(1.0));

	ClassDB::bind_method(D_METHOD("set_snapshot", "snapshot"), &Debug2D::set_snapshot);
	ClassDB::bind_method(D_METHOD("get_snapshot"), &Debug2D::get_snapshot);

	ClassDB::bind_method(D_METHOD("commit"), &Debug2D::commit);
	ClassDB::bind_method(D_METHOD("update"), &Debug2D::update);
	ClassDB::bind_method(D_METHOD("clear"), &Debug2D::clear);
	
	ADD_PROPERTY(PropertyInfo(Variant::INT, "snapshot"), "set_snapshot", "get_snapshot");
}
