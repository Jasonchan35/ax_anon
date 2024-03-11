#include "EdUINumberField.h"

namespace Anon { namespace Editor {

template<class T>
EdUINumberField<T>::EdUINumberField(CreateDesc& desc)
	: Base(desc)
{
	_field.evSetValue.bind(this, [this](T& newValue){
		if (auto* ob = objectBinding()) {
			ob->template setValue<T>(_fieldPath, newValue, false);
		}
	});

	_field.evSetEditingValue.bind(this, [this](T& newValue) {
		if (auto* ob = objectBinding()) {
			ob->template setValue<T>(_fieldPath, newValue, true);
		}
	});
}

template<class T>
void EdUINumberField<T>::onReloadContent() {
//	Base::onReloadContent();

	auto* ob = objectBinding();
	if (!ob) return;

	auto result = ob->template getValue<T>(_fieldPath);

	if (!result.value) {
		_field.setNoValue();
		return;
	}

	if (result.isMultipleValue) {
		_field.setIsMultipleValue();
		return;
	}

	_field.setValue(*result.value, false);
}


#define	AX_MACRO_OP(T) \
	template class EdUINumberField<T>; \
//-------------
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
	AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
#undef AX_MACRO_OP

}}
