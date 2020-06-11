#include "goost/core/math/2d/geometry/goost_geometry_2d.h"
#include "poly_decomp.h"

Vector<Vector<Point2> > PolyDecomp2D::decompose_polygons(DecompType p_type, const Vector<Vector<Point2> > &p_polygons) {
    Vector<Vector<Point2> > polys;

    switch (p_type) {
		case DECOMP_TRIANGLES_EC: {
            polys = triangulate_ec(p_polygons);
        } break;
		case DECOMP_TRIANGLES_OPT: {
            polys = triangulate_opt(p_polygons);
        } break;
		case DECOMP_TRIANGLES_MONO: {
            polys = triangulate_mono(p_polygons);
        } break;
		case DECOMP_CONVEX_HM: {
            polys = decompose_convex_hm(p_polygons);
        } break;
		case DECOMP_CONVEX_OPT: {
            polys = decompose_convex_opt(p_polygons);
        } break;
    }
    return polys;
}

void PolyDecompParameters2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_fill_rule", "fill_rule"), &PolyDecompParameters2D::set_fill_rule);
    ClassDB::bind_method(D_METHOD("get_fill_rule"), &PolyDecompParameters2D::get_fill_rule);
    
    BIND_ENUM_CONSTANT(FILL_RULE_EVEN_ODD);
    BIND_ENUM_CONSTANT(FILL_RULE_NON_ZERO);
    BIND_ENUM_CONSTANT(FILL_RULE_POSITIVE);
    BIND_ENUM_CONSTANT(FILL_RULE_NEGATIVE);
    
    ADD_PROPERTY(PropertyInfo(Variant::INT, "fill_rule"), "set_fill_rule", "get_fill_rule");
}
