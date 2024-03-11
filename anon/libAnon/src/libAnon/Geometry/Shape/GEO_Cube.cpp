#include "GEO_Cube.h"

namespace Anon {

void GEO_Cube::onCompute() {
	Base::onCompute();

	auto& geo = _output;

	geo.clear();

	auto pos = geo.editPoints(8);
	auto h = size / 2.0;
	auto c = center;

	pos[0].pos.set(c.x - h.x, c.y + h.y, c.z + h.z);
	pos[1].pos.set(c.x + h.x, c.y + h.y, c.z + h.z);
	pos[2].pos.set(c.x - h.x, c.y - h.y, c.z + h.z);
	pos[3].pos.set(c.x + h.x, c.y - h.y, c.z + h.z);

	pos[4].pos.set(c.x - h.x, c.y + h.y, c.z - h.z);
	pos[5].pos.set(c.x + h.x, c.y + h.y, c.z - h.z);
	pos[6].pos.set(c.x - h.x, c.y - h.y, c.z - h.z);
	pos[7].pos.set(c.x + h.x, c.y - h.y, c.z - h.z);

//     4-------5
//    /|      /|
//   0-------1 |
//   | 6-----|-7
//   |/      |/
//   2-------3

	axInt poly[6][4] = {{1,0,2,3},
						{5,4,0,1},
						{5,1,3,7},
						{0,4,6,2},
						{4,5,7,6},
						{6,7,3,2}};


	geo.addPrimitive(GEO_PrimitiveType::Polygon, poly[0]);
	geo.addPrimitive(GEO_PrimitiveType::Polygon, poly[1]);
	geo.addPrimitive(GEO_PrimitiveType::Polygon, poly[2]);
	geo.addPrimitive(GEO_PrimitiveType::Polygon, poly[3]);
	geo.addPrimitive(GEO_PrimitiveType::Polygon, poly[4]);
	geo.addPrimitive(GEO_PrimitiveType::Polygon, poly[5]);

	auto primNormals = geo.editPrimitiveNormals();
	primNormals[0].set( 0, 0, 1);
	primNormals[1].set( 0, 1, 0);
	primNormals[2].set( 1, 0, 0);
	primNormals[3].set(-1, 0, 0);
	primNormals[4].set( 0, 0,-1);
	primNormals[5].set( 0,-1, 0);
}

} //namespace