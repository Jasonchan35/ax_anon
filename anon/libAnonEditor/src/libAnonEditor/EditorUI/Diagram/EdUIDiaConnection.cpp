#include "EdUIDiaConnection.h"

namespace Anon { namespace Editor {

EdUIDiaConnection::EdUIDiaConnection(axUIDiaSocket* parent, axUIDiaSocket* target) 
	: Base(parent, target)
{
}

void EdUIDiaConnection::onRender(axRenderRequest& req) {
	auto* diaConn = diaConnection();

	if (isMouseHover()) {
		setColor(axColor(1,0,1));

	} else if (isSelected()) {
		setColor(axColor(1,1,0));

	} else if (!diaConn) {
		setColor(axColor(1,0,0));

	} else {
		setColor(axColor(.7f, .7f, .7f));

	}

	Base::onRender(req);

	if (isMouseHover()) {
		if (diaConn) {
			auto text = axTempString::s_convert(*diaConn);
			auto fontStyle = axUIDefaultAssets::s_instance()->fontStyles.stdWithBg;

			auto pos = axUIInputManager::s_instance()->mouseWorldPos() - worldPos();
			pos.x -= 100;
			pos.y -= 30;

			req.ui.drawText(AX_LOC, text, pos, fontStyle);
		}
	}
}

}}