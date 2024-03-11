#pragma once

#include "../EditorPanel.h"
#include "Tools/SceneView_Tool.h"

namespace Anon{ namespace Editor {

class SceneView_ToolView : public axUIView {
	AX_RTTI_CLASS(SceneView_ToolView, axUIView)
public:
	using Tool = SceneView_Tool;

	SceneView_ToolView(SceneViewPanel* parent);

	virtual void onRender(axRenderRequest& req) override;
	virtual void onMouseEvent(axUIMouseEvent& ev) override;
	virtual bool onHitTest(axUIHitTest& ht) override;

	axUPtr<Tool>	_tool;
	SceneViewPanel*	_panel = nullptr;
};


class SceneViewPanel : public EditorPanel {
	AX_RTTI_CLASS(SceneViewPanel, EditorPanel)
public:
	using Tool = SceneView_Tool;

	SceneViewPanel(axUIDockZone* zone);

	virtual void onMouseEvent(axUIMouseEvent& ev) override;
	virtual void onKeyEvent(axUIKeyEvent& ev) override;
	virtual void onRender(axRenderRequest& req) override;
	virtual void onUpdateLayout() override;
	virtual void onEditorSelectionChanged() override;
	virtual void onAnonContextChanged(AnonContext::ChangedFlags flags) override;

	Ray3f	getCameraRay(const Vec2f& point);

	void moveCamera(const Vec3f& deltaPos);

private:
	axVec2f _lastMousePos;
	axRenderMesh _grid;

	axEditableMesh	_editableMesh;
	axRenderMesh	_editableMeshDisplayEdges;
	axRenderMesh	_editableMeshDisplayNormals;
	axRenderMesh	_editableMeshDisplayPointIds;

	axRenderMesh	_model;
	axRenderMesh	_modelDisplay;

	axRenderMesh		_mlSamplesMesh;
	float				_mlGraphScale = 1;

	axRenderTerrain3d	_terrain;
	axSPtr<axTexture2D>	_testTexture;
	axSPtr<axTexture2D>	_testTexture2;
	axTextureSprite		_testSprite;

	axSPtr<axTexture2D>	_testImagePainter;

	axCamera3f _camera;

	class ToolBar : public axUIToolBar {
		AX_RTTI_CLASS(ToolBar, axUIToolBar)
	public:
		ToolBar(axUIView* parent);

		axUIToggleButton		debugRenderButton;
		axUIToggleButton		vSyncButton;

		axUIToggleButton		displayWireframeButton;
		axUIToggleButton		enableRayTracingButton;

		axUILabel				displayIdsLabel;
		axUIChoiceBox			displayIds;

		axUILabel				displayNormalsLabel;
		axUIChoiceBox			displayNormals;
		axUINumberBox<float>	displayNormalLength;

		axRenderComponentFlags	_renderComponentIds		= axRenderComponentFlags::None;
		axRenderComponentFlags	_renderComponentNormals = axRenderComponentFlags::None;
	};

	SceneView_ToolView	_toolView;
	ToolBar	_toolBar;

	axVec3f	_testPos;
	axArray<axVec3f> _testCurve;

	bool _testStageBufferDone = false;
	axSPtr<axRenderStageBuffer>	_testStageBuffer;
	axSPtr<axRenderFence>		_testStageBufferFence;
};

}} //namespace
