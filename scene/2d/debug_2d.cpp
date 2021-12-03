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
	_push_command(c);
}

void Debug2D::draw_clear() {
	DrawCommand c;
	c.type = DrawCommand::CLEAR;
	_push_command(c);
}

void Debug2D::_push_command(const DrawCommand &p_command) {
	commands.push_back(p_command);
	capture_end += 1;
}

void Debug2D::_draw_command(const DrawCommand &p_command) {
	switch (p_command.type) {
		case DrawCommand::POLYLINE: {
			const PoolVector2Array &polyline = p_command.args[0];
			for (int i = 0; i < polyline.size() - 1; ++i) {
				canvas_item->draw_line(polyline[i], polyline[i + 1], p_command.args[1], p_command.args[2], true);
			}
		} break;
		case DrawCommand::CLEAR: {
			// This clears all internal draw commands in CanvasItem itself.
			VisualServer::get_singleton()->canvas_item_clear(canvas_item->get_canvas_item());
		} break;
	}
}

void Debug2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			canvas_item = memnew(Node2D);
			canvas_item->connect(SceneStringNames::get_singleton()->draw, this, "_on_canvas_item_draw");
			canvas_item->set_name("Canvas");
			add_child(canvas_item);
		} break;
	}
}

void Debug2D::capture() {
	state->snapshots.push_back(capture_begin);
	state->snapshots.push_back(capture_end);

	capture_begin = capture_end;
	capture_end = commands.size();
}

void Debug2D::update() {
	canvas_item->update();
}

void Debug2D::clear() {
	commands.clear();
	state->snapshots.clear();
	capture_begin = 0;
	capture_end = 0;
	update();
}

void Debug2D::_on_canvas_item_draw() {
	int snapshot_idx = 0;
	int begin = capture_begin;
	int end = capture_end;

	for (int i = 0; i < state->snapshots.size(); i += 2) {
		begin = state->snapshots[i];
		end = state->snapshots[i + 1];

		for (int j = begin; j < end; ++j) {
			_draw_command(commands[j]);
		}
		++snapshot_idx;
		if (state->snapshot >= 0 && snapshot_idx > state->snapshot) {
			// Stop drawing at this point.
			break;
		}
	}
	// Process rest of the commands that were not explicitly captured.
	// These type of commands will be drawn regardless.
	if (state->snapshots.empty()) {
		for (int j = 0; j < commands.size(); ++j) {
			_draw_command(commands[j]);
		}
	} else {
		begin = state->snapshots[state->snapshots.size() - 1];
		end = commands.size();
		for (int j = begin; j < end; ++j) {
			_draw_command(commands[j]);
		}
	}
}

void Debug2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_canvas_item_draw"), &Debug2D::_on_canvas_item_draw);

	ClassDB::bind_method(D_METHOD("draw_polyline", "polyline", "color", "width"), &Debug2D::draw_polyline, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("draw_clear"), &Debug2D::draw_clear);

	ClassDB::bind_method(D_METHOD("get_capture"), &Debug2D::get_capture);
	ClassDB::bind_method(D_METHOD("capture"), &Debug2D::capture);

	ClassDB::bind_method(D_METHOD("update"), &Debug2D::update);
	ClassDB::bind_method(D_METHOD("clear"), &Debug2D::clear);
}

// DebugCapture

void DebugCapture::draw(int p_index) {
	ERR_FAIL_INDEX(p_index, get_count());
	snapshot = p_index;
	Debug2D::get_singleton()->update();
}

void DebugCapture::draw_next() {
	snapshot = CLAMP(snapshot + 1, 0, get_count() - 1);
	Debug2D::get_singleton()->update();
}

void DebugCapture::draw_prev() {
	snapshot = CLAMP(snapshot - 1, 0, get_count() - 1);
	Debug2D::get_singleton()->update();
}

void DebugCapture::reset() {
	snapshot = -1;
	Debug2D::get_singleton()->update();
}

void DebugCapture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("draw", "index"), &DebugCapture::draw);
	ClassDB::bind_method(D_METHOD("draw_next"), &DebugCapture::draw_next);
	ClassDB::bind_method(D_METHOD("draw_prev"), &DebugCapture::draw_prev);

	ClassDB::bind_method(D_METHOD("get_count"), &DebugCapture::get_count);

	ClassDB::bind_method(D_METHOD("reset"), &DebugCapture::reset);
}
