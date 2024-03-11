#pragma once
#include "EdUIField.h"

namespace Anon { namespace Editor {

template<class T>
class EdUINumberField : public EdUIField_< axUINumberBox<T> > {
	AX_RTTI_CLASS(EdUINumberField<T>, EdUIField_< axUINumberBox<T> >)
public:
	using Value = T;
	using typename Base::CreateDesc;
	using Base::_field;
	using Base::_fieldPath;
	using Base::objectBinding;

	EdUINumberField(CreateDesc& desc);
	virtual void onReloadContent() override;
};

}}
