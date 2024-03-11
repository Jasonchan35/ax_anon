#pragma once
#include "../EdUICommon.h"
#include "../../Core/AnonObjectBinding.h"

namespace Anon { namespace Editor {


class EdUIField : public axUIBoxView {
	AX_RTTI_CLASS(EdUIField, axUIBoxView)
public:
	class CreateDesc {
	public:
		axUIView*			parent = nullptr;
		const axFieldInfo*	fieldInfo = nullptr;
		axStrView			fieldPath;
		AnonObjectBinding*	objectBinding = nullptr;
		axInt				level = 0;
	};

	EdUIField(CreateDesc& desc);
	AnonObjectBinding*	objectBinding() { return _objectBinding.ptr(); }

	BoxView&	fieldBox() { return _fieldBox; }

protected:
	BoxView		_labelBox;
	BoxView		_fieldBox;
	axUILabel	_label;

	const axFieldInfo*			_fieldInfo = nullptr;
	axString					_fieldPath;
	axLink<AnonObjectBinding>	_objectBinding;
};

template<class T>
class EdUIField_ : public EdUIField {
	using Base = EdUIField;
public:
	using FieldView = T;

	EdUIField_(CreateDesc& desc) 
		: Base(desc)
		, _field(&fieldBox())
	{
		_field.layout.setExpandW();
	}
protected:
	T	_field;
};

}} //namespace

