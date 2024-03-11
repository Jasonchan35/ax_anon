#include "axUIApp.h"
#include "axUIWindow.h"
#include "axUIView.h"
#include "axUIRootView.h"
#include "../layout/axUIDefaultLayout.h"
#include "axUIRenderContextView.h"

void axUIView::_ctor() {
	setLayoutHandler(axUIDefaultLayoutHandler::s_instance());

	layout._view = this;
	_debugDrawRect = false;

	_enable = true;
	_visible = true;

	_needToMeasure = false;
	_needToLayout  = false;
	_childrenNeedToLayout = false;

	_draggable = false;
	_clipContent = false;
	_wantMouseWheelEvent = false;
	_wantMouseDragEvent = true;
	_worldPosDirty = true;

	_hasFocus = false;
	_acceptFocus = true;

	_isMouseHover = false;
	_handling_parentChanged = false;
}

axUIView::axUIView(const NullParent&) {
	_ctor();
}

axUIView::axUIView(axUIView* parent) {
	AX_ASSERT(parent);
	if (this == parent) {
		AX_ASSERT(false);
		parent = nullptr;
	}

	_ctor();
	if (parent) {
		setParent(parent);
	}
}

void axUIView::onSetEnable(bool b) {
	if (_enable == b) return; 
	_enable = b;
	setNeedToRender();
}

void axUIView::onSetVisible(bool b) {
	if (_visible == b) return;
	_visible = b;
	setNeedToMeasure();
	setNeedToRender();
}

axUIView::~axUIView() {
	setFocus(false);

	if (_visible) {
		setNeedToRender();
	}

	clearChildren();
	setParent(nullptr);
}

void axUIView::setNeedToDestroy() {
	axUIApp::s_instance()->setNeedToDestroyUIView(this);
}

void axUIView::setBgColor(const Color& c) {
	if (_bgColor == c) return;
	_bgColor = c;
	setNeedToRender();
}

void axUIView::setBorderColor(const Color& c) {
	if (_borderColor == c) return;
	_borderColor = c;
	setNeedToRender();
}

void axUIView::setBorderWidth(const Margin2& m) {
	if (_borderWidth == m) return;
	_borderWidth = m;
	setNeedToRender();
}

void axUIView::setDebugDrawRect(bool b) {
	_debugDrawRect = b;
}

void axUIView::dump(axInt level) {
	axString s;
	s.appendRepeat("  ", level);
	s.appendFormat("{?}: {?}, rect={?} worldPos={?} clip={?}",
					debugName(), ax_typeof(this)->name(), _rect, worldPos(),
					_clipContent);
	s.appendFormat(" padding={?}", _padding);
	if (1) {
		s.appendFormat(" layout(mode=[{?},{?}], need={?},{?} measured=[{?}] min={?}, max={?}, exclude={?})",
						layout.modeW(), layout.modeH(),
						_needToLayout, _needToMeasure,
						layout.measured(),
						layout.min(), layout.max(),
						layout.excludeFromLayout());
	}

	AX_LOG("{?}", s);
	for (auto& p : _children) {
		p.dump(level + 1);
	}
}

void axUIView::onFormat(axFormat& f) const {
	f.format("({?} {?}: {?})", (void*)this, debugName(), ax_typeof(this));
}

#if 0
#pragma mark ------------ Position --------------
#endif

axRect2f axUIView::rectOnWindow() const {
	auto rc = worldRect();
	if (_uiWindow) {
		rc.pos -= _uiWindow->worldPos();
		rc.pos -= _uiWindow->padding().topLeft();
	}
	return rc;
}

void axUIView::moveWithinWorldRect(const Rect2& withinRect) {
	auto rc = worldRect();

	bool needToMove = false;

	if (rc.x < withinRect.x) {
		rc.x = withinRect.x;
		needToMove = true;
	} else if (rc.xMax() > withinRect.xMax()) {
		rc.x = withinRect.xMax() - rc.w - 1;
		needToMove = true;
	}

	if (rc.y < withinRect.y) {
		rc.y = withinRect.y;
		needToMove = true;
	} else if (rc.yMax() > withinRect.yMax()) {
		rc.y = withinRect.yMax() - rc.h - 1;
		needToMove = true;
	}

	if (needToMove) {
		setWorldPos(rc.pos);
	}
}

