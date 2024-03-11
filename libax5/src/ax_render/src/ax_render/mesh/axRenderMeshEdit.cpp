#include "axRenderMeshEdit.h"
#include "axEditableMeshEdit.h"
#include "../axRenderer.h"
#include "../axRenderContext.h"

void axRenderMeshEdit::addRect(VertexDesc vertexDesc, const axRect2f& rect, const axRect2f& uv, const axColor& color_) {
// 0 ----- 1				   VertexDesc vertexDesc, 
// |       |
// 2 ----- 3
	using Index = axUInt16;
	auto indexType = axRenderIndexTypeOf<Index>();
	auto primType  =  PrimType::Triangles;

	auto  added = _mesh.addVertices(primType, vertexDesc, indexType, 4);

	if (added.hasPositions()) {
		auto dst = ax_foreach_begin(added.positions());
		dst->set(rect.xMin(), rect.yMin(), 0); dst++;
		dst->set(rect.xMax(), rect.yMin(), 0); dst++;
		dst->set(rect.xMin(), rect.yMax(), 0); dst++;
		dst->set(rect.xMax(), rect.yMax(), 0); dst++;
	}

	if (added.hasUvSet(0)) {
		auto dst = ax_foreach_begin(added.uvSet(0));
		dst->set(uv.xMin(), uv.yMin()); dst++;
		dst->set(uv.xMax(), uv.yMin()); dst++;
		dst->set(uv.xMin(), uv.yMax()); dst++;
		dst->set(uv.xMax(), uv.yMax()); dst++;
	}

	if (added.hasColorSet(0)) {
		auto color = axColorRGBAh_make(color_.toPremultipliedAlpha());
		added.colorSet(0).fillValues(color);
	}

	auto  baseIdx = static_cast<Index>(added.range.start);
	Index indices[] = {
		static_cast<Index>(baseIdx + 0),
		static_cast<Index>(baseIdx + 1),
		static_cast<Index>(baseIdx + 3),
		static_cast<Index>(baseIdx + 0),
		static_cast<Index>(baseIdx + 3),
		static_cast<Index>(baseIdx + 2)
	};

	added.indices().append(axSpan<Index>(indices));
}

void axRenderMeshEdit::createRect(VertexDesc vertexDesc, const axRect2f& rect, const axRect2f& uv, const axColor& color) {
	_mesh.clear();
	addRect(vertexDesc, rect, uv, color);
}

void axRenderMeshEdit::createBorderRect(VertexDesc vertexDesc, const axRect2f& rect, const axMargin2f& border, const axRect2f& uv, const axMargin2f& uvBorder, const axColor& color, bool hasCenter) {
	_mesh.clear();
	addBorderRect(vertexDesc, rect, border, uv, uvBorder, color, hasCenter);
}

