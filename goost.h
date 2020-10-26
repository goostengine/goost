#ifndef GOOST_H
#define GOOST_H

#include "core/object.h"

class GoostEngine : public Object {
	GDCLASS(GoostEngine, Object);

private:
	static GoostEngine *singleton;

protected:
	static void _bind_methods();

public:
	static GoostEngine *get_singleton() { return singleton; }

	Dictionary get_color_constants() const;

	GoostEngine() { singleton = this; }
};

#endif // GOOST_H