void axUIView::setPos(const Vec2 & v) {
	auto re = _re_setPos.enter();
	if (!re) return;
	onSetPos(v);
}

void axUIView::setSize(const Vec2 & v) {
	auto re = _re_setSize.enter();
	if (!re) return;
	onSetSize(v);
}

void axUIView::onSetPos(const Vec2& v) {
	if (_rect.pos == v) return;
	_rect.pos = v;
	setWorldPosDirty();
	setNeedToRender();
}

void axUIView::onSetSize(const Vec2& v) {
	if (_rect.size == v) return;
	_rect.size = v;
	setNeedToMeasure();
}

void axUIView::setWorldPos(const Vec2& v) {
	auto pos = v;
	if (_parent) {
		pos -= _parent->worldPos();
	}
	setPos(pos);
}

void axUIView::setWorldPosDirty() {
	if (_worldPosDirty) return;
	_worldPosDirty = true;

	for (auto& c : _children) {
		c.setWorldPosDirty();
	}
}

const axVec2f& axUIView::worldPos() const {
	if (_worldPosDirty) {
		Vec2 pos = _rect.pos;
		if (_parent) {
			pos += _parent->worldPos();
		}
		ax_const_cast(this)->_worldPosDirty = false;
		ax_const_cast(this)->_worldPosComputed = pos;
	}
	return _worldPosComputed;
}

#if 0
#pragma mark ------------ Parent / Child --------------
#endif

void axUIView::setParent(axUIView* newParent) {
	if (_parent == newParent)
		return;

	if (_parent) {
		_parent->setNeedToMeasure();
		_parent->_children.remove(this);
	}
	_parent = newParent;

	if (_parent) {
		_parent->_children.append(this);
		_parent->setNeedToMeasure();
	}

	_handleParentChanged();
}

axUIScreen axUIView::uiScreen() { return _uiWindow ? _uiWindow->screen() : axUIScreen(); }

void axUIView::_handleParentChanged() {
	if (_handling_parentChanged) {
		AX_ASSERT(false);
		return;
	}
	_handling_parentChanged = true;
	onParentChanged();
	AX_ASSERT(!_handling_parentChanged); // forgot to call  Base::onParentChanged()
}

void axUIView::onParentChanged() {
	setWorldPosDirty();

	if (_uiWindow != this) {
		_uiWindow = _parent ? _parent->_uiWindow : nullptr;
	}

	if (_uiRenderContextView != this) {
		_uiRenderContextView = _parent ? _parent->_uiRenderContextView : nullptr;
	}

	for (auto& c : _children) {
		c._handleParentChanged();
	}

	_handling_parentChanged = false;
}

void axUIView::clearChildren() {
	while (auto* p = _children.tail()) {
		p->setParent(nullptr);
		ax_delete(p);
	}
}

void axUIView::setAsFirstSibling() {
	if (!_parent) return;
	_parent->_children.remove(this);
	_parent->_children.insert(this);
	setNeedToMeasure();
}

void axUIView::setAsLastSibling() {
	if (!_parent) return;
	_parent->_children.remove(this);
	_parent->_children.append(this);
	setNeedToMeasure();
}

void axUIView::setPadding(const Margin2& m) {
	if (_padding == m) return;
	_padding = m;
	setNeedToMeasure();
}

axUIView::Rect2 axUIView::getChildrenBounds() {
	if (!_children.size()) {
		return Rect2(0);
	}

	Rect2 rc = _children.head()->rect();
	for (auto& c : _children) {
		c.updateLayout();
		rc = rc.unionWith(c.rect());
	}
	return rc;
}

#if 0
#pragma mark ------------ Render --------------
#endif

void axUIView::onSetNeedToRender() {
	if (_uiRenderContextView) {
		_uiRenderContextView->setNeedToRender();
	}
}

void axUIView::setNeedToRender() {
	auto re = _re_setNeedToRender.enter();
	if (!re) return;
	onSetNeedToRender();
}

