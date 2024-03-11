#pragma once

#include "axUILayout.h"

class axUIDefaultLayoutHandler : public axUILayoutHandler {
	AX_RTTI_CLASS(axUIDefaultLayoutHandler, axUILayoutHandler)
public:
	static axUIDefaultLayoutHandler* s_instance();

	virtual void onUpdateLayout	(axUIView* view) override;
	virtual void onMeasureLayout	(axUIView* view, axUIMeasureRequest& req) override;
};
