#include "EdUIModule.h"
#include "Fields/EdUINumberField.h"
#include "Fields/EdUISliderField.h"
#include "Fields/EdUITextField.h"
#include "Fields/EdUIStructField.h"

namespace Anon { namespace Editor {

void EdUIModule::create() {
	_fieldFactory.addEntry<axIString, EdUITextField>();
	#define AX_MACRO_OP(T) \
		_fieldFactory.addEntry<T, EdUINumberField<T>>(); \
		_sliderFieldFactory.addEntry<T, EdUISliderField<T>>(); \
	//-----
		AX_TYPE_LIST_SINT(AX_MACRO_OP)
		AX_TYPE_LIST_UINT(AX_MACRO_OP)
		AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
	#undef AX_MACRO_OP
}

EdUIField* EdUIModule::createField(const axSourceLoc& loc, axAllocator* allocator, EdUIField::CreateDesc& desc) {
	if (!desc.fieldInfo) {
		throw axError_Undefined(AX_LOC);
	}

	EdUIField* o = nullptr;
	if (desc.fieldInfo->findAttr<SliderAttribute>()) {
		o = _sliderFieldFactory.create(loc, allocator, desc.fieldInfo->type(), desc).detach();
	}
	
	if (!o) {
		o = _fieldFactory.create(loc, allocator, desc.fieldInfo->type(), desc).detach();
	}

	if (!o) {
		o = new(loc, allocator) EdUIStructField(desc);
	}
	return o;
}

EdUIObjectInspector* EdUIModule::createObjectInspector(const axSourceLoc& loc, axAllocator* allocator, EdUIObjectInspector::CreateDesc& desc) {
	if (auto o = _objectViewFactory.create(loc, allocator, desc.objectType, desc)) {
		return o.detach();
	}
	return new(loc, allocator) EdUIObjectInspector(desc);
}

EdUIDiaNode* EdUIModule::createDiaNode(const axSourceLoc& loc, axAllocator* allocator, EdUIDiaNode::CreateDesc& desc) {
	if (auto o = _diaNodeFactory.create(loc, allocator, ax_typeof(desc.diaNode), desc)) {
		return o.detach();
	}
	return new(loc, allocator) EdUIDiaNode(desc);
}

}}


