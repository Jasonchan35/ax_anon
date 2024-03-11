#include "axRenderTerrain.h"
#include <ax_render/axRenderContext.h>

template<class T>
void axRenderTerrain3<T>::Patch::create(Terrain& terrain, const axVec2i& pos, const axImage& heightMap) {
	_terrain = &terrain;
	_pos	 = pos;

	auto cellPerRow	  = _terrain->cellPerPatchRow();
	auto vertexPerRow = cellPerRow + 1;

	axInt vertexCount = vertexPerRow * vertexPerRow;

	if (vertexCount >= axNumLimit<VertexIndex>::max())
		throw axError_Undefined(AX_LOC);

	auto  vertices	  = _vertices.create<Vertex>(vertexCount);

	auto cellSize	   = terrain.cellSize();
	auto patchSize	   = terrain.patchSize(); // + 2.0f; // offset for debug

	auto color = axColorRGBAh_make(1.0f, 1.0f, 1.0f, 1.0f);

	switch (heightMap.colorType()) {
		case axColorType::Ls: {
			auto* v	= vertices.begin();
			for (axInt y = 0; y < vertexPerRow; y++) {
				axInt mapY = pos.y * cellPerRow + y;

				for (axInt x = 0; x < vertexPerRow; x++) {
					axInt mapX = pos.x * cellPerRow + x;

					T height = _getHeight(heightMap, mapX, mapY);
					v->pos.set(	static_cast<float>(cellSize.x * x + patchSize.x * pos.x),
								static_cast<float>(height),
								static_cast<float>(cellSize.y * y + patchSize.y * pos.y));

					//v->pos.y = (ax_sin(v->pos.x * 0.02f) + ax_sin(v->pos.z * 0.02f)) * 16.0f;
					// normal
					{
						T w = mapX <= 0 ? 0 : _getHeight(heightMap, mapX - 1, mapY) - height;
						T n = mapY <= 0 ? 0 : _getHeight(heightMap, mapX, mapY - 1) - height;
						T e = mapX + 1 >= heightMap.width()  ? 0 : _getHeight(heightMap, mapX + 1, mapY) - height;
						T s = mapY + 1 >= heightMap.height() ? 0 : _getHeight(heightMap, mapX, mapY + 1) - height;

						Vec3 normal = Vec3(-cellSize.x, w, 0).cross(Vec3( 0, 0, 1)).normal()
									+ Vec3( cellSize.x, e, 0).cross(Vec3( 0, 0,-1)).normal()
									+ Vec3( 0, n,-cellSize.y).cross(Vec3(-1, 0, 0)).normal()
									+ Vec3( 0, s, cellSize.y).cross(Vec3( 1, 0, 0)).normal();
						normal /= 4;
						v->normal = axVec3f::s_cast(normal.normal());
					}
					//v->normal = axVec3f(0,1,0);

					v->color[0] = color;
					v++;
				}
			}
			AX_ASSERT(v == vertices.end());
		} break;

		default: throw axError_Undefined(AX_LOC);
	}
}

template<class T>
T axRenderTerrain3<T>::Patch::_getHeight(const axImage& heightMap, axInt x, axInt y) {
	auto h = heightMap.pixel<axColorLs>(x, y).luma;
	T o = axColorElementUtil::to<T>(h) * _terrain->terrainHeight();
	return o;
}

template<class T>
void axRenderTerrain3<T>::Patch::renderPoints(axRenderRequest& req) {
	auto mtl = req.world.createMaterial_UnlitTexture0();
	req.addDrawCall(AX_LOC, mtl, PrimitiveType::Points, _vertices, nullptr);
}

template<class T>
void axRenderTerrain3<T>::Patch::renderNormals(axRenderRequest& req) {
	if (!_debugNormalMesh.vertexCount()) {
		_debugNormalMesh.edit().createLinesFromVertexNormals(axRenderVertex_PosColor::s_desc(), _vertices, 4, {axColor::kWhite(), axColor::kRed()} );
	}
	req.world.drawMesh(AX_LOC, _debugNormalMesh);
}

template<class T> inline
bool axRenderTerrain3<T>::Patch::_adjacentPatchHasHigherLod(axInt x, axInt y) const {
	auto* adj = _terrain->patch(_pos.x + x, _pos.y + y);
	if (!adj) return false;
	return adj->displayLevel() > _displayLevel;
}

