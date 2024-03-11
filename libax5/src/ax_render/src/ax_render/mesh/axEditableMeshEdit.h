#pragma once

#include "axEditableMesh.h"

class axEditableMeshEdit {
public:
	using Mesh = axEditableMesh;

	void createSphere	(double radius, axInt rows, axInt columns);
	void createCylinder	(double height, double radius, axInt rows, axInt columns, bool topCap, bool bottomCap);
	void createCone		(double height, double radius, axInt rows, axInt columns, bool bottomCap);

friend class axEditableMesh;
protected:
	axEditableMeshEdit(Mesh& mesh) : _mesh(mesh) {}

private:
	Mesh& _mesh;
};

inline axEditableMeshEdit axEditableMesh::edit() { return axEditableMeshEdit(*this); }
