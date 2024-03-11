#pragma once

#include "axRenderMesh.h"
#include "axEditableMesh.h"
#include <ax_render/font/axFont.h>

class axRenderMeshEdit : public axNonCopyable {
public:
	using AttrId			= axRenderVertexAttrId;
	using Mesh				= axRenderMesh;
	using SubMesh			= axRenderSubMesh;
	using EditableMesh		= axEditableMesh;

	using PrimType			= axRenderPrimitiveType;
	using VertexDesc		= axRenderVertexDesc;
	using IndexType			= axRenderIndexType;
	using VertexArray		= axRenderVertexArray;

	void createRect				(VertexDesc vertexDesc, const axRect2f& rect, const axRect2f& uv, const axColor& color);
	void createBorderRect		(VertexDesc vertexDesc, const axRect2f& rect, const axMargin2f& border, const axRect2f& uv, const axMargin2f& uvBorder, const axColor& color, bool hasCenter);

	void addRect				(VertexDesc vertexDesc, const axRect2f& rect, const axRect2f& uv, const axColor& color);
	void addBorderRect			(VertexDesc vertexDesc, const axRect2f& rect, const axMargin2f& border, const axRect2f& uv, const axMargin2f& uvBorder, const axColor& color, bool hasCenter);

	void createCube				(VertexDesc vertexDesc, const axVec3f& pos, const axVec3f& size, const axColor& color);

	void createText				(VertexDesc vertexDesc, axStrView text, const axVec2f& pos, const axFontStyle* style);
	void addText				(VertexDesc vertexDesc, axStrView text, const axVec2f& pos, const axFontStyle* style);

	void createTextBillboard	(VertexDesc vertexDesc, axStrView text, const axVec3f& pos, const axFontStyle* style);
	void addTextBillboard		(VertexDesc vertexDesc, axStrView text, const axVec3f& pos, const axFontStyle* style);

	void createGrid				(VertexDesc vertexDesc,
								 float cellSize, axInt cellCount,
								 axColor centerLineColor  = axColor(1.0,  1.0f, 1.0f),
								 axColor gridLineColor    = axColor(0.4f, 0.4f, 0.4f),
								 axColor gridLine2_Color  = axColor(.65f, .65f, .65f),
								 axInt   gridLine2_Interval = 5); 

	void createLines			(VertexDesc vertexDesc, axSpan<axVec3f> positions, const axColor& color);
	void createLines			(VertexDesc vertexDesc, axSpan<axVec2f> positions, const axColor& color);

	void createLineStrip		(VertexDesc vertexDesc, axSpan<axVec3f> positions, const axColor& color);
	void createLineStrip		(VertexDesc vertexDesc, axSpan<axVec2f> positions, const axColor& color);

	template<class SRC>
	void createLinesFromVertexNormals(VertexDesc vertexDesc, const SRC& src, float normalLength, const axColorPair& color) {
		_mesh.clear();
		addLinesFromVertexNormals(vertexDesc, src, normalLength, color);
	}

	void addLinesFromVertexNormals			(VertexDesc vertexDesc, const Mesh&        srcMesh,     float normalLength, const axColorPair& color);
	void addLinesFromVertexNormals			(VertexDesc vertexDesc, const SubMesh&     srcSubMesh,  float normalLength, const axColorPair& color);
	void addLinesFromVertexNormals			(VertexDesc vertexDesc, const VertexArray& vertexArray, float normalLength, const axColorPair& color);

	void createFromEditableMesh				(VertexDesc vertexDesc, EditableMesh& srcMesh);
	void addFromEditableMesh				(VertexDesc vertexDesc, EditableMesh& srcMesh);

	void createLinesFromEdges				(VertexDesc vertexDesc, EditableMesh& srcMesh, const axColor& color);
	void createLinesFromFaceEdges			(VertexDesc vertexDesc, EditableMesh& srcMesh, const axColor& color);

	void createLinesFromVertexNormals		(VertexDesc vertexDesc, EditableMesh& srcMesh, float normalLength, const axColorPair& color);
	void createLinesFromEdgeNormals			(VertexDesc vertexDesc, EditableMesh& srcMesh, float normalLength, const axColorPair& color);
	void createLinesFromFaceNormals			(VertexDesc vertexDesc, EditableMesh& srcMesh, float normalLength, const axColorPair& color);
	void createLinesFromFaceVertexNormals	(VertexDesc vertexDesc, EditableMesh& srcMesh, float normalLength, const axColorPair& color);

	void createSphere						(VertexDesc vertexDesc, float radius, axInt rows, axInt columns);
	void createCylinder						(VertexDesc vertexDesc, float height, float radius, axInt rows, axInt columns, bool topCap, bool bottomCap);
	void createCone							(VertexDesc vertexDesc, float height, float radius, axInt rows, axInt columns, bool bottomCap);

	void createTextFromPointIds				(VertexDesc vertexDesc, EditableMesh& srcMesh, const axFontStyle* fontStyle);
	void createTextFromFaceIds				(VertexDesc vertexDesc, EditableMesh& srcMesh, const axFontStyle* fontStyle);
	void createTextFromEdgeIds				(VertexDesc vertexDesc, EditableMesh& srcMesh, const axFontStyle* fontStyle);
	void createTextFromFaceEdgeIds			(VertexDesc vertexDesc, EditableMesh& srcMesh, const axFontStyle* fontStyle);

	void setColor(const axColor color, axInt colorSet = 0);

friend class axRenderMesh;
protected:
	axRenderMeshEdit(Mesh& mesh) : _mesh(mesh) {}

private:
	Mesh&	_mesh;
};

inline  axRenderMeshEdit axRenderMesh::edit() { return axRenderMeshEdit(*this); }
