#pragma once

#include "../axRenderRequest.h"

#define axRenderTerrain3_ZoneMask_EnumList(EV) \
	EV(North, = 1 << 0)  \
	EV(East,  = 1 << 1)  \
	EV(South, = 1 << 2)  \
	EV(West,  = 1 << 3)  \
//----
ax_ENUM_FLAGS_CLASS(axRenderTerrain3_ZoneMask, axUInt8);

template<class T>
class axRenderTerrain3 : public axNonCopyable {
	using This = axRenderTerrain3<T>;

public:
	using Terrain			= This;
	using Vec2				= axVec2<T>;
	using Vec3				= axVec3<T>;
	using SubMesh			= axRenderSubMesh;
	using VertexArray		= axRenderVertexArray;
	using IndexArray	= axRenderIndexArray;
	using IndexType	= axRenderIndexType;
	using VertexIndex		= axUInt16;
	using Material			= axMaterial;
	using PrimitiveType		= axRenderPrimitiveType;
	using ZoneMask			= axRenderTerrain3_ZoneMask;

	void createFromImageFile(const Vec2& terrainSize, T terrainHeight, axInt maxLodLevel, axStrView heightMapFile);
	void createFromImage	(const Vec2& terrainSize, T terrainHeight, axInt maxLodLevel, const axImage& heightMap);

	void destroy();

	void render(axRenderRequest& req, const Vec3& viewPos, axSPtr<Material>& mtl);
	void renderPoints(axRenderRequest& req);
	void renderNormals(axRenderRequest& req);

	class Patch {
	public:
		void create(Terrain& terrian, const axVec2i& pos, const axImage& heightMap);
		const VertexArray& 	vertices() { return _vertices; }

		const axVec2i&	pos() const { return _pos; }
			  axVec2<T> worldCenterPos() const;

		void setDisplayLevel(const Vec3& viewPos);
		axInt displayLevel() const { return _displayLevel; }

		void renderPoints(axRenderRequest& req);
		void renderNormals(axRenderRequest& req);
		void render(axRenderRequest& req, axSPtr<Material>& mtl);

	private:
		using Vertex = axRenderVertex_PosColorNormal;

		inline	bool _adjacentPatchHasHigherLod(axInt x, axInt y) const;
				T	 _getHeight(const axImage& heightMap, axInt x, axInt y);

		axInt				_displayLevel = 0;
		axVec2i				_pos{0,0};
		axRenderTerrain3*	_terrain = nullptr;
		VertexArray			_vertices;
		axRenderMesh		_debugNormalMesh;
	};

	class TriIndexSet {
	public:
		void create(Terrain& terrain, axInt level, ZoneMask zoneMask);
		const IndexArray& indices() const { return _indices; }

	private:
		void _addToIndices(axSpan<axVec2i> sector, axInt vertexPerRow, axVec2i direction, bool flipXY);

		IndexArray	_indices;
	};

	class Level {
	public:
		static const axInt s_triIndexSetCount = 16;

		void create(Terrain& terrain, axInt level);

		const TriIndexSet& triIndexSet(ZoneMask zoneMask) const {
			return _triIndexSets[ax_enum_int(zoneMask)];
		}

	private:
		Terrain* _terrain = nullptr;
		axInt _level = 0;
		axArray<TriIndexSet, s_triIndexSetCount>	_triIndexSets;
	};

	Patch* patch(axInt x, axInt y) {
		if (x < 0 || y < 0 || x >= _patchCount.x || y >= _patchCount.y)
			return nullptr;
		return &_patches[y * _patchCount.x + x];
	}

			axInt	cellPerPatchRow		() const { return axInt(1) << (_maxLodLevel - 1); }
			axInt	vertexPerPatchRow	() const { return cellPerPatchRow() + 1; }
			axInt	maxLodLevel			() const { return _maxLodLevel; }

	const	Vec2&	terrainSize			() const { return _terrainSize; }
			T		terrainHeight		() const { return _terrainHeight; }

			axVec2i	patchCount			() const { return _patchCount; }
			Vec2	patchSize			() const { return _terrainSize / Vec2::s_cast(_patchCount); }
			Vec2	cellSize			() const { return _terrainSize / Vec2::s_cast(_heightmapResolution - 1); }

			axSpan<Level>	levels() const { return _levels; }

private:
	Vec2				_terrainSize {0, 0};
	T					_terrainHeight = 0;
	axVec2i				_heightmapResolution {0, 0};
	axInt				_maxLodLevel = 1;
	axVec2i				_patchCount {0, 0};
	axArray<Patch>		_patches;
	axArray<Level>		_levels;
};

using axRenderTerrain3f = axRenderTerrain3<float>;
using axRenderTerrain3d = axRenderTerrain3<double>;