void axUIView::render(axRenderRequest& req) {
	if (!_visible)
		return;

	updateLayout();

	auto mv = req.ui.modelview.push();

	if (_uiRenderContextView != this) { // if this is render context, then pos should be start from (0,0)
		mv *= Mat4::s_translate(_rect.pos);
	}

	if (_clipContent && !req.scissorRect->isIntersected(worldRect())) {
		return; // don't draw if out of screen
	}

	auto rc = localRect();
	req.ui.drawRect(AX_LOC, rc, _bgColor);

	{
		auto sc = req.scissorRect.push();
		if (_clipContent) {
			sc = sc->intersects(worldRect());
		}
		onRender(req);
		onRenderChildren(req);

		req.ui.drawRectBorder(AX_LOC, rc, _borderWidth, _borderColor);
	}

	auto* app = axUIApp::s_instance();
	if (app->debug.showUIRect) {
		req.ui.drawRectBorder(AX_LOC, localRect(), Margin2(1), _isMouseHover ? Color(1, 0, 0, 1) : Color(0, 0, .8f, .25f));

		if (_isMouseHover) {
			req.ui.drawRect(AX_LOC, localRect(), Color(.8f, .8f, 0, .2f));
			{	// info text
				auto& style = axUIDefaultAssets::s_instance()->fontStyles.stdWithBg;

				const float textH = 18;
				axTempString str;
				onGetDebugInfo(str);
				req.ui.drawText(AX_LOC, str, Vec2(0, textH), style);
			}
		}
	}

	if (_debugDrawRect) {
		req.ui.drawRect(AX_LOC, contentRect(), axColor(.5f, .2f, .2f, .5f));
	}

//	if (_mouseHover) {
//		req.drawRect(AX_LOC, localRect(), axColor(1,0,0, 0.25f));
//	}
}

void axUIView::onGetDebugInfo(axIString& str) {
	str.appendFormat("{?}:{?}" , debugName(), ax_typeof(this)->name());
	str.appendFormat("\n  rect{?}", rect());
	str.appendFormat("\n  layout({?}, {?})", layout.modeW(), layout.modeH());
	str.appendFormat("\n  measured(min{?}, max{?})", layout.measured().min, layout.measured().max);
	if (_parent) {
		str.appendFormat("\n  parent {?}:{?} rect{?}", _parent->debugName(), ax_typeof(_parent)->name(), _parent->rect());
	}
}

void axUIView::onRender(axRenderRequest& req) {
}

void axUIView::onRenderChildren(axRenderRequest& req) {
	for (auto& c : _children) {
		c.render(req);
	}
}

#if 0
#pragma mark ------------ Keyboard Input --------------
#endif

void axUIView::handleKeyEvent(axUIKeyEvent& ev) {
	onKeyEvent(ev);
}

void axUIView::setFocus(bool b) {
	if (b) {
		if (!_acceptFocus) return;
		if (_hasFocus) return;
		if (auto* mgr = axUIInputManager::s_instance()) {
			mgr->_setFocus(this);
		}
	} else {
		if (!_hasFocus) return;
		if (auto* mgr = axUIInputManager::s_instance()) {
			if (mgr->focusView() != this) {
				AX_ASSERT(false);
				return;
			}
			mgr->_setFocus(nullptr);
		}
	}
}

void axUIView::setAcceptFocus(bool b) {
	_acceptFocus = b;
	if (!b) setFocus(false);
}

void axUIView::forwardEventTo(axUIView* target, axUIKeyEvent& ev) {
	if (!target) return;
	target->onKeyEvent(ev);
}

void axUIView::forwardEventToParent(axUIKeyEvent& ev) {
	forwardEventTo(_parent, ev);
}

void axUIView::onKeyEvent(axUIKeyEvent& ev) {
//	AX_DUMP_VAR(ev);
	forwardEventToParent(ev);
}

#if 0
#pragma mark ------------ Mouse / Touch Input --------------
#endif

