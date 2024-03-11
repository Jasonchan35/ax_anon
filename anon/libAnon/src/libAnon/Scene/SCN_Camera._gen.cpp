//==============================================================//
//                                                              //
//       This file is generated by axNodeGen                    //
//                                                              //
//==============================================================//

#include "SCN_Camera.h"

namespace Anon{ 

struct SCN_Camera::Spec : public AnonObjectSpec {
	static const Spec* s_instance;

	axArray<axFieldInfo*, 16> _fields;

	Spec(axType* type)
		: AnonObjectSpec(type)
	{
		s_instance = this;
		axInt ownFieldCount = 0;
		auto baseFieldCount = type->_baseClass->fields().size();
		_fields.resize(ownFieldCount + baseFieldCount);
		_fields.slice(0, baseFieldCount).copyValues(type->_baseClass->fields());
		auto ownFields = _fields.sliceFrom(baseFieldCount);


		type->_fields    = _fields.span();
		type->_ownFields = ownFields;
	}

};

void SCN_Camera::s_initRtti(axType* type) {
	static Spec spec(type);
}

const SCN_Camera::Spec* SCN_Camera::Spec::s_instance = nullptr;

void SCN_Camera::onInitProps() {
	Base::onInitProps();
}

bool SCN_Camera::onReadJsonProp(axJsonReader& rd_) {
	bool _ret = Base::onReadJsonProp(rd_);
	return _ret;
}

void SCN_Camera::onWriteJsonProp(axJsonWriter& wr_) const {
	Base::onWriteJsonProp(wr_);
}

} // namespace 
