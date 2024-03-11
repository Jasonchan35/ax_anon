#pragma once

#include "axUICommon.h"
#include "axUIScreen.h"
#include <ax_ui/input/axUIEvent.h>
#include <ax_ui/input/axUIInputManager.h>

class axUILayoutHandler;
class axUIRenderContextView;
class axUIWindow;
class axUIBoxView;
class axUIHBoxView;
class axUIVBoxView;

class axUIView : public axObject, protected axDListNode<axUIView> {
	AX_RTTI_CLASS(axUIView, axObject)
public:
	friend class axUIInputManager;
	friend class axDList<axUIView>;

	using Vec2 = axVec2f;
	using Vec3 = axVec3f;
	using Vec4 = axVec4f;
	using Mat4 = axMat4f;
	using Rect2 = axRect2f;
	using BBox2 = axBBox2f;
	using Margin2 = axMargin2f;
	using Color = axColor;
	using Align  = axUIAlign;
	using AlignX = axUIAlignX;
	using AlignY = axUIAlignY;

	using BoxView  = axUIBoxView;
	using HBoxView = axUIHBoxView;
	using VBoxView = axUIVBoxView;

	class NullParent {};
	virtual ~axUIView();

	void setNeedToDestroy();

			axUIView(const NullParent&);
			axUIView(axUIView* parent);

			void		setEnable	(bool b)	{ onSetEnable(b); }
	virtual void		onSetEnable	(bool b);
			bool		enable		() const	{ return _enable; }

			void		setVisible	(bool b)	{ onSetVisible(b); }
	virtual void		onSetVisible(bool b);
			bool		visible		() const	{ return _visible; }

			void		setBgColor		(const Color& c);
			void		setBgColor		(float r, float g, float b, float a = 1) { setBgColor(Color(r,g,b,a)); }
	const Color&		bgColor			() const		{ return _bgColor; }

			void		setBorderColor	(const Color& c);
			void		setBorderColor	(float r, float g, float b, float a = 1) { setBorderColor(Color(r,g,b,a)); }
	const Color&		borderColor		() const		{ return _borderColor; }

			void		setBorderWidth	(const Margin2& m);
			void		setBorderWidth	(float f) { setBorderWidth(Margin2(f)); }
	const Margin2		borderWidth		() const		{ return _borderWidth; }

			void		setDebugDrawRect(bool b);
			bool		debugDrawRect() const			{ return _debugDrawRect; }

			void		dump(axInt level = 0);
	virtual	void		onFormat(axFormat& f) const;

			void		reloadContent	();
	virtual	void		onReloadContent	();

	virtual	void		onLinkedObjectDestroy(axObject* obj) override;

			axObject*	setLinkedObject(axObject* obj);
			axObject*	linkedObject() { return _linkedObject.ptr(); }
			
			template<class R>
			R*			linkedObject() { 
				if (_linkedObject) {
					auto* r = ax_type_cast<R>(_linkedObject.ptr());
					AX_ASSERT_MSG(r, "invalid type in linkedObject");
					return r;
				}
				return nullptr;
			}

#if 0
#pragma mark ------------ Position --------------
#endif
	const	Rect2&		rect		() const { return _rect; }
			Rect2		localRect	() const { return Rect2(0, 0, _rect.w, _rect.h); }

			Rect2		contentRect	() const { return localRect() - _padding; }
			Vec2		contentSize () const { return contentRect().size; }

			Rect2		rectOnWindow() const;

			void		moveWithinWorldRect(const Rect2& withinRect);

	const	Vec2&		pos			() const { return _rect.pos; }
	const	Vec2&		size		() const { return _rect.size; }
			float		width		() const { return _rect.w; }
			float		height		() const { return _rect.h; }

	const	Vec2&		worldPos	() const;
			Rect2		worldRect	() const { return Rect2(worldPos(), size()); }

			void		setWorldPos	(const Vec2& v);
			void		setWorldRect(const Rect2& rect)		{ setWorldPos(rect.pos); setSize(rect.size); }
			void		setWorldPosDirty();

			void		setRect		(const Rect2& v)		{ setPos(v.pos); setSize(v.size); }
			void		setRect		(float x, float y, float w, float h) { setRect(Rect2(x,y,w,h)); }

			void		setPos		(const Vec2& v);
			void		setPos		(float x, float y)		{ setPos(Vec2(x,y)); }

			void		offsetPos	(const Vec2& offset)	{ setPos(pos() + offset); }

			void		setSize		(const Vec2& v);
			void		setSize		(float w, float h)		{ setSize(Vec2(w,h)); }

			void		setWidth	(float v)				{ setSize(Vec2(v, _rect.h)); }
			void		setHeight	(float v)				{ setSize(Vec2(_rect.w, v)); }

			void		setFixedWidth	(float v)			{ layout.setFixedW(); setWidth(v); }
			void		setFixedHeight	(float v)			{ layout.setFixedH(); setHeight(v); }

