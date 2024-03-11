#include "SceneView_Tool.h"
#include "../SceneViewPanel.h"

namespace Anon { namespace Editor {

SceneView_Tool::SceneView_Tool()
	: _isMouseHover(false)
{
}

axRay3f SceneView_Tool::getCameraRay(const axVec2f& pt) const {
	if (_panel) {
		return _panel->getCameraRay(pt);
	}
	return axRay3f::s_zero();
}


}}