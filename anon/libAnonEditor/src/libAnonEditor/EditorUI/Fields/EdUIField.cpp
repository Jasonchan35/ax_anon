#include "EdUIField.h"

namespace Anon { namespace Editor {

EdUIField::EdUIField(CreateDesc& desc)
	: Base(desc.parent) 
	, _labelBox(this)
	, _fieldBox(this)
	, _label(&_labelBox)
{
//	boxLayout.setSpacing(8);
	layout.setFitContentH();
	_fieldBox.layout.setExpandProportionW(4);

	setDebugName(desc.fieldInfo->name());
	_fieldInfo = desc.fieldInfo;
	_fieldPath = desc.fieldPath;
	_objectBinding = desc.objectBinding;

	if (desc.fieldInfo) {
		_label.setText(desc.fieldInfo->name());
	}
	Margin2 padding(2);
	padding.left += desc.level * 16.0f;
	_label.setPadding(padding);
	_label.layout.setExpandW();
}

}}