void axRenderMeshEdit::addBorderRect(VertexDesc vertexDesc, const axRect2f& rect, const axMargin2f& border, const axRect2f& uv, const axMargin2f& uvBorder, const axColor& color_, bool hasCenter) {
//  0 --- 1 ----- 2 --- 3
//  |  \  |       |  /  |
//  4 --- 5 ----- 6 --- 7
//  |     |       |     |
//  8 --- 9 -----10 ---11
//  |  /  |       |  \  |
// 12 ---13 -----14 ---15

	using Index = axUInt16;
	auto indexType = axRenderIndexTypeOf<Index>();
	auto primType  =  PrimType::Triangles;

	auto  added = _mesh.addVertices(primType, vertexDesc, indexType, 16);

	auto outerRect	= rect;
	auto innerRect	= rect - border;
	auto outerUv	= uv;
	auto innerUv	= uv - uvBorder;

	if (added.hasPositions()) {
		auto dst = ax_foreach_begin(added.positions());
		// top row
		dst->set(outerRect.xMin(), outerRect.yMin(), 0); dst++;
		dst->set(innerRect.xMin(), outerRect.yMin(), 0); dst++;
		dst->set(innerRect.xMax(), outerRect.yMin(), 0); dst++;
		dst->set(outerRect.xMax(), outerRect.yMin(), 0); dst++;
		// 2nd row
		dst->set(outerRect.xMin(), innerRect.yMin(), 0); dst++;
		dst->set(innerRect.xMin(), innerRect.yMin(), 0); dst++;
		dst->set(innerRect.xMax(), innerRect.yMin(), 0); dst++;
		dst->set(outerRect.xMax(), innerRect.yMin(), 0); dst++;
		// 3rd row
		dst->set(outerRect.xMin(), innerRect.yMax(), 0); dst++;
		dst->set(innerRect.xMin(), innerRect.yMax(), 0); dst++;
		dst->set(innerRect.xMax(), innerRect.yMax(), 0); dst++;
		dst->set(outerRect.xMax(), innerRect.yMax(), 0); dst++;
		// bottom row
		dst->set(outerRect.xMin(), outerRect.yMax(), 0); dst++;
		dst->set(innerRect.xMin(), outerRect.yMax(), 0); dst++;
		dst->set(innerRect.xMax(), outerRect.yMax(), 0); dst++;
		dst->set(outerRect.xMax(), outerRect.yMax(), 0); dst++;
	}

	if (added.hasUvSet(0)) {
		auto dst = ax_foreach_begin(added.uvSet(0));
		// top row
		dst->set(outerUv.xMin(), outerUv.yMin()); dst++;
		dst->set(innerUv.xMin(), outerUv.yMin()); dst++;
		dst->set(innerUv.xMax(), outerUv.yMin()); dst++;
		dst->set(outerUv.xMax(), outerUv.yMin()); dst++;
		// 2nd row
		dst->set(outerUv.xMin(), innerUv.yMin()); dst++;
		dst->set(innerUv.xMin(), innerUv.yMin()); dst++;
		dst->set(innerUv.xMax(), innerUv.yMin()); dst++;
		dst->set(outerUv.xMax(), innerUv.yMin()); dst++;
		// 3rd row
		dst->set(outerUv.xMin(), innerUv.yMax()); dst++;
		dst->set(innerUv.xMin(), innerUv.yMax()); dst++;
		dst->set(innerUv.xMax(), innerUv.yMax()); dst++;
		dst->set(outerUv.xMax(), innerUv.yMax()); dst++;
		// bottom row
		dst->set(outerUv.xMin(), outerUv.yMax()); dst++;
		dst->set(innerUv.xMin(), outerUv.yMax()); dst++;
		dst->set(innerUv.xMax(), outerUv.yMax()); dst++;
		dst->set(outerUv.xMax(), outerUv.yMax()); dst++;
	}

	auto color = axColorRGBAh_make(color_);

	if (added.hasColorSet(0)) {
		auto dst = ax_foreach_begin(added.colorSet(0));
		// top row
		*dst = color; dst++;
		*dst = color; dst++;
		*dst = color; dst++;
		*dst = color; dst++;
		// 2nd row
		*dst = color; dst++;
		*dst = color; dst++;
		*dst = color; dst++;
		*dst = color; dst++;
		// 3rd row
		*dst = color; dst++;
		*dst = color; dst++;
		*dst = color; dst++;
		*dst = color; dst++;
		// bottom row
		*dst = color; dst++;
		*dst = color; dst++;
		*dst = color; dst++;
		*dst = color; dst++;
	}

	Index indicesBuf[] = {
		4,0,5,5,0,1,      1,2,5,5,2,6,        2,3,6,6,3,7,
		4,5,8,8,5,9,                          6,7,10,10,7,11,
		8,9,12,12,9,13,   9,10,13,13,10,14,   14,10,15,15,10,11,
		5,6,9,9,6,10 //center
	};

	auto  baseIdx = static_cast<Index>(added.range.start);
	auto indices = axMutSpan_make(indicesBuf);
	for (auto& v : indices) {
		v += baseIdx;
	}	
	
	indices = hasCenter ? indices : indices.slice(0, 8 * 6);
	added.indices().append(indices.constSpan());
}

