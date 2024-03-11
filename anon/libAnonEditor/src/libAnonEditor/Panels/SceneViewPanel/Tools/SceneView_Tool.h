#pragma once

namespace Anon { namespace Editor {

class SceneViewPanel;

class SceneView_Tool : public axObject {
	AX_RTTI_CLASS(SceneView_Tool, axObject)
public:
	SceneView_Tool();

			void render(axRenderRequest& req) { onRender(req); }
	virtual void onRender(axRenderRequest& req) {}
	virtual void onMouseEvent(axUIMouseEvent& ev) {}
	virtual bool onHitTest(axUIHitTest& ht) { return false; }

	SceneViewPanel*	panel() { return _panel; }

	axRay3f getCameraRay(const axVec2f& pt) const;

	bool isMouseHover() const { return _isMouseHover; }

friend class SceneViewPanel;
friend class SceneView_ToolView;
private:
	SceneViewPanel*	_panel = nullptr;
	bool _isMouseHover : 1;
};

}}