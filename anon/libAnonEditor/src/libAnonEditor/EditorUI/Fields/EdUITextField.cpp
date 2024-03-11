#include "EdUITextField.h"

namespace Anon { namespace Editor {

EdUITextField::EdUITextField(CreateDesc& desc)
	: Base(desc)
{
	_field.evSetValue.bind(this, &This::_onSetValue);
}

void EdUITextField::_onSetValue(axStrViewU newValue) {
	auto* ob = objectBinding();
	if (!ob) return;

	auto tmp = axTempString::s_convert(newValue);
	ob->setValue(_fieldPath, tmp, false);
}

void EdUITextField::onReloadContent() {
//	Base::onReloadContent();

	auto* ob = objectBinding();
	if (!ob) return;

	auto result = ob->getValue<axString>(_fieldPath);

	if (!result.value) {
		_field.setNoValue();
		return;
	}

	if (result.isMultipleValue) {
		_field.setIsMultipleValue();
		return;
	}

	_field.setText(*result.value);
}

}}