bool axUIView::hitTest(axUIHitTest& ht) {
	if (!_visible)  return false;
	if (ht.dragAndDropSource == this) return false;

#if 0 // debug
	{
		if (ht.inEvent.type == axUIMouseEventType::Down) {
			if (ht.inEvent.modifier == axUIEventModifier::Atl) {
				AX_LOG("hitTest {?} localRect={?} ht.pos={?}", this, localRect(), ht.pos);
				printf("");
			}
		}
	}
#endif

	return onHitTest(ht);
}

bool axUIView::hitTestSelf(axUIHitTest& ht) {
	auto pos = ax_push(ht.pos);
	pos = ht.worldPos - worldPos();

	if (!onHitTestSelf(ht))
		return false;

	using Type = axUIMouseEventType;
	switch (ht.inEvent.type) {
		case Type::Wheel: {
			if (!_wantMouseWheelEvent) return false;
		}break;
		case Type::DragBegin:
		case Type::DragEnd:
		case Type::Dragging: {
			if (!_wantMouseDragEvent) return false;
		}break;
	}

	ht.result.view = this;
	return true;
}

bool axUIView::onHitTest(axUIHitTest& ht) {
	if (hitTestChildren(ht)) return true;
	if (hitTestSelf(ht)) return true;
	return false;
}

bool axUIView::onHitTestSelf(axUIHitTest& ht) {
	return hitTestRect().containsPoint(ht.pos);
}

bool axUIView::hitTestChildren(axUIHitTest& ht) {
	auto pos = ax_push(ht.pos);
	pos = ht.worldPos - worldPos();

	if (_clipContent && !contentRect().containsPoint(ht.pos))
		return false;

	for (auto& c : _children.revEach()) {
		if (!c.visible())
			continue;

		if (c.hitTest(ht)) return true;
	}
	return false;
}

void axUIView::handleMouseEvent(axUIMouseEvent& ev) {
	using Type	 = axUIMouseEvent::Type;
	using Button = axUIMouseEvent::Button;

	axUIView* target =this;

	switch (ev.type) {
		case Type::Enter: _setMouseHover(true);  break;
		case Type::Leave: _setMouseHover(false); break;
		case Type::Down: {
			if (ev.button == Button::Left) {
				setFocus(true);
			}
		}break;
	}

	ev.pos = ev.worldPos - target->worldPos();
	target->onMouseEvent(ev);
}

void axUIView::forwardEventToParent(axUIMouseEvent& ev) {
	forwardEventTo(_parent, ev);
}

void axUIView::forwardEventTo(axUIView* target, axUIMouseEvent& ev) {
	if (!target) return;
	ev.pos = ev.worldPos - target->worldPos();
	target->onMouseEvent(ev);
}

void axUIView::setDraggable(bool b) {
	_draggable = b;
}

void axUIView::_setMouseHover(bool b) {
	if (_isMouseHover != b) {
		_isMouseHover = b;
		setNeedToRender();
	}
}

#if 0
#pragma mark ------------ Layout--------------
#endif

void axUIView::reloadContent() {
	onReloadContent();
}

void axUIView::onReloadContent() {
	for (auto& c : _children) {
		c.reloadContent();
	}
}

void axUIView::onLinkedObjectDestroy(axObject* obj) {
	if (_linkedObject == obj) {
		setNeedToDestroy();
	}
}

axObject* axUIView::setLinkedObject(axObject* obj) {
	_linkedObject.setOwner(this);
	_linkedObject.ref(obj);
	return obj;
}

void axUIView::setNeedToMeasure() {
	setNeedToLayout();
	if (_needToMeasure) return;
	_needToMeasure = true;

	if (_parent) {
		_parent->setNeedToMeasure();
		_parent->setNeedToLayout();
	}
}

void axUIView::setNeedToLayout() {
	if (_needToLayout) return;
	_needToLayout = true;

	axUIApp::s_instance()->setNeedToLayoutRootView();

	auto* p = _parent;
	while (p) {
		if (p->_childrenNeedToLayout) break;
		p->_childrenNeedToLayout = true;
		p = p->_parent;
	}
}

