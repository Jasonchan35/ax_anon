#pragma once

#include "../EditorUI/EdUIObjectInspector.h"

namespace Anon{ namespace Editor {

class SCN_Object_Inspector : public EdUIObjectInspector {
	AX_RTTI_CLASS(SCN_Object_Inspector, EdUIObjectInspector)
public:
	SCN_Object_Inspector(CreateDesc& desc);
	virtual void onReloadContent() override;

private:
	void _addComponent(const axType* type);

	EdUIButton	_addComponentButton;
};

}} //namespace