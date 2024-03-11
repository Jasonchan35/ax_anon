#pragma once

#include "../Base/axNativeUIApp_Base.h"

#if AX_USE_Win32Api

class axNativeUIApp_Win32 : public axNativeUIApp_Base {
	AX_RTTI_CLASS(axNativeUIApp_Win32, axNativeUIApp_Base)
public:
	ax_DOWNCAST_GET_INSTANCE()

	virtual int		onRun		() override;
	virtual	void	quit		(int returnCode) override;

	static axRect2f s_getVirtualDesktopRect();

	MSG _win32_msg;

protected:
	virtual bool	onSetNativeCursor(axUICursorType type) override;
	virtual bool	onSetNativeCursorPos(const axVec2f& worldPos) override;
	virtual bool	onRequestNativeCustomAppEvent() override;

private:
	void _createHiddenWindow();
	static LRESULT WINAPI s_hiddenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	#define AX_MACRO_OP(E, VALUE) HCURSOR _cursor##E = nullptr;
		axUICursorType_EnumList(AX_MACRO_OP)
	#undef AX_MACRO_OP

	HWND _hiddenWindow = nullptr;
};

AX_INLINE axVec2f axWin32_LPARAM_to_Vec2f(LPARAM lp) {
	return axVec2f((float)(short)LOWORD(lp), (float)(short)HIWORD(lp));
}

#endif