void axRenderMeshEdit::createCube(VertexDesc vertexDesc, const axVec3f& pos, const axVec3f& size, const axColor& color_) {
//     4-------5
//    /|      /|
//   0-------1 |
//   | 6-----|-7
//   |/      |/
//   2-------3
	
	_mesh.clear();

	using Index = axUInt16;
	auto added = _mesh.addVertices(PrimType::Triangles, vertexDesc, axRenderIndexTypeOf<Index>(), 8);

	auto s = size * 0.5f;

	if (added.hasPositions()) {
		auto dst = ax_foreach_begin(added.positions());
		{
			auto t = pos;
			t.z -= s.z;
			dst->set(t.x - s.x, t.y - s.y, t.z); dst++;
			dst->set(t.x + s.x, t.y - s.y, t.z); dst++;
			dst->set(t.x - s.x, t.y + s.y, t.z); dst++;
			dst->set(t.x + s.x, t.y + s.y, t.z); dst++;
		}

		{
			auto t = pos;
			t.z += s.z;
			dst->set(t.x - s.x, t.y - s.y, t.z); dst++;
			dst->set(t.x + s.x, t.y - s.y, t.z); dst++;
			dst->set(t.x - s.x, t.y + s.y, t.z); dst++;
			dst->set(t.x + s.x, t.y + s.y, t.z); dst++;
		}
	}

	if (added.hasColorSet(0)) {
		auto color = axColorRGBAh_make(color_.toPremultipliedAlpha());
		added.colorSet(0).fillValues(color);
	}

	Index indicesBuf[] = {
		0,1,3,0,3,2,
		1,5,7,1,7,3,
		4,5,1,4,1,0,
		5,4,6,5,6,7,
		4,0,2,4,2,6,
		2,3,7,2,7,6,
	};

	auto baseIdx = static_cast<Index>(added.range.start);
	auto indices = axMutSpan_make(indicesBuf);
	for (auto& v : indices) {
		v += baseIdx;
	}	
	
	added.indices().append(axSpan<Index>(indices));
}

void axRenderMeshEdit::createText(VertexDesc vertexDesc, axStrView text, const axVec2f& pos, const axFontStyle* style) {
	_mesh.clear();
	addText(vertexDesc, text, pos, style);
}

void axRenderMeshEdit::addText(VertexDesc vertexDesc, axStrView text, const axVec2f& pos, const axFontStyle* style) {
	addTextBillboard(vertexDesc, text, pos.xy0(), style);
}

void axRenderMeshEdit::createTextBillboard(VertexDesc vertexDesc, axStrView text, const axVec3f& pos, const axFontStyle* style) {
	_mesh.clear();
	addTextBillboard(vertexDesc, text, pos, style);
}