template<class T>
void axRenderTerrain3<T>::Patch::render(axRenderRequest& req, axSPtr<Material>& mtl) {
	auto levels = _terrain->levels();
	if (levels.size() <= 0) return;

	auto zoneMask = axEnum<ZoneMask>::kZero;
	if (_adjacentPatchHasHigherLod( 0, -1)) zoneMask |= ZoneMask::North;
	if (_adjacentPatchHasHigherLod( 1,  0)) zoneMask |= ZoneMask::East;
	if (_adjacentPatchHasHigherLod( 0,  1)) zoneMask |= ZoneMask::South;
	if (_adjacentPatchHasHigherLod(-1,  0)) zoneMask |= ZoneMask::West;

	auto lv = ax_clamp(_displayLevel, axInt(0), levels.size() - 1);
	auto& ia = levels[lv].triIndexSet(zoneMask).indices();

	req.addDrawCall(AX_LOC, mtl, PrimitiveType::Triangles, _vertices, &ia);
}

template<class T>
axVec2<T> axRenderTerrain3<T>::Patch::worldCenterPos() const {
	auto s = _terrain->patchSize();
	auto o = (axVec2<T>::s_cast(_pos) + T(0.5)) * s;
	return o;
}

template<class T>
void axRenderTerrain3<T>::Patch::setDisplayLevel(const Vec3& viewPos) {
	auto distance = (worldCenterPos() - viewPos.xz()).length();
	auto d = _terrain->patchSize().x * T(4); //* T(1.5);
	_displayLevel = static_cast<axInt>(distance / d);
}

template<class T>
void axRenderTerrain3<T>::TriIndexSet::create(Terrain& terrain, axInt level, ZoneMask zoneMask) {
	_indices.create<VertexIndex>(0);

	axInt vertexPerRow = terrain.vertexPerPatchRow();
	axInt rows = axInt(1) << (terrain.maxLodLevel() - 1 - level);
	axInt n = rows / 2;
	axInt step = axInt(1) << level;

	if (rows == 1) {
		VertexIndex x0 = 0;
		VertexIndex x1 = static_cast<VertexIndex>(vertexPerRow - 1);
		VertexIndex y0 = 0;
		VertexIndex y1 = static_cast<VertexIndex>((vertexPerRow - 1) * vertexPerRow);

		auto dst = _indices.add<VertexIndex>(6);
		dst[0] = x0 + y0;
		dst[1] = x1 + y1;
		dst[2] = x1 + y0;

		dst[3] = x0 + y0;
		dst[4] = x0 + y1;
		dst[5] = x1 + y1;

	} else {
		axArray<axVec2i, 256> sector0;
		axArray<axVec2i, 256> sector1;

		for (axInt y = 0; y < n; y++) {
			axInt lastRow = (y == n - 1) ? 1 : 0;

			for (axInt x = 0; x <= y; x++) {
				axVec2i v[3];
				axInt odd = (x + y) % 2;

				v[0] = axVec2i(x,     y          ) * step;
				v[1] = axVec2i(x + 1, y + 1 - odd) * step;
				v[2] = axVec2i(x,     y + 1      ) * step;
				sector0.appendRange(v);

				if (lastRow) {
					v[2].x = (x - 1 + odd) * step;
				}
				sector1.appendRange(v);

				if (x == y) break; // drop last triangle in row

				v[0] = axVec2i(x,     y + odd) * step;
				v[1] = axVec2i(x + 1, y      ) * step;
				v[2] = axVec2i(x + 1, y + 1  ) * step;
				sector0.appendRange(v);

				if (!lastRow || !odd) { // drop half triangle in last row
					sector1.appendRange(v);
				}
			}
		}

		{ // north
			auto& sector = ax_enum(zoneMask).hasFlags(ZoneMask::North) ? sector1 : sector0;
			_addToIndices(sector, vertexPerRow, axVec2i( 1,-1), false);
			_addToIndices(sector, vertexPerRow, axVec2i(-1,-1), false);
		}
		{ // east
			auto& sector = ax_enum(zoneMask).hasFlags(ZoneMask::East) ? sector1 : sector0;
			_addToIndices(sector, vertexPerRow, axVec2i( 1, 1), true);
			_addToIndices(sector, vertexPerRow, axVec2i(-1, 1), true);
		}
		{ // south
			auto& sector = ax_enum(zoneMask).hasFlags(ZoneMask::South) ? sector1 : sector0;
			_addToIndices(sector, vertexPerRow, axVec2i( 1, 1), false);
			_addToIndices(sector, vertexPerRow, axVec2i(-1, 1), false);
		}
		{ // west
			auto& sector = ax_enum(zoneMask).hasFlags(ZoneMask::West) ? sector1 : sector0;
			_addToIndices(sector, vertexPerRow, axVec2i( 1,-1), true);
			_addToIndices(sector, vertexPerRow, axVec2i(-1,-1), true);
		}
	}
}

