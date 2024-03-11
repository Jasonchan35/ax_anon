#include "axNativeUIWindow_Base.h"
#include <ax_core/rtti/axTypeManager.h>

axNativeUIWindow_Base::axNativeUIWindow_Base(CreateDesc& desc) {
	_active = false;
}

void axNativeUIWindow_Base::setActive(bool b) {
	auto re = _re_setActive.enter();
	if (!re) return;
	onSetNativeActive(b);
	onActive(b);
}

void axNativeUIWindow_Base::setVisible(bool b) {
	onSetNativeVisible(b);
}

void axNativeUIWindow_Base::setWorldPos(const axVec2f& pos) {
	if (_worldRect.pos == pos) return;
	auto re = _re_setPos.enter();
	if (!re) return;
	onSetNativeWorldPos(pos);
	onSetWorldPos(pos);
}

void axNativeUIWindow_Base::setSize(const axVec2f& size) {
	if (_worldRect.size == size) return;
	auto re = _re_setSize.enter();
	if (!re) return;
	onSetNativeSize(size);
	onSetSize(size);
}

void axNativeUIWindow_Base::setWindowTitle(axStrView title) {
	onSetNativeWindowTitle(title);
}

void axNativeUIWindow_Base::_onNativeUIMouseEvent(axNativeUIMouseEvent& ev) {
	onNativeUIMouseEvent(ev);
}

void axNativeUIWindow_Base::_onNativeUIKeyEvent(axNativeUIKeyEvent& ev) {
	onNativeUIKeyEvent(ev);
}

