#include "EdUISliderField.h"

namespace Anon { namespace Editor {

template<class T>
EdUISliderField<T>::EdUISliderField(CreateDesc& desc)
	: Base(desc)
{
	_field.evSetValue.bind(this, [this](T& newValue) {
		if (auto* ob = objectBinding()) {
			ob->template setValue<T>(_fieldPath, newValue, false);
		}
	});

	_field.evSetEditingValue.bind(this, [this](T& newValue) {
		if (auto* ob = objectBinding()) {
			ob->template setValue<T>(_fieldPath, newValue, true);
		}
	});

	auto* info = desc.fieldInfo;
	AX_ASSERT(info->type() == ax_typeof<T>());

	auto* attr = info->template findAttr<SliderAttribute>();
	if (!attr) throw axError_Undefined(AX_LOC);

	_field.setMin(static_cast<T>(attr->min()));
	_field.setMax(static_cast<T>(attr->max()));
}

template<class T>
void EdUISliderField<T>::onReloadContent() {
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
	template class EdUISliderField<T>; \
//-------------
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
	AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
#undef AX_MACRO_OP

}}
