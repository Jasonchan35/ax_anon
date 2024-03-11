//==============================================================//
//                                                              //
//       This file is generated by axNodeGen                    //
//                                                              //
//==============================================================//

#include "GEO_Transform.h"

namespace Anon{ 

struct GEO_Transform::Spec : public AnonObjectSpec {
	struct Field_translate : public axFieldInfo {
		Field_translate(axType* type) : axFieldInfo(type, &GEO_Transform::_translate, "translate", "Translate") {
		}
		virtual void onSetToDefaultValue(axValuePtr& data) const final {
			data.value<Vec3d>() = Vec3d(0,0,0);
		};
	};
	Field_translate field_translate;

	struct Field_rotate : public axFieldInfo {
		Field_rotate(axType* type) : axFieldInfo(type, &GEO_Transform::_rotate, "rotate", "Rotate") {
		}
		virtual void onSetToDefaultValue(axValuePtr& data) const final {
			data.value<Vec3d>() = Vec3d(0,0,0);
		};
	};
	Field_rotate field_rotate;

	struct Field_scale : public axFieldInfo {
		Field_scale(axType* type) : axFieldInfo(type, &GEO_Transform::_scale, "scale", "Scale") {
		}
		virtual void onSetToDefaultValue(axValuePtr& data) const final {
			data.value<Vec3d>() = Vec3d(1,1,1);
		};
	};
	Field_scale field_scale;

	static const Spec* s_instance;

	axArray<axFieldInfo*, 16> _fields;

	Spec(axType* type)
		: AnonObjectSpec(type)
		, field_translate(type)
		, field_rotate(type)
		, field_scale(type)
	{
		s_instance = this;
		axInt ownFieldCount = 3;
		auto baseFieldCount = type->_baseClass->fields().size();
		_fields.resize(ownFieldCount + baseFieldCount);
		_fields.slice(0, baseFieldCount).copyValues(type->_baseClass->fields());
		auto ownFields = _fields.sliceFrom(baseFieldCount);

		ownFields[0] = &field_translate;
		ownFields[1] = &field_rotate;
		ownFields[2] = &field_scale;

		type->_fields    = _fields.span();
		type->_ownFields = ownFields;
	}

};

void GEO_Transform::s_initRtti(axType* type) {
	static Spec spec(type);
}

const GEO_Transform::Spec* GEO_Transform::Spec::s_instance = nullptr;

void GEO_Transform::onInitProps() {
	Base::onInitProps();
	auto* spec = Spec::s_instance;
	_initProp(spec->field_translate, _translate);
	_initProp(spec->field_rotate, _rotate);
	_initProp(spec->field_scale, _scale);
}

bool GEO_Transform::onReadJsonProp(axJsonReader& rd_) {
	bool _ret = Base::onReadJsonProp(rd_);
	if (rd_.isMember("translate")) { rd_.readValue(_translate); _ret = true; }
	if (rd_.isMember("rotate")) { rd_.readValue(_rotate); _ret = true; }
	if (rd_.isMember("scale")) { rd_.readValue(_scale); _ret = true; }
	return _ret;
}

void GEO_Transform::onWriteJsonProp(axJsonWriter& wr_) const {
	Base::onWriteJsonProp(wr_);
	wr_.writeMember("translate", _translate);
	wr_.writeMember("rotate", _rotate);
	wr_.writeMember("scale", _scale);
}

} // namespace 