void axRenderMeshEdit::addTextBillboard(VertexDesc vertexDesc, axStrView text, const axVec3f& pos, const axFontStyle* style) {
	if (!style) {
		style = axRenderer::s_instance()->defaultFontStyle();
	}

	if (!style)			{ AX_ASSERT(false); return; }
	if (!style->font)	{ AX_ASSERT(false); return; }

	if (!vertexDesc->hasPositions()) { AX_ASSERT(false); return; }

	axArray<axFontGlyph, 128> glyphs;
	style->font->getGlyphs(glyphs, text);

	auto glyphCount  = glyphs.size();
	if (!glyphCount) return;

	auto vertexCount = glyphCount * 4;
	auto indexCount  = glyphCount * 6;

	using Index = axUInt16;
	auto primType  =  PrimType::Triangles;

	auto added = _mesh.addVertices(primType, vertexDesc, axRenderIndexTypeOf<Index>(), vertexCount);
	auto baseIdx = static_cast<Index>(added.range.start);

	auto textColor = axColorRGBAh_make(style->color);

// 0 ----- 1
// |       |
// 2 ----- 3

	auto dstPos    = ax_foreach_begin(added.positions());
	auto dstIndices = added.indices().add<Index>(indexCount);
	auto dstIndex = ax_foreach_begin(dstIndices);

	struct Pen {
		float maxHeightInThisLine = 0;
		axVec2f pos{0,0};
		void advance(axFontGlyph& g) {
			ax_max_it(maxHeightInThisLine, g.outerRect.h);
			pos.x += g.outerRect.w + g.kerning.x;
			if (g.charCode == '\n') {
				pos.y += ax_ceil(maxHeightInThisLine * 1.08f);
				maxHeightInThisLine = 0;
				pos.x = 0;
			}
		}
	};

	{
		Pen pen;
		for (auto& g : glyphs) {
			auto rc = g.vertexRect + pen.pos;

			*dstPos = pos + rc.xMinYMin().xy0(); dstPos++;
			*dstPos = pos + rc.xMaxYMin().xy0(); dstPos++;
			*dstPos = pos + rc.xMinYMax().xy0(); dstPos++;
			*dstPos = pos + rc.xMaxYMax().xy0(); dstPos++;

			*dstIndex = static_cast<Index>(baseIdx + 0); dstIndex++;
			*dstIndex = static_cast<Index>(baseIdx + 1); dstIndex++;
			*dstIndex = static_cast<Index>(baseIdx + 3); dstIndex++;
			*dstIndex = static_cast<Index>(baseIdx + 0); dstIndex++;
			*dstIndex = static_cast<Index>(baseIdx + 3); dstIndex++;
			*dstIndex = static_cast<Index>(baseIdx + 2); dstIndex++;

			pen.advance(g);
			baseIdx += 4;
		}
	}

	if (added.hasUvSet(0)) {
		auto dstUv0    = ax_foreach_begin(added.uvSet(0));
		for (auto& g : glyphs) {
			*dstUv0 = g.uv.xMinYMin(); dstUv0++;
			*dstUv0 = g.uv.xMaxYMin(); dstUv0++;
			*dstUv0 = g.uv.xMinYMax(); dstUv0++;
			*dstUv0 = g.uv.xMaxYMax(); dstUv0++;
		}
	}

	if (added.hasUvSet(1)) {
		auto dstUv1    = ax_foreach_begin(added.uvSet(1));
		Pen pen;
		for (auto& g : glyphs) {
			auto rc = g.vertexRect + pen.pos;
			*dstUv1 = rc.xMinYMin(); dstUv1++;
			*dstUv1 = rc.xMaxYMin(); dstUv1++;
			*dstUv1 = rc.xMinYMax(); dstUv1++;
			*dstUv1 = rc.xMaxYMax(); dstUv1++;

			pen.advance(g);
		}
	}

	if (added.hasColorSet(0)) {
		added.colorSet(0).fillValues(textColor);
	}
}

void axRenderMeshEdit::createGrid(VertexDesc vertexDesc, float cellSize, axInt cellCount, axColor gridLineColor_, axColor centerLineColor_, axColor gridLine2_Color_, axInt gridLine2_Interval) {
		_mesh.clear();

		auto gridLineColor   = axColorRGBAh_make(gridLineColor_);
		auto gridLine2_Color = axColorRGBAh_make(gridLine2_Color_);
		auto centerLineColor = axColorRGBAh_make(centerLineColor_);

		const axInt dx = cellCount / 2;
		const axInt dy = cellCount / 2;

		const axVec3f scale(cellSize * dx, cellSize * dy, 1);

		const axInt dx1 = dx + 1;
		const axInt dy1 = dy + 1;

		auto vertexCount = (dx + dy) * 4 + 4;

		_mesh.clear();
		_mesh.setSubMeshCount(1);

		auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, vertexCount);

		if (added.hasPositions() && added.hasColorSet(0) ) {
			auto pos = ax_foreach_begin(added.positions());
			auto col = ax_foreach_begin(added.colorSet(0));

			//center line Y
			*pos = axVec3f( 0,-1, 0) * scale; pos++;	*col = centerLineColor; col++;
			*pos = axVec3f( 0, 1, 0) * scale; pos++;	*col = centerLineColor; col++;

			//center line X
			*pos = axVec3f(-1, 0, 0) * scale; pos++;	*col = centerLineColor; col++;
			*pos = axVec3f( 1, 0, 0) * scale; pos++;	*col = centerLineColor; col++;

			for (axInt x = 1; x < dx1; x++) {
				float px = (float)x / dx;
				auto color = (gridLine2_Interval != 0 && x % gridLine2_Interval == 0) ? gridLine2_Color : gridLineColor;

				*pos = axVec3f( px, -1, 0) * scale; pos++;	*col = color; col++;
				*pos = axVec3f( px,  1, 0) * scale; pos++;	*col = color; col++;
				*pos = axVec3f(-px, -1, 0) * scale; pos++;	*col = color; col++;
				*pos = axVec3f(-px,  1, 0) * scale; pos++;	*col = color; col++;
			}

			for (axInt y = 1; y < dy1; y++) {
				float py = (float)y / dy;
				auto color = (gridLine2_Interval != 0 && y % gridLine2_Interval == 0) ? gridLine2_Color : gridLineColor;
				*pos = axVec3f(-1,  py, 0) * scale; pos++;	*col = color; col++;
				*pos = axVec3f( 1,  py, 0) * scale; pos++;	*col = color; col++;
				*pos = axVec3f(-1, -py, 0) * scale; pos++;	*col = color; col++;
				*pos = axVec3f( 1, -py, 0) * scale; pos++;	*col = color; col++;
			}
		}
}