void axUIView::updateLayout() {
	auto re = _re_updateLayout.enter();
	if (!re) return;

	auto& m = layout.measured();
	auto newSize = _rect.size;
	ax_max_it(newSize, m.min);
	setSize(newSize);

	if (_needToLayout) {
		onUpdateLayout();
	}

	if (_childrenNeedToLayout) {
		updateChildrenLayout();
	}

	_needToLayout = false;
	_childrenNeedToLayout = false;
}

void axUIView::updateChildrenLayout() {
	for (auto& c : _children) {
		c.updateLayout();
	}
}

void axUIView::onUpdateLayout() {
	if (_layoutHandler) {
		_layoutHandler->onUpdateLayout(this);
	}
}

void axUIView::onMeasureLayout(axUIMeasureRequest& req) {
	if (_layoutHandler) {
		_layoutHandler->onMeasureLayout(this, req);
	}
}

void axUIView::setLayoutResultRect(const Rect2& rc) {
	if (layout.excludeFromLayout()) {
		AX_ASSERT(false);
		return;
	}

	auto result = rc;
	auto& mr = layout.measured();
	using M = axUILayoutMode;

	switch (layout.modeW()) {
		case M::Fixed:		result.w = _rect.w;		break;
		case M::FitContent:	result.w = mr.min.x;	break;
		case M::Expand:		result.w = ax_clamp(rc.w, mr.min.x, mr.max.x); break;
		default: throw axError_Undefined(AX_LOC);
	}

	switch (layout.modeH()) {
		case M::Fixed:		result.h = _rect.h;		break;
		case M::FitContent:	result.h = mr.min.y;	break;
		case M::Expand:		result.h = ax_clamp(rc.h, mr.min.y, mr.max.y); break;
		default: throw axError_Undefined(AX_LOC);
	}

	{ // align X
		auto remain = rc.w - result.w;
		switch(layout.align().x) {
			case AlignX::Left:	 break;
			case AlignX::Center: result.x += ax_floor(remain / 2); break;
			case AlignX::Right:	 result.x += remain;               break;
		}
	}

	{ // align Y
		auto remain = rc.h - result.h;
		switch(layout.align().y) {
			case AlignY::Top:	 break;
			case AlignY::Center: result.y += ax_floor(remain / 2); break;
			case AlignY::Bottom: result.y += remain;               break;
		}
	}

	setRect(result);
}

axUIView::Layout::Layout() 
	: _excludeFromLayout(false)
{
}

const axUIMeasureRequest::Result& axUIView::Layout::measured() const {
	if (_view->_needToMeasure) {
		_doMeasure();
	}
	return _measured;
}

