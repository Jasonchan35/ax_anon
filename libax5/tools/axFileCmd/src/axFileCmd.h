#pragma once

#include <ax_core.h>

class axFileCmd : public axConsoleApp {
public:
	void showHelp();
	int onRun() override;
};
