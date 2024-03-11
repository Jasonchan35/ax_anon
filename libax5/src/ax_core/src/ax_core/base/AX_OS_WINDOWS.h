#pragma once

#if AX_OS_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1 // Exclude rarely-used stuff from Windows headers
#endif

#ifndef NOMINMAX
	#define NOMINMAX 1 // no min and max macro in windows.h
#endif

#ifdef TEXT
	#undef TEXT
#endif

#pragma warning( push )
#pragma warning( disable : 4668) //directive is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'

#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
#include <ws2tcpip.h> // struct sockaddr_in6
#include <Windows.h>
#include <Mmsystem.h>
#include <time.h>
#include <conio.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h> 
#include <Winsvc.h>
#include <comdef.h> //Com error

#if AX_TRY_USE_OPENGL
	#define ax_HAS_GLEW		1
	#define GLEW_STATIC		1

	#include <ax_core/glew/glew.h>
	#include <ax_core/glew/wglew.h>

	#include <GL/gl.h>
	#include <GL/glu.h>
#endif	

#pragma warning( pop )

#define AX_USE_Win32Api		1

#undef	min
#undef	max
#undef	RGB

#define ax_EXE_SUFFIX ".exe"
#define	ax_SO_SUFFIX  ".dll"

#define AX_TYPE_HAS_LONG		1
//#define AX_TYPE_HAS_LONGLONG	1

class axWin32_Handle {
public:
	axWin32_Handle(const axWin32_Handle&) = delete;
	void operator=(const axWin32_Handle&) = delete;

	axWin32_Handle(HANDLE h = INVALID_HANDLE_VALUE) : _h(h) {}
	~axWin32_Handle() { unref(); }

	void set(HANDLE h) { unref(); _h = h; }
	void unref() { if(isValid()) {::CloseHandle(_h); _h = INVALID_HANDLE_VALUE; } }

	HANDLE	get() { return _h; }
	operator HANDLE() { return _h; }

	bool isValid() const { return _h != INVALID_HANDLE_VALUE; }

private:
	HANDLE _h;
};

#endif // #if AX_OS_WINDOWS