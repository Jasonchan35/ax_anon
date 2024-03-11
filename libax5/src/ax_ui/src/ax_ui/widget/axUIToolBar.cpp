#include "axUIToolBar.h"

axUIToolBar::axUIToolBar(axUIView* parent)
	: Base(parent)
{
	layout.setFitContentH();
	setBgColor(.2f, .2f, .2f);
}