axUIView::Layout& axUIView::Layout::setModeW(axUILayoutMode m) & {
	if (_modeW != m) {
		_modeW = m;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout& axUIView::Layout::setModeH(axUILayoutMode m) & {
	if (_modeH != m) {
		_modeH = m;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout& axUIView::Layout::setMode(axUILayoutMode w, axUILayoutMode h)& {
	setModeW(w);
	setModeH(h);
	return *this;
}

// Mode

axUIView::Layout& axUIView::Layout::setFixedW()& { setModeW(axUILayoutMode::Fixed); return *this; }
axUIView::Layout& axUIView::Layout::setFixedH()& { setModeH(axUILayoutMode::Fixed); return *this; }
axUIView::Layout& axUIView::Layout::setExpandW()& { setModeW(axUILayoutMode::Expand); return *this; }
axUIView::Layout& axUIView::Layout::setExpandH()& { setModeH(axUILayoutMode::Expand); return *this; }
axUIView::Layout& axUIView::Layout::setFitContentW()& { setModeW(axUILayoutMode::FitContent); return *this; }
axUIView::Layout& axUIView::Layout::setFitContentH()& { setModeH(axUILayoutMode::FitContent); return *this; }

axUIView::Layout& axUIView::Layout::setMinW(float v)& {
	if (_min.x != v) {
		_min.x = v;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout&  axUIView::Layout::setMinH(float v) &{
	if (_min.y != v) {
		_min.y = v;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout& axUIView::Layout::setMin(float w, float h)& { setMinW(w); setMinH(h); return *this; }
axUIView::Layout& axUIView::Layout::setMin(const Vec2& v)& { setMin(v.x, v.y); return *this; }

axUIView::Layout&  axUIView::Layout::setMaxW(float v) & {
	if (_max.x != v) {
		_max.x = v;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout&  axUIView::Layout::setMaxH(float v) & {
	if (_max.y != v) {
		_max.y = v;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout& axUIView::Layout::setMax(float w, float h)& { setMaxW(w); setMaxH(w); return *this; }

axUIView::Layout& axUIView::Layout::setMax(const Vec2& v)& { setMax(v.x, v.y); return *this; }

axUIView::Layout&  axUIView::Layout::setNoMaxW() & {
	if (_max.x != ax_inf) {
		_max.x = ax_inf;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout&  axUIView::Layout::setNoMaxH() & {
	if (_max.y != ax_inf) {
		_max.y = ax_inf;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout& axUIView::Layout::setNoMax()& { setNoMaxW(); setNoMaxH(); return *this; }

axUIView::Layout&  axUIView::Layout::setExpandProportionW(float v) & {
	if (_expandProportion.x != v) {
		_expandProportion.x = v;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout&  axUIView::Layout::setExpandProportionH(float v) & {
	if (_expandProportion.y != v) {
		_expandProportion.y = v;
		_view->setNeedToMeasure();
	}
	return *this;
}

axUIView::Layout& axUIView::Layout::setExpandProportion(float w, float h)& { setExpandProportionW(w); setExpandProportionH(h); return *this; }
axUIView::Layout& axUIView::Layout::setExpandProportion(const Vec2& v)& { setExpandProportion(v.x, v.y); return *this; }
axUIView::Layout& axUIView::Layout::setCell(axInt x, axInt y)& { setCellX(x); setCellY(y); return *this; }
axUIView::Layout& axUIView::Layout::setCellX(axInt val)& { ax_assert_safe_assign(_cellX, val); return *this; }
axUIView::Layout& axUIView::Layout::setCellY(axInt val)& { ax_assert_safe_assign(_cellY, val); return *this; }
axUIView::Layout& axUIView::Layout::setCellSpan(axInt x, axInt y)& { setCellSpanX(x); setCellSpanY(y); return *this; }
axUIView::Layout& axUIView::Layout::setCellSpanX(axInt val)& { ax_assert_safe_assign(_cellSpanX, val); return *this; }
axUIView::Layout& axUIView::Layout::setCellSpanY(axInt val)& { ax_assert_safe_assign(_cellSpanY, val); return *this; }

axUIView::Layout&  axUIView::Layout::setAlign(const Align& v) & {
	setAlignX(v.x);
	setAlignY(v.y);
	return *this;
}

axUIView::Layout&  axUIView::Layout::setAlignX(AlignX v) & {
	if (_align.x != v) {
		_align.x = v;
		_view->setNeedToLayout();
	}
	return *this;
}

axUIView::Layout&  axUIView::Layout::setAlignY(AlignY v) & {
	if (_align.y != v) {
		_align.y = v;
		_view->setNeedToLayout();
	}
	return *this;
}

void axUIView::Layout::_doMeasure() const {
	for (auto& c : _view->children()) {
		c.layout.measured();
	}

	axUIMeasureRequest req;

	if (!isFixedW() || !isFixedH()) {
		_view->onMeasureLayout(req);
		auto pad = _view->_padding.total();
		req.result.min += pad;
		req.result.max += pad;
	}

	auto currentSize = _view->size();

	if (isFixedW()) {
		req.result.min.x = currentSize.x;
		req.result.max.x = currentSize.x;

	} else {
		ax_max_it(req.result.min.x, _min.x);
		req.result.max.x = isExpandW() ? _max.x : req.result.min.x;
	}

	if (isFixedH()) {
		req.result.min.y = currentSize.y;
		req.result.max.y = currentSize.y;

	} else {
		ax_max_it(req.result.min.y, _min.y);
		req.result.max.y = isExpandH() ? _max.y : req.result.min.y;
	}

	ax_const_cast(this)->_measured = req.result;
	ax_const_cast(_view)->_needToMeasure = false;
}