void axRenderMeshEdit::createLines(VertexDesc vertexDesc, axSpan<axVec3f> positions, const axColor& color) {
	_mesh.clear();

	axInt n = positions.size();
	if (n < 2) return;

	AX_ASSERT(n % 2 == 0);

	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, n);

	if (added.hasPositions()) {
		auto pos = ax_foreach_begin(added.positions());
		for (auto& src : positions) {
			*pos = src;
			pos++;
		}
	}

	if (added.hasColorSet(0)) {
		added.colorSet(0).fillValues(axColorRGBAh_make(color));
	}
}

void axRenderMeshEdit::createLines(VertexDesc vertexDesc, axSpan<axVec2f> positions, const axColor& color) {
	_mesh.clear();

	axInt n = positions.size();
	if (n < 2) return;

	AX_ASSERT(n % 2 == 0);
	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, n);

	if (added.hasPositions()) {
		auto pos = ax_foreach_begin(added.positions());
		for (auto& src : positions) {
			*pos = src.xy0();
			pos++;
		}
	}

	if (added.hasColorSet(0)) {
		added.colorSet(0).fillValues(axColorRGBAh_make(color));
	}
}

void axRenderMeshEdit::createLineStrip(VertexDesc vertexDesc, axSpan<axVec3f> positions, const axColor& color) {
	_mesh.clear();

	axInt n = positions.size();
	if (n < 2) return;

	axInt segmentCount = n - 1;
	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, segmentCount * 2);

	if (added.hasPositions()) {
		auto pos = ax_foreach_begin(added.positions());

		for (axInt i = 0; i < segmentCount; i++) {
			*pos = positions[i];
			pos++;

			*pos = positions[i+1];
			pos++;
		}
	}

	if (added.hasColorSet(0)) {
		added.colorSet(0).fillValues(axColorRGBAh_make(color));
	}
}

void axRenderMeshEdit::createLineStrip(VertexDesc vertexDesc, axSpan<axVec2f> positions, const axColor& color) {
	_mesh.clear();

	axInt n = positions.size();
	if (n < 2) return;

	axInt segmentCount = n - 1;
	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, segmentCount * 2);

	if (added.hasPositions()) {
		auto pos = ax_foreach_begin(added.positions());
		for (axInt i = 0; i < segmentCount; i++) {
			*pos = positions[i].xy0();
			pos++;

			*pos = positions[i+1].xy0();
			pos++;
		}
	}

	if (added.hasColorSet(0)) {
		added.colorSet(0).fillValues(axColorRGBAh_make(color));
	}

}

void axRenderMeshEdit::createFromEditableMesh(VertexDesc vertexDesc, EditableMesh& srcMesh) {
	_mesh.clear();
	addFromEditableMesh(vertexDesc, srcMesh);
}

