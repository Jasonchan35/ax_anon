#pragma once

#if AX_USE_X11

// !! required package ----------------------------------------------------
//	ubuntu: libglu1-mesa-dev
//	cygwin: libGLU

#include "AX_X11_define.h"
#include "AX_X11_undef.h"

template<class T> class axVec2;
using axVec2f = axVec2<float>;

template<class T> class axRect2;
using axRect2f = axRect2<float>;

class axNativeUIKeyEvent;
class axNativeUIMouseEvent;

//undefine common word defined from <X11/X.h> or <GL/glx.h>


#endif //AX_USE_X11