template<class T>
void axRenderTerrain3<T>::TriIndexSet::_addToIndices(axSpan<axVec2i> sector, axInt vertexPerRow, axVec2i direction, bool flipXY) {
	axVec2i center(vertexPerRow / 2);
	axInt vertexPerPatch = vertexPerRow * vertexPerRow;

	auto dstArray = _indices.add<VertexIndex>(sector.size());
	{
		auto* dst = dstArray.begin();
		for (auto s : sector) {
			s = s * direction + center;
			if (flipXY) s = s.yx();
			AX_ASSERT(s.x >= 0 && s.y >= 0);

			auto vi = static_cast<VertexIndex>(s.x + s.y * vertexPerRow);
			AX_ASSERT(vi < vertexPerPatch);

			*dst = vi;
			dst++;
		}
		AX_ASSERT(dst == dstArray.end());
	}


	axInt rx = direction.x < 0 ? 1 : 0;
	axInt ry = direction.y < 0 ? 1 : 0;
	axInt rf = flipXY ? 0 : 1;

	if (rx ^ ry ^ rf) {
		auto* dst = dstArray.begin();
		auto* end = dstArray.end();
		for (; dst < end; dst += 3) {
			ax_swap(dst[0], dst[1]);
		}
	}
}

template<class T>
void axRenderTerrain3<T>::Level::create(Terrain& terrain, axInt level) {
	_terrain = &terrain;
	_level = level;
	
	_triIndexSets.resize(s_triIndexSetCount);
	auto zoneMask = axEnum<ZoneMask>::kZero;
	for (auto& it : _triIndexSets) {
		it.create(terrain, level, zoneMask);
		zoneMask++;
	}
}

template<class T>
void axRenderTerrain3<T>::createFromImageFile(const Vec2& terrainSize, T terrainHeight, axInt maxLodLevel, axStrView heightMapFile) {
	axImage img;
	img.loadFile(heightMapFile);
	createFromImage(terrainSize, terrainHeight, maxLodLevel, img);
}

template<class T>
void axRenderTerrain3<T>::createFromImage(const Vec2& terrainSize, T terrainHeight, axInt maxLodLevel, const axImage& heightMap) {
	destroy();

	if (heightMap.width() < 1 || heightMap.height() < 1)
		throw axError_Undefined(AX_LOC);

	if (maxLodLevel < 1)
		maxLodLevel = 1;

	if (maxLodLevel > 8) // vertex index > uint16
		throw axError_Undefined(AX_LOC);

	_heightmapResolution.set(heightMap.width(), heightMap.height());
	_terrainSize = terrainSize;
	_terrainHeight = terrainHeight;

	_maxLodLevel = maxLodLevel;

	_patchCount = (_heightmapResolution - 1) / cellPerPatchRow();

	_patches.resize(_patchCount.x * _patchCount.y);
	_levels.resize(maxLodLevel);

	for (axInt lv = 0; lv < _levels.size(); lv++) {
		_levels[lv].create(*this, lv);
	}

	{
		auto* p = _patches.begin();
		for (axInt y = 0; y < _patchCount.y; y++) {
			for (axInt x = 0; x < _patchCount.x; x++) {
				p->create(*this, axVec2i(x, y), heightMap);
				p++;
			}
		}
		AX_ASSERT(p == _patches.end());
	}
}

template<class T>
void axRenderTerrain3<T>::destroy() {
	_terrainSize.set(0, 0);
	_terrainHeight = 0;

	_heightmapResolution.set(0, 0);
	_maxLodLevel = 1;

	_patchCount.set(0, 0);
	_patches.clear();
	_levels.clear();
}

template<class T>
void axRenderTerrain3<T>::render(axRenderRequest& req, const Vec3& viewPoint, axSPtr<Material>& mtl) {
	for (auto& p : _patches) {
		p.setDisplayLevel(viewPoint);
	}

	for (auto& p : _patches) {
		p.render(req, mtl);
	}
}

template<class T>
void axRenderTerrain3<T>::renderPoints(axRenderRequest& req) {
	for (auto& p : _patches) {
		p.renderPoints(req);
	}
}

template<class T>
void axRenderTerrain3<T>::renderNormals(axRenderRequest& req) {
	for (auto& p : _patches) {
		p.renderNormals(req);
	}
}

//=================

template class axRenderTerrain3<float>;
template class axRenderTerrain3<double>;

