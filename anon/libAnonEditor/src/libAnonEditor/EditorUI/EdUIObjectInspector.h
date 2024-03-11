#pragma once

#include "Fields/EdUITextField.h"

namespace Anon { namespace Editor {

class EdUIObjectInspector : public axUIVBoxView {
	AX_RTTI_CLASS(EdUIObjectInspector, axUIVBoxView);
public:
	using ObjectInspector = EdUIObjectInspector;
	struct CreateDesc {
		axUIView* parent = nullptr;
		const axType* objectType = nullptr;
	};

	EdUIObjectInspector(CreateDesc& desc);

	const axType* objectType() const;

	AnonObjectBinding& objectBinding() { return _objectBinding; }

	void addObject(AnonObject* object);

	axEvent<void (AnonObject*)>	evRelatedObject;

protected:
	virtual void onAddField(EdUIField::CreateDesc& desc);
	virtual void onReloadContent() override;

private:
	void _onObjectValueChanged();
	void _onOptionButton(axUIButtonEvent& ev);

	void _addFields(const axType* type);
	const axType*		_objectType = nullptr;
	AnonObjectBinding	_objectBinding;

	HBoxView		_titleBar;
	EdUILabel		_typeLabel;
	EdUIButton		_optionButton;

	HBoxView		_nameRow;
	EdUILabel		_nameLabel;
	EdUITextBox		_nameBox;

	VBoxView		_fieldsView;

	EdUILabel		_objectInfo;
};





}}