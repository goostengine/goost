#ifndef GOOST_CLIPPER6_PATH_CONVERT_H
#define GOOST_CLIPPER6_PATH_CONVERT_H

#include "core/vector.h"
#include "core/math/vector2.h"
#include "thirdparty/misc/clipper.hpp"

// Note: we provide a complete type for LocalMinimum as Clipper 6.4.2 only
// forward-declares the struct, which leads to the compilation errors.
namespace ClipperLib {
    struct LocalMinimum {
    cInt          Y;
    TEdge        *LeftBound;
    TEdge        *RightBound;
    };
}

namespace GodotClipperUtils {

using namespace ClipperLib;

void scale_up_polypaths(const Vector<Vector<Point2> > &p_polypaths_in, Paths &p_polypaths_out);
void scale_down_polypaths(const Paths &p_polypaths_in, Vector<Vector<Point2> > &p_polypaths_out);
void scale_up_polypath(const Vector<Point2> &p_polypath_in, Path &p_polypath_out);
void scale_down_polypath(const Path &p_polypath_in, Vector<Point2> &p_polypath_out);

} // namespace GodotClipperUtils

#endif // GOOST_CLIPPER6_PATH_CONVERT_H
