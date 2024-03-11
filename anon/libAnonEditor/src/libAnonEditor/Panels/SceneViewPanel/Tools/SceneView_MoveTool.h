#pragma once

#include "SceneView_Tool.h"

namespace Anon { namespace Editor {

class SceneView_MoveTool : public SceneView_Tool {
	AX_RTTI_CLASS(SceneView_MoveTool, SceneView_Tool);
public:
	SceneView_MoveTool();
	virtual bool onHitTest(axUIHitTest& ht) override;
	virtual void onRender(axRenderRequest& req) override;
	virtual void onMouseEvent(axUIMouseEvent& ev) override;

private:
	void _updatePos();

	struct Axis {
		axRenderMesh		mesh;
		axVec3f				pos0, pos1;
		float				screenScale = 1;
		axColor				color;
		axVec3f				dir;

		void init(const axColor& color_, const axVec3f dir_);
		void onRender(SceneView_MoveTool& owner, axRenderRequest& req, const axVec3f& pos0_, const axVec3f& pos1_, float screenScale_);
		bool hitTest(const axRay3f& ray);
	};

	void onMoved(const axVec3f& newPos, bool isPreview);

	bool _pointOnAxis(axVec3f& outPos, const axVec2f& pt);

	Axis* hitAxis(const axVec2f& pt);
	void selectAxis(Axis* s);

	axVec3f	_pos {0,40,30};

	Axis	_axisX;
	Axis	_axisY;
	Axis	_axisZ;
	Axis*	_selectedAxis = nullptr;
	Axis*	_hoverAxis = nullptr;
	float	_axisHeight = 0;

	axVec3f	_lastDragPos;
};

}} //namespace
