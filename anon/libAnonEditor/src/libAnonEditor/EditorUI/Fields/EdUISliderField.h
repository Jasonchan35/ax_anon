#pragma once
#include "EdUIField.h"

namespace Anon { namespace Editor {

template<class T>
class EdUISliderField : public EdUIField_< axUISlider<T> > {
	AX_RTTI_CLASS(EdUISliderField<T>, EdUIField_< axUISlider<T> >)
public:
	using Value = T;
	using typename Base::CreateDesc;
	using Base::_field;
	using Base::_fieldPath;
	using Base::objectBinding;

	EdUISliderField(CreateDesc& desc);
	virtual void onReloadContent() override;
};

}}
