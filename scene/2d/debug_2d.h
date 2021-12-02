#pragma once

#include "core/engine.h"
#include "scene/2d/node_2d.h"

class Debug2D : public Node {
	GDCLASS(Debug2D, Node);

private:
	static Debug2D *singleton;

	Node2D *canvas_item = nullptr;

	struct DrawCommand {
		enum Type {
			POLYLINE,
			COMMIT,
		};
		Type type;
		Vector<Variant> args;
	};
	Vector<DrawCommand> commands;

	int snapshot = -1;

protected:
	void _notification(int p_what);
	static void _bind_methods();

	void _on_canvas_item_draw();
	void _flush_commands();

public:
	static Debug2D *get_singleton() { return singleton; }

	void draw_polyline(const Vector<Point2> &p_polyline, const Color &p_color, real_t p_width = 1.0);
	
	void set_snapshot(int p_snapshot);
	int get_snapshot() const { return snapshot; }

	void commit();
	void update();
	void clear();

	Debug2D() {
		ERR_FAIL_COND_MSG(singleton != nullptr, "Singleton already exists");
		singleton = this;
	}
};
