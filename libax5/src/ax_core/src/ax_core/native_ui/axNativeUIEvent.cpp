#include "axNativeUIEvent.h"

void axNativeUIMouseEvent::onFormat(axFormat& f) const {
	f.format("[NativeUIMouseEvent: type={?} button={?} worldPos={?} modifier={?} time={?}]", type, button, worldPos, modifier, time);
}

void axNativeUIKeyEvent::onFormat(axFormat& f) const {
	f.format("[NativeUIKeyboardEvent: type={?} key={?} text={?} modifier={?} time={?}]", type, key, text, modifier, time);
}
