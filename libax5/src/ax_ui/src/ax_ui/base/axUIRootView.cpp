#include "axUIRootView.h"

axUIRootView::axUIRootView() : Base(NullParent()) {
	setLayoutHandler(nullptr);
	layout.setFixedW();
	layout.setFixedH();
}