void axRenderMeshEdit::addFromEditableMesh(VertexDesc vertexDesc, EditableMesh& srcMesh) {
	axArray<EditableMesh::Point*, 64> faceVertices;

	using Index = axUInt16;
	auto indexType = axRenderIndexTypeOf<Index>();
	auto primType = PrimType::Triangles;

	//TODO polygon triangulate
	for (auto& face : srcMesh.faces()) {
		const auto fvCount = face.pointCount();
		auto added = _mesh.addVertices(primType, vertexDesc, indexType, fvCount);

		if (added.hasPositions()) {
			face.getPoints(srcMesh, faceVertices);

			auto dst = ax_foreach_begin(added.positions());
			for (auto& fv : faceVertices) {
				dst->setByCast(fv->pos);
				dst++;
			}
		}

		if (added.hasNormals()) {
			auto dst = ax_foreach_begin(added.normals());
			auto src = face.getNormals(srcMesh);
			for (auto& p : src) {
				dst->setByCast(p);
				dst++;
			}
		}

		// color set
		for (axInt i = 0; i < srcMesh.colorSetCount(); i++) {
			if (added.hasColorSet(i)) {
				auto dst = ax_foreach_begin(added.colorSet(i));
				auto src = face.getColors(srcMesh, i);
				for (auto& p : src) {
					*dst = axColorRGBAh_make(p);
					dst++;
				}
			}
		}

		// uv set
		for (axInt i = 0; i < srcMesh.uvSetCount(); i++) {
			if (added.hasUvSet(i)) {
				auto dst = ax_foreach_begin(added.uvSet(i));
				auto src = face.getUvs(srcMesh, i);
				for (auto& p : src) {
					dst->setByCast(p);
					dst++;
				}
			}
		}

		//add vertex indices
		if (fvCount >= 3) {
			auto triCount = fvCount - 2;
			auto startVi = static_cast<Index>(added.range.start);
			auto indices = added.indices().add<Index>(triCount * 3);
			auto vi = ax_foreach_begin(indices);
			for (axInt tri = 0; tri < triCount; tri++) {
				*vi = static_cast<Index>(startVi          ); vi++;
				*vi = static_cast<Index>(startVi + tri + 1); vi++;
				*vi = static_cast<Index>(startVi + tri + 2); vi++;
			}
		}
	}
}

void axRenderMeshEdit::addLinesFromVertexNormals(VertexDesc vertexDesc, const Mesh& src, float normalLength, const axColorPair& color) {
	for (auto& sm : src.subMeshes()) {
		addLinesFromVertexNormals(vertexDesc, sm, normalLength, color);
	}
}

void axRenderMeshEdit::addLinesFromVertexNormals(VertexDesc vertexDesc, const SubMesh& src, float normalLength, const axColorPair& color) {
	addLinesFromVertexNormals(vertexDesc, src.vertices, normalLength, color);
}

void axRenderMeshEdit::addLinesFromVertexNormals(VertexDesc vertexDesc, const VertexArray& vertexArray, float normalLength, const axColorPair& color) {
	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, vertexArray.size() * 2);

	auto srcPositions	= vertexArray.positions();
	auto srcNormals		= vertexArray.normals();

	if (added.hasPositions()) {
		auto srcNormal = ax_foreach_begin(srcNormals);
		auto dst = ax_foreach_begin(added.positions());
		for (auto& srcPos : srcPositions) {
			*dst = srcPos;
			dst++;

			*dst = axVec3f(srcPos) + axVec3f(*srcNormal) * normalLength;
			dst++;
			srcNormal++;
		}
	}

	if (added.hasColorSet(0)) {
		axColorRGBAh c[] = { axColorRGBAh_make(color.c0), axColorRGBAh_make(color.c1)};
		added.colorSet(0).fillRotateValues(c);
	}
}

void axRenderMeshEdit::createLinesFromVertexNormals(VertexDesc vertexDesc, EditableMesh& srcMesh, float normalLength, const axColorPair& color) {
	_mesh.clear();

	auto n = srcMesh.points().size();
	if (n <= 0) return;

	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, n * 2);

	if (added.hasPositions()) {
		auto dst = ax_foreach_begin(added.positions());
		for (auto& pt : srcMesh.points()) {
			dst->setByCast(pt.pos);
			dst++;

			dst->setByCast(pt.pos + pt.normal * normalLength);
			dst++;
		}
	}

	if (added.hasColorSet(0)) {
		axColorRGBAh c[] = { axColorRGBAh_make(color.c0), axColorRGBAh_make(color.c1)};
		added.colorSet(0).fillRotateValues(c);
	}
}

void axRenderMeshEdit::createLinesFromEdgeNormals(VertexDesc vertexDesc, EditableMesh& srcMesh, float normalLength, const axColorPair& color) {
	_mesh.clear();

	auto n = srcMesh.edges().size();
	if (n <= 0) return;

	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, n * 2);

	if (added.hasPositions()) {
		auto dst = ax_foreach_begin(added.positions());
		for (auto& e : srcMesh.edges()) {
			auto center = e.center(srcMesh);
			dst->setByCast(center);
			dst++;

			dst->setByCast(center + e.normal * normalLength);
			dst++;
		}
	}

	if (added.hasColorSet(0)) {
		axColorRGBAh c[] = { axColorRGBAh_make(color.c0), axColorRGBAh_make(color.c1)};
		added.colorSet(0).fillRotateValues(c);
	}

}

void axRenderMeshEdit::createLinesFromFaceNormals(VertexDesc vertexDesc, EditableMesh& srcMesh, float normalLength, const axColorPair& color) {
	auto n = srcMesh.faces().size();
	if (n <= 0) return;

	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, n * 2);

	if (added.hasPositions()) {
		auto dst = ax_foreach_begin(added.positions());
		for (auto& face : srcMesh.faces()) {
			auto center = face.center;
			dst->setByCast(center);
			dst++;

			dst->setByCast(center + face.normal * normalLength);
			dst++;
		}
	}

	if (added.hasColorSet(0)) {
		axColorRGBAh c[] = { axColorRGBAh_make(color.c0), axColorRGBAh_make(color.c1)};
		added.colorSet(0).fillRotateValues(c);
	}
}

void axRenderMeshEdit::createLinesFromFaceVertexNormals(VertexDesc vertexDesc, EditableMesh& srcMesh, float normalLength, const axColorPair& color) {
	auto n = srcMesh.faceEdges().size();
	if (n <= 0) return;

	if (srcMesh.fvNormals().size() < n) return;

	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, n * 2);

	if (added.hasPositions()) {
		auto dst = ax_foreach_begin(added.positions());
		for (auto& fe : srcMesh.faceEdges()) {
			auto pos = fe.point(srcMesh).pos;
			dst->setByCast(pos);
			dst++;

			dst->setByCast(pos + fe.normal(srcMesh) * normalLength);
			dst++;
		}
	}

	if (added.hasColorSet(0)) {
		axColorRGBAh c[] = { axColorRGBAh_make(color.c0), axColorRGBAh_make(color.c1)};
		added.colorSet(0).fillRotateValues(c);
	}
}

void axRenderMeshEdit::createSphere(VertexDesc vertexDesc, float radius, axInt rows, axInt columns) {
	EditableMesh tmp;
	tmp.edit().createSphere(radius, rows, columns);
	tmp.addColorSet(axColor(1,1,1,1));
	createFromEditableMesh(vertexDesc, tmp);
}

void axRenderMeshEdit::createCylinder(VertexDesc vertexDesc, float height, float radius, axInt rows, axInt columns, bool topCap, bool bottomCap) {
	EditableMesh tmp;
	tmp.edit().createCylinder(height, radius, rows, columns, topCap, bottomCap);
	tmp.addColorSet(axColor(1,1,1,1));
	createFromEditableMesh(vertexDesc, tmp);
}

