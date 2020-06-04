#include "poly_offset.h"

void PolyOffsetParameters2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_join_type", "join_type"), &PolyOffsetParameters2D::set_join_type);
    ClassDB::bind_method(D_METHOD("get_join_type"), &PolyOffsetParameters2D::get_join_type);
    ClassDB::bind_method(D_METHOD("set_end_type", "end_type"), &PolyOffsetParameters2D::set_end_type);
    ClassDB::bind_method(D_METHOD("get_end_type"), &PolyOffsetParameters2D::get_end_type);
    ClassDB::bind_method(D_METHOD("set_arc_tolerance", "arc_tolerance"), &PolyOffsetParameters2D::set_arc_tolerance);
    ClassDB::bind_method(D_METHOD("get_arc_tolerance"), &PolyOffsetParameters2D::get_arc_tolerance);
    ClassDB::bind_method(D_METHOD("set_miter_limit", "miter_limit"), &PolyOffsetParameters2D::set_miter_limit);
    ClassDB::bind_method(D_METHOD("get_miter_limit"), &PolyOffsetParameters2D::get_miter_limit);
    
    ADD_PROPERTY(PropertyInfo(Variant::INT, "join_type"), "set_join_type", "get_join_type");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "end_type"), "set_end_type", "get_end_type");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "arc_tolerance"), "set_arc_tolerance", "get_arc_tolerance");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "miter_limit"), "set_miter_limit", "get_miter_limit");

    BIND_ENUM_CONSTANT(JOIN_SQUARE);
    BIND_ENUM_CONSTANT(JOIN_ROUND);
    BIND_ENUM_CONSTANT(JOIN_MITER);
    BIND_ENUM_CONSTANT(END_POLYGON);
    BIND_ENUM_CONSTANT(END_JOINED);
    BIND_ENUM_CONSTANT(END_BUTT);
    BIND_ENUM_CONSTANT(END_SQUARE);
    BIND_ENUM_CONSTANT(END_ROUND);
}
