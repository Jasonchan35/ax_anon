#pragma once

#include <ax_core/other/axObject.h>
#include <ax_core/math/axRect2.h>
#include <ax_core/native_ui/axNativeUIEvent.h>

class axUIRenderView;

struct axNativeUIWindow_CreateDesc {
	axNativeUIWindow_CreateDesc()
		: rect(100, 100, 8, 8)
		, titleBar(true)
		, isMainWindow(false)
		, visible(true)
		, resizable(true)
		, closeButton(true)
		, minButton(true)
		, maxButton(true)
		, centerToScreen(true)
		, alwaysOnTop(false)
	{}

	enum class Type {
		Unknown,
		NormalWindow,
		ToolWindow,
		PopupWindow,
	};

	axRect2f	rect {0};
	Type		type = Type::NormalWindow;
	bool		titleBar		: 1;
	bool		isMainWindow	: 1;
	bool		visible			: 1;
	bool		resizable		: 1;
	bool		closeButton		: 1;
	bool		minButton		: 1;
	bool		maxButton		: 1;
	bool		centerToScreen	: 1;
	bool		alwaysOnTop		: 1;
};

class axNativeUIWindow_Base : public axObject {
	AX_RTTI_CLASS(axNativeUIWindow_Base, axObject);
public:
	using Vec2    = axVec2f;
	using Rect2   = axRect2f;
	using Margin2 = axMargin2f;

	using CreateDesc = axNativeUIWindow_CreateDesc;

	axNativeUIWindow_Base(CreateDesc& desc);

	virtual void onCloseButton	() = 0;

			void setActive		(bool b);
	virtual void onSetNativeActive	(bool b) = 0;
	virtual void onActive		(bool b)				{ _active = b; }

			void setVisible		(bool b);
	virtual void onSetNativeVisible	(bool b) = 0;

			void setWorldPos		(const Vec2& pos);
			void setWorldPos		(float x, float y)	{ setWorldPos(axVec2f(x,y)); }
	virtual void onSetNativeWorldPos(const Vec2& pos) = 0;
	virtual void onSetWorldPos		(const Vec2& pos)	{ _worldRect.pos  = pos; }

	const Rect2&	worldRect		() const		{ return _worldRect; }
	const Vec2&		worldPos		() const		{ return _worldRect.pos; }
	const Margin2&	padding			() const		{ return _padding; }
		  Rect2		contentWorldRect() const		{ return _worldRect - _padding; }
		  Vec2		contentWorldPos	() const		{ return _worldRect.pos + _padding.topLeft(); }

			void setSize		(const Vec2& size);
			void setSize		(float w, float h)		{ setSize(Vec2(w,h)); }
	virtual void onSetNativeSize(const Vec2& size) = 0;
	virtual void onSetSize		(const Vec2& size)	{ _worldRect.size = size; }

			void setWorldRect	(const Rect2& rect)	{ setWorldPos(rect.pos); setSize(rect.size); }
			void setPadding		(const Margin2& m)	{ _padding = m; }

			//! same as click close button by user
			void closeWindow	() { doCloseWindow(); }
	virtual void doCloseWindow	() = 0;
	
			void destroyWindow	() { doDestroyWindow(); }
	virtual void doDestroyWindow() = 0;

			void setWindowTitle		(axStrView title);
	virtual void onSetNativeWindowTitle	(axStrView title) = 0;

protected:
	void _onNativeUIMouseEvent(axNativeUIMouseEvent& ev);
	void _onNativeUIKeyEvent(axNativeUIKeyEvent& ev);

	virtual	void onNativeUIMouseEvent(axNativeUIMouseEvent& ev) {}
	virtual	void onNativeUIKeyEvent(axNativeUIKeyEvent& ev) {}
	virtual	void onRender() {}
	
	axEnterOnce _re_setActive;
	axEnterOnce	_re_setSize;
	axEnterOnce	_re_setPos;

private:
	Rect2		_worldRect {0};
	Margin2		_padding {0};
	bool		_active : 1;
};

