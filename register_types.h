#ifndef GOOST_REGISTER_TYPES
#define GOOST_REGISTER_TYPES

#include "core/engine.h"

// Do not include `goost.h` here.
//
// This may lead to clashes with Godot's namespace, or produce include errors
// when `module_goost_enabled=no` is specified via command-line.

void register_goost_types();
void unregister_goost_types();

#endif // GOOST_REGISTER_TYPES
