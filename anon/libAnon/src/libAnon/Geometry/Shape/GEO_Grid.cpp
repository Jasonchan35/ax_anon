#include "GEO_Grid.h"

namespace Anon {

void GEO_Grid::onCompute() {
	Base::onCompute();

	_output.clear();

	if (_div.x <= 0 || _div.y <= 0) return;

	auto div = ax_min(_div, Vec2i(128)); // limit to 128

	axInt pointCount = (div.x+1) * (div.y+1);
	auto points = _output.editPoints(pointCount);
	auto size3 = ax_max(_size.x0y(), Vec3d(0));
	auto offset = _center - size3 / 2;
	auto cellSize = size3 / Vec3d::s_cast(div.x1y());

	{
		auto pt = ax_foreach_begin(points);
		for (axInt y = 0; y <= div.y; y++) {
			for (axInt x = 0; x <= div.x; x++) {
				pt->pos = offset + Vec3d::s_cast(Vec3i(x,0,y)) * cellSize;
				pt++;
			}
		}
	}

	auto rowStride = div.x + 1;

	axInt poly[4];
	for (axInt y = 0; y < div.y; y++) {
		for (axInt x = 0; x < div.x; x++) {
			poly[0] =    y    * rowStride + x + 1;
			poly[1] =    y    * rowStride + x;
			poly[2] = (y + 1) * rowStride + x;
			poly[3] = (y + 1) * rowStride + x + 1;
			_output.addPrimitive(GEO_PrimitiveType::Polygon, poly);
		}
	}

	//_output.editPrimitiveNormals().fillValues(Vec3d(0, 1, 0));
	//_output.editVertexNormals().fillValues(Vec3d(0, 1, 0));
	_output.editPointNormals().fillValues(Vec3d(0, 1, 0));
}

} //namespace