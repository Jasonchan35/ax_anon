#include "axEditableMeshEdit.h"

void axEditableMeshEdit::createSphere(double radius, axInt rows, axInt columns) {
	_mesh.clear();

	for (axInt y = 1; y < rows; y++) {
		double sinY, cosY;
		ax_sincos(ax_pi * (double)y/rows, sinY, cosY);
		for (axInt x = 0; x < columns; x++) {
			double sinX, cosX;
			ax_sincos(2 * ax_pi * (double)x/columns, sinX, cosX);
			_mesh.addPoint(axVec3d(sinX * sinY, cosY, cosX * sinY) * radius);
		}
	}

	//top
	{
		auto s = _mesh.points().size();
		_mesh.addPoint(0, radius, 0);
		for (axInt x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			axInt vi[] = {s, x, x1};
			_mesh.addFace(vi);
		}
	}

	for (axInt y = 0; y < rows - 2; y++) {
		for (axInt x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			auto y1 = y + 1;
			axInt vi[4] = {
				y  * columns + x,
				y1 * columns + x,
				y1 * columns + x1,
				y  * columns + x1,
			};
			_mesh.addFace(vi);
		}
	}

	//bottom
	{
		auto center = _mesh.points().size();
		_mesh.addPoint(0, -radius, 0);
		for (axInt x = 0; x < columns; x++) {
			auto y  = rows - 2;
			auto x1 = (x + 1) % columns;

			axInt vi[] = {
				y * columns + x1, 
				y * columns + x, 
				center
			};
			_mesh.addFace(vi);
		}
	}
}

void axEditableMeshEdit::createCylinder(double height, double radius, axInt rows, axInt columns, bool topCap, bool bottomCap) {
	_mesh.clear();

	auto heightHalf = height * 0.5;
	auto rows1 = rows + 1;

	for (axInt y = 0; y < rows1; y++) {
		double dy = (double)y/rows;
		double sinY, cosY;
		ax_sincos(ax_pi * dy, sinY, cosY);
		for (axInt x = 0; x < columns; x++) {
			double sinX, cosX;
			double dx = (double)x/columns;
			ax_sincos(2 * ax_pi * dx, sinX, cosX);
			_mesh.addPoint(axVec3d(sinX * radius, dy * height - heightHalf, cosX * radius));
		}
	}

	for (axInt y = 0; y < rows; y++) {
		for (axInt x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			auto y1 = y + 1;
			axInt vi[4] = {
				y  * columns + x,
				y1 * columns + x,
				y1 * columns + x1,
				y  * columns + x1,
			};
			_mesh.addFace(vi);
		}
	}

	if (topCap) {
		auto s = _mesh.points().size();
		_mesh.addPoint(0, heightHalf, 0);
		for (axInt x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			axInt vi[] = {s, x, x1};
			_mesh.addFace(vi);
		}
	}

	if (bottomCap) {
		auto center = _mesh.points().size();
		_mesh.addPoint(0, -heightHalf, 0);

		for (axInt x = 0; x < columns; x++) {
			auto y  = rows;
			auto x1 = (x + 1) % columns;

			axInt vi[] = {
				y * columns + x1, 
				y * columns + x, 
				center
			};
			_mesh.addFace(vi);
		}
	}
}

void axEditableMeshEdit::createCone(double height, double radius, axInt rows, axInt columns, bool bottomCap/*=true*/) {
	_mesh.clear();

	auto heightHalf = height * 0.5;
	auto rows1 = rows + 1;

	for (axInt y = 1; y < rows1; y++) {
		double dy = (double)y/rows;
		double sinY, cosY;
		ax_sincos(ax_pi * dy, sinY, cosY);
		for (axInt x = 0; x < columns; x++) {
			double sinX, cosX;
			double dx = (double)x/columns;
			ax_sincos(2 * ax_pi * dx, sinX, cosX);
			_mesh.addPoint(axVec3d(sinX * radius * dy, dy * -height + heightHalf, cosX * radius * dy));
		}
	}

	for (axInt y = 0; y < rows-1; y++) {
		for (axInt x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			auto y1 = y + 1;
			axInt vi[4] = {
				y  * columns + x,
				y1 * columns + x,
				y1 * columns + x1,
				y  * columns + x1,
			};
			_mesh.addFace(vi);
		}
	}

	//top
	{
		auto s = _mesh.points().size();
		_mesh.addPoint(0, heightHalf, 0);
		for (axInt x = 0; x < columns; x++) {
			auto x1 = (x + 1) % columns;
			axInt vi[] = {s, x, x1};
			_mesh.addFace(vi);
		}
	}

	if (bottomCap) {
		auto center = _mesh.points().size();
		_mesh.addPoint(0, -heightHalf, 0);

		for (axInt x = 0; x < columns; x++) {
			auto y  = rows - 1;
			auto x1 = (x + 1) % columns;

			axInt vi[] = {
				y * columns + x1, 
				y * columns + x, 
				center
			};
			_mesh.addFace(vi);
		}
	}
}

