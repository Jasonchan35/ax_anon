#include "axUIEvent.h"

void axUIRenderEventHandler::onRender(axRenderRequest& req) {
}

void axUIRenderEventHandler::onUpdateGpuResources(axRenderCommandDispatcher& dispatcher) {
	axFontManager::s_instance()->onUpdateGpuResources(dispatcher);
}

void axUIMouseEvent::onFormat(axFormat& f) const {
	f.format("[UIMouseEvent: type={?} pos={?} worldPos={?} button={?} pressedButtons={?} modifier={?} time={?}]"
			, type, pos, worldPos, button, pressedButtons, modifier, time);
}

void axUIKeyEvent::onFormat(axFormat& f) const {
	f.format("[UIKeyboardEvent: type={?} key={?} text={?}, time={?}]", type, key, text, time);
}

