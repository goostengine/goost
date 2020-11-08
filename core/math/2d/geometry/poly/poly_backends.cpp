#include "poly_backends.h"

PolyBackend2DManager<PolyBoolean2DBackend *> PolyBackends2D::poly_boolean = PolyBackend2DManager<PolyBoolean2DBackend *>();
PolyBackend2DManager<PolyOffset2DBackend *> PolyBackends2D::poly_offset = PolyBackend2DManager<PolyOffset2DBackend *>();
PolyBackend2DManager<PolyDecomp2DBackend *> PolyBackends2D::poly_decomp = PolyBackend2DManager<PolyDecomp2DBackend *>();
