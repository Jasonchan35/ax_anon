#pragma once
#include "EdUIField.h"

namespace Anon { namespace Editor {

class EdUIStructField : public EdUIField {
	AX_RTTI_CLASS(EdUIStructField, EdUIField)
public:
	EdUIStructField(CreateDesc& desc);
};

}}