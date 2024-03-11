#include "axType.h"
#include <ax_core/rtti/axTypeManager.h>

axType_Init::axType_Init(const axSourceLoc& loc, axStrLiteral8 internalUniqueName, const axType* baseClass, axStrLiteral name,
						 axType::InternalType internalType, axType::InitFunc initFunc)
{
	auto* mgr = axTypeManager::s_instance();
	mgr->addType(loc, internalUniqueName, &_type, baseClass, name, internalType, initFunc);
	_type._postInitCheck();
}

//----- primitive -------
template<class T> inline
void axType_Primitive_initRtti(axType* type) {
	type->setVoidPtrAssignFunc	(&axType_VoidPtrAssignWrapper<T>);
	type->setReadJsonFunc		(&axJsonReader_VoidPtrWrapper<T>);
	type->setWriteJsonFunc		(&axJsonWriter_VoidPtrWrapper<T>);
}


//---------- void -----------

template<> const axType* ax_typeof<void>() {
	static axType_Init typeInit(AX_LOC, AX_FUNC_FULLNAME, nullptr, "void", axType::InternalType::Void, nullptr);
	return &typeInit._type;
}


#define AX_RTTI_IMPL_PRIMITIVE(T, INTERAL_TYPE) \
	template<> const axType* ax_typeof<T>() { \
		static axType_Init typeInit(AX_LOC, AX_FUNC_FULLNAME, nullptr, #T, axType::InternalType::INTERAL_TYPE, axType_Primitive_initRtti<T>); \
		return &typeInit._type; \
	} \
//-------

AX_RTTI_IMPL_PRIMITIVE(bool,       Bool);

AX_RTTI_IMPL_PRIMITIVE(axInt8 ,    Int8 );
AX_RTTI_IMPL_PRIMITIVE(axInt16,    Int16);
AX_RTTI_IMPL_PRIMITIVE(axInt32,    Int32);
AX_RTTI_IMPL_PRIMITIVE(axInt64,    Int64);

AX_RTTI_IMPL_PRIMITIVE(axUInt8 ,   UInt8 );
AX_RTTI_IMPL_PRIMITIVE(axUInt16,   UInt16);
AX_RTTI_IMPL_PRIMITIVE(axUInt32,   UInt32);
AX_RTTI_IMPL_PRIMITIVE(axUInt64,   UInt64);

AX_RTTI_IMPL_PRIMITIVE(axFloat16 , Float16);
AX_RTTI_IMPL_PRIMITIVE(axFloat32 , Float32);
AX_RTTI_IMPL_PRIMITIVE(axFloat64 , Float64);
AX_RTTI_IMPL_PRIMITIVE(axFloat128, Float128);

const axType* axTypedBase::s_rtti() {
	static axType_Init typeInit(AX_LOC, AX_FUNC_FULLNAME, nullptr, "axTypedBase", axType::InternalType::Class, nullptr);
	return &typeInit._type;
}

void axType::onFormat(axFormat& f) const {
#if 1
	f << _name;
#else
	f << "\n Type: " << _name;
	for (auto& e : fields()) {
		f << "\n  " << e;
	}
	f << "\n";
#endif
}

axType::~axType() {
	axDListNode<axType>::removeFromList();
}

bool axType::isKindOf(const axType* t) const
{
	if (this == t) return true;
	if (!t) return false;

	auto* p = this;
	while (p) {
		if (*p == *t) return true;
		p = p->_baseClass;
	}
	return false;
}

bool axType::operator==(const axType& r) const {
	if (this == &r)
		return true;

	if (_internalType != r._internalType)
		return false;

	if (_internalType < InternalType::_EndOfBuiltInType)
		return true;

	switch (_internalType) {
		case InternalType::Class: {
			return this == &r;
		}break;

		case InternalType::Array: {
			if (!_elementType || !r._elementType) {
				AX_ASSERT(false);
				return false;
			}
			return *_elementType == *r._elementType;
		}break;

		case InternalType::Dict: {
			if (!_dictKeyType || !r._dictKeyType || !_elementType || !r._elementType) {
				AX_ASSERT(false);
				return false;
			}
			return *_elementType == *r._elementType && *_dictKeyType == *r._dictKeyType;
		}break;

		default: {
			AX_ASSERT(false);
			return false;
		}break;
	}
	AX_ASSERT(false);
	return false;
}

void axType::_postInitCheck() {
	if (_ownFields.size() != _initOwnFieldCount) {
		throw axError_Undefined(AX_LOC);
	}

	if (_baseClass) {
		if (_fields.size() != _ownFields.size() + _baseClass->_fields.size()) {
			throw axError_Undefined(AX_LOC);
		}
	}
}

void axType::_addField(axFieldInfo& fieldInfo) {
	AX_ASSERT(fieldInfo.owner() == this);

	axInt i =  _initOwnFieldCount;
	if (_baseClass) {
		i += _baseClass->fieldCount();
	}
	fieldInfo._index = i;
	_initOwnFieldCount++;
}

const axFieldInfo* axType::findField(axInt index) const {
	if (!_fields.inBound(index)) return nullptr;
	return _fields[index];
}

const axFieldInfo* axType::findField(axStrView name) const {
	for (const auto& f : fields()) {
		if (f->name() == name) {
			return f;
		}
	}
	return nullptr;
}

bool axType::getDataPtrByPath(void* obj, axStrView path_, void* & outData, const axType* & outType) const {
	axStrView path = path_;
	const axType* type = this;
	char* data = static_cast<char*>(obj);

	for (;;) {
		if (!path.size()) {
			outData = data;
			outType = type;
			return true;
		}

		auto pair = path.split(".");
		auto* field = type->findField(pair.first);
		if (!field)
			return false;

		data += field->offset();
		type = field->type();
		path = pair.second;
	}
}

void axType::setValueByPath(void* obj, axStrView path_, axStrView newValue) const {
	void* data = nullptr;
	const axType* type = nullptr;

	if (!getDataPtrByPath(obj, path_, data, type))
		throw axError_Undefined(AX_LOC);

	axJsonReader rd(newValue, "");
	type->readJson(rd, data);
}

void axType::initArray(const axType* elementType) {
	AX_ASSERT(_internalType == InternalType::Array);
	_elementType = elementType;
}

void axType::initDict (const axType* dictKeyType, const axType* elementType) {
	AX_ASSERT(_internalType == InternalType::Dict);
	_dictKeyType = dictKeyType;
	_elementType = elementType;
}

void axType::initSPtr(const axType* elementType) {
	AX_ASSERT(_internalType == InternalType::SPtr);
	_elementType = elementType;
}
