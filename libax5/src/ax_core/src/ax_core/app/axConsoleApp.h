#pragma once
#include "axApp.h"

class axConsoleApp : public axApp {
	AX_RTTI_CLASS(axConsoleApp, axApp)
public:

	static void setConsoleTitle(axStrView title);
};
