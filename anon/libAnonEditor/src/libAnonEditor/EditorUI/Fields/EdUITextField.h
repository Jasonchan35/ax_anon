#pragma once
#include "EdUIField.h"

namespace Anon { namespace Editor {

class EdUITextField : public EdUIField_< EdUITextBox > {
	AX_RTTI_CLASS(EdUITextField, EdUIField_< EdUITextBox >)
public:
	using typename Base::CreateDesc;
	using Base::_field;
	using Base::_fieldPath;
	using Base::objectBinding;

	EdUITextField(CreateDesc& desc);
	virtual void onReloadContent() override;

private:
	void _onSetValue(axStrViewU newValue);
};

}}
