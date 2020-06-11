#ifndef GOOST_CLIPPER10_PATH_CONVERT_H
#define GOOST_CLIPPER10_PATH_CONVERT_H

#include "core/math/vector2.h"
#include "core/vector.h"
#include "goost/thirdparty/clipper/clipper.h"

namespace GodotClipperUtils {

using namespace clipperlib;

void scale_up_polypaths(const Vector<Vector<Point2>> &p_polypaths_in, Paths &p_polypaths_out);
void scale_down_polypaths(const Paths &p_polypaths_in, Vector<Vector<Point2>> &p_polypaths_out);
void scale_up_polypath(const Vector<Point2> &p_polypath_in, Path &p_polypath_out);
void scale_down_polypath(const Path &p_polypath_in, Vector<Point2> &p_polypath_out);

} // namespace GodotClipperUtils

#endif // GOOST_CLIPPER10_PATH_CONVERT_H
