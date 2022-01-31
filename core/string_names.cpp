#include "string_names.h"

StringNames *StringNames::singleton = nullptr;

StringNames::StringNames() :
		_create_vertex(StaticCString::create("_create_vertex")),
		_create_edge(StaticCString::create("_create_edge"))
{}
