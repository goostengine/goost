#pragma once

#include "../poly_offset.h"
#include "thirdparty/misc/clipper.hpp"

class PolyOffset2DClipper6 : public PolyOffset2DBackend {
public:
	virtual Vector<Vector<Point2>> offset_polypaths(const Vector<Vector<Point2>> &p_polypaths, real_t p_delta);

private:
	ClipperLib::ClipperOffset configure(const Ref<PolyOffsetParameters2D> &p_parameters);
	ClipperLib::JoinType join_type;
	ClipperLib::EndType end_type;
};