	const Margin2&		padding		() const { return _padding; }	
			void		setPadding	(const Margin2& m);
			Rect2		getChildrenBounds();
#if 0
#pragma mark ------------ Parent / Child --------------
#endif
			axUIView*	parent()			{ return _parent; }
			void		setParent			(axUIView* newParent);

			axUIScreen	uiScreen();
			axUIWindow*	uiWindow			() { return _uiWindow; }
axUIRenderContextView*	uiRenderContextView	() { return _uiRenderContextView; }

			void		clearChildren		();

			void		setAsFirstSibling	();
			void		setAsLastSibling	();

			void		setClipContent		(bool b) { _clipContent = b; }
			bool		clipContent			() const { return _clipContent; }

			template<class R>
			R*			findParentOfType	();

			axInt		childCount			() const { return _children.size(); }
	axDList<This>::Enumerator	children()	{ return _children.each(); }

#if 0
#pragma mark ------------ Render --------------
#endif
			void		setNeedToRender		();
			void		render				(axRenderRequest& req);
protected:
	virtual void		onSetNeedToRender	();
	virtual void		onRender			(axRenderRequest& req);
	virtual void		onRenderChildren	(axRenderRequest& req);
public:

#if 0
#pragma mark ------------ Keyboard Input --------------
#endif
			void		handleKeyEvent	(axUIKeyEvent& ev);

			void		setFocus			(bool b);
			bool		hasFocus			() const		{ return _hasFocus; }
			void		setAcceptFocus		(bool b);
			bool		acceptFocous		() const		{ return _acceptFocus; }

			void		forwardEventTo		(axUIView* target, axUIKeyEvent& ev);
			void		forwardEventToParent(axUIKeyEvent& ev);

protected:
	virtual void		onKeyEvent			(axUIKeyEvent& ev);
	virtual void		onSetFocus			(bool b) {}
public:

#if 0
#pragma mark ------------ Mouse / Touch Input --------------
#endif

			bool		hitTest				(axUIHitTest& ht);
			bool		hitTestSelf			(axUIHitTest& ht);
			bool		hitTestChildren		(axUIHitTest& ht);

			Rect2		hitTestRect			() const				{ return localRect() + _hitTestMargin; }

			Margin2		hitTestMargin		() const				{ return _hitTestMargin; }
			void		setHitTestMargin	(const Margin2& v)		{ _hitTestMargin = v; }
			void		setHitTestMargin	(float v)				{ setHitTestMargin(Margin2(v)); }
			void		setHitTestMargin	(float top, float right, float bottom, float left) { setHitTestMargin(Margin2(top, right, bottom, left)); }

			void		forwardEventToParent(axUIMouseEvent& ev);
			void		forwardEventTo		(axUIView* target, axUIMouseEvent& ev);

			bool		isMouseHover		() const		{ return _isMouseHover; }

			void		setDraggable		(bool b);
			bool		draggable			() const		{ return _draggable; }

			void		setWantMouseWheelEvent(bool b)		{ _wantMouseWheelEvent = b; }
			bool		wantMouseWheelEvent() const			{ return _wantMouseWheelEvent; }

			void		setWantMouseDragEvent(bool b)		{ _wantMouseDragEvent = b; }
			bool		wantMouseDragEvent() const			{ return _wantMouseDragEvent; }

			void		handleMouseEvent	(axUIMouseEvent& ev);

protected:
	virtual bool		onHitTest			(axUIHitTest& ht);
	virtual bool		onHitTestSelf		(axUIHitTest& ht);

	virtual void		onMouseEvent		(axUIMouseEvent& ev) { forwardEventToParent(ev); }

private:
			void		_setMouseHover(bool b);
public:

#if 0
#pragma mark ------------ Layout--------------
#endif
			class Layout {
			public:
				Layout();

				const axUIMeasured&			measured() const;

				Layout&		setModeW		(axUILayoutMode m) &;
				Layout&		setModeH		(axUILayoutMode m) &;
				Layout&		setMode			(axUILayoutMode w, axUILayoutMode h) &;

				axUILayoutMode	modeW		() const			{ return _modeW; }
				axUILayoutMode	modeH		() const			{ return _modeH; }

			// Mode
				Layout&		setFixedW		() &;
				Layout&		setFixedH		() &;
				bool		isFixedW		() const			{ return _modeW == axUILayoutMode::Fixed; }
				bool		isFixedH		() const			{ return _modeH == axUILayoutMode::Fixed; }

				Layout&		setExpandW		() &;
				Layout&		setExpandH		() &;
				bool		isExpandW		() const			{ return _modeW == axUILayoutMode::Expand; }
				bool		isExpandH		() const			{ return _modeH == axUILayoutMode::Expand; }

				Layout&		setFitContentW	() &;
				Layout&		setFitContentH	() &;
				bool		isFitContentW	() const			{ return _modeW == axUILayoutMode::FitContent; }
				bool		isFitContentH	() const			{ return _modeH == axUILayoutMode::FitContent; }

			// Min, Max
				const Vec2&	min				() const			{ return _min; }
				const Vec2&	max				() const			{ return _max; }