void axRenderMeshEdit::createCone(VertexDesc vertexDesc, float height, float radius, axInt rows, axInt columns, bool bottomCap) {
	EditableMesh tmp;
	tmp.edit().createCone(height, radius, rows, columns, bottomCap);
	tmp.addColorSet(axColor(1,1,1,1));
	createFromEditableMesh(vertexDesc, tmp);
}

void axRenderMeshEdit::createLinesFromEdges(VertexDesc vertexDesc, EditableMesh& srcMesh, const axColor& color_) {
	_mesh.clear();
	auto color = axColorRGBAh_make(color_);

	auto n = srcMesh.edges().size();
	if (n <= 0) return;

	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, n * 2);

	if (added.hasPositions()) {
		auto dst = ax_foreach_begin(added.positions());
		for (auto& e : srcMesh.edges()) {
			dst->setByCast(e.point0(srcMesh).pos);
			dst++;

			dst->setByCast(e.point1(srcMesh).pos);
			dst++;
		}
	}

	if (added.hasColorSet(0)) {
		added.colorSet(0).fillValues(color);
	}
}

void axRenderMeshEdit::createLinesFromFaceEdges(VertexDesc vertexDesc, EditableMesh& srcMesh, const axColor& color_) {
	_mesh.clear();
	auto color = axColorRGBAh_make(color_);

	auto n = srcMesh.faceEdges().size();
	if (n <= 0) return;

	auto added = _mesh.addVertices(PrimType::Lines, vertexDesc, IndexType::Unknown, n * 2);

	for (auto& f : srcMesh.faces()) {
		auto faceEdges = f.getFaceEdges(srcMesh);

		if (added.hasPositions()) {
			auto dst = ax_foreach_begin(added.positions());
			for (auto& fe : faceEdges) {
				auto& e = fe.edge(srcMesh);

				dst->setByCast(e.point0(srcMesh).pos);
				dst++;

				dst->setByCast(e.point1(srcMesh).pos);
				dst++;
			}
		}

		if (added.hasColorSet(0)) {
			added.colorSet(0).fillValues(color);
		}
	}
}

void axRenderMeshEdit::createTextFromPointIds(VertexDesc vertexDesc, EditableMesh& srcMesh, const axFontStyle* fontStyle) {
	_mesh.clear();
	axTempString text;
	for (auto& v : srcMesh.points()) {
		text.format("{?}", ax_enum_int(v.id()));
		addTextBillboard(vertexDesc, text, axVec3f::s_cast(v.pos), fontStyle);
	}
}

void axRenderMeshEdit::createTextFromFaceIds(VertexDesc vertexDesc, EditableMesh& srcMesh, const axFontStyle* fontStyle) {
	_mesh.clear();
	axTempString text;
	for (auto& f : srcMesh.faces()) {
		text.format("{?}", ax_enum_int(f.id()));
		addTextBillboard(vertexDesc, text, axVec3f::s_cast(f.center), fontStyle);
	}
}

void axRenderMeshEdit::createTextFromEdgeIds(VertexDesc vertexDesc, EditableMesh& srcMesh, const axFontStyle* fontStyle) {
	_mesh.clear();
	axTempString text;
	for (auto& e : srcMesh.edges()) {
		text.format("{?}", ax_enum_int(e.id()));
		addTextBillboard(vertexDesc, text, axVec3f::s_cast(e.center(srcMesh)), fontStyle);
	}
}

void axRenderMeshEdit::createTextFromFaceEdgeIds(VertexDesc vertexDesc, EditableMesh& srcMesh, const axFontStyle* fontStyle) {
	_mesh.clear();
	axTempString text;
	for (auto& fe : srcMesh.faceEdges()) {
		text.format("{?}", ax_enum_int(fe.id()));
		addTextBillboard(vertexDesc, text, axVec3f::s_cast(fe.point(srcMesh).pos), fontStyle);
	}
}

void axRenderMeshEdit::setColor(const axColor color, axInt colorSet) {
		auto c = axColorRGBAh_make(color);
		for (auto& sm : _mesh.subMeshes()) {
			if (sm.vertices.hasColorSet(colorSet)) {
				sm.vertices.editColorSet(colorSet).fillValues(c);
			}
		}
}