				Layout&		setMinW			(float v) &;
				Layout&		setMinH			(float v) &;
				Layout&		setMin			(float w, float h) &;
				Layout&		setMin			(const Vec2& v) &;

				Layout&		setMaxW			(float v) &;
				Layout&		setMaxH			(float v) &;
				Layout&		setMax			(float w, float h) &;
				Layout&		setMax			(const Vec2& v) &;

				Layout&		setNoMaxW		() &;
				Layout&		setNoMaxH		() &;
				Layout&		setNoMax		() &;
				
			//
				Layout&		setExpandProportionW(float v) &;
				Layout&		setExpandProportionH(float v) &;
				Layout&		setExpandProportion(float w, float h) &;
				Layout&		setExpandProportion(const Vec2& v) &;

			const Vec2&		expandProportion	() const			{ return _expandProportion; }

				bool		excludeFromLayout	() const			{ return _excludeFromLayout; }
				Layout&		setExcludeFromLayout(bool b) &			{ _excludeFromLayout = b; return *this; }

				axInt		cellX			() const				{ return _cellX; }
				axInt		cellY			() const				{ return _cellY; }

				Layout&		setCell			(axInt x, axInt y)&;
				Layout&		setCellX		(axInt val)&;
				Layout&		setCellY		(axInt val)&;

				axInt		cellSpanX		() const				{ return _cellSpanX; }
				axInt		cellSpanY		() const				{ return _cellSpanY; }

				Layout&		setCellSpan		(axInt x, axInt y)&;
				Layout&		setCellSpanX	(axInt val)&;
				Layout&		setCellSpanY	(axInt val)&;

				Layout&		setAlign		(const Align& v) &;
				Layout&		setAlignX		(AlignX v) &;
				Layout&		setAlignY		(AlignY v) &;

				const Align& align	() const	{ return _align; }

				Vec2		_tempSize {0};

			friend class axUIView;
			protected:
				axUIView*	_view = nullptr;
				Vec2		_min {0};
				Vec2		_max {ax_inf};
				Vec2		_expandProportion {1};
				axInt		_cellX = 0;
				axInt		_cellY = 0;
				axInt		_cellSpanX = 1;
				axInt		_cellSpanY = 1;
				Align		_align;

				axUILayoutMode	_modeW = axUILayoutMode::Expand;
				axUILayoutMode	_modeH = axUILayoutMode::Expand;

				bool		_excludeFromLayout : 1;

			private:
				void			_doMeasure() const;
				axUIMeasured	_measured;
			};

			Layout	layout;

			void		setNeedToLayout	();

			void		setNeedToMeasure();

			void		updateLayout	();
			void		updateChildrenLayout();

			void		setLayoutResultRect	(const Rect2& v);
			void		setLayoutHandler(axUILayoutHandler* p)	{ _layoutHandler = p; }


protected:
	virtual	void		onUpdateLayout	();
	virtual void		onMeasureLayout	(axUIMeasureRequest& req);
	
public:
	virtual void onGetDebugInfo(axIString& str);

#if 0
#pragma mark ---------- protected ---------------------------
#endif

protected:
	void _ctor();
	void _handleParentChanged();
	virtual void onParentChanged();

	virtual void		onSetPos	(const Vec2& v);
	virtual void		onSetSize	(const Vec2& v);

	Rect2				_rect {0};
	Vec2				_worldPosComputed {0};
	Margin2				_padding {0};
	Margin2				_hitTestMargin {0};
	Color				_bgColor {0,0,0,0};
	Color				_borderColor {0,0,0,0};
	Margin2				_borderWidth {1};

	axUIView*			_parent = nullptr;
	axUIWindow*			_uiWindow = nullptr;
	axUIRenderContextView*	_uiRenderContextView = nullptr;

	axDList<This>		_children;

	axLink<axObject>	_linkedObject;

	axUILayoutHandler*	_layoutHandler = nullptr;

	bool				_debugDrawRect				: 1;
	bool				_enable						: 1;
	bool				_visible					: 1;
	bool				_needToLayout				: 1;
	bool				_childrenNeedToLayout		: 1;
	bool				_needToMeasure				: 1;

	bool				_draggable					: 1;
	bool				_isMouseHover				: 1;
	bool				_wantMouseWheelEvent		: 1;
	bool				_wantMouseDragEvent			: 1;
	bool				_clipContent				: 1;
	bool				_worldPosDirty				: 1;
	bool				_hasFocus					: 1;
	bool				_acceptFocus				: 1;
	bool				_handling_parentChanged		: 1;

	axEnterOnce			_re_setSize;
	axEnterOnce			_re_setPos;
	axEnterOnce			_re_setNeedToRender;
	axEnterOnce			_re_updateLayout;
};

template<class R> inline
R* axUIView::findParentOfType() {
	auto* p = _parent;
	while (p) {
		if (auto* o = ax_type_cast<R>(p))
			return o;
		p = p->_parent;
	}
	return nullptr;
}

