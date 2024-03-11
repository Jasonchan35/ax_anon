#pragma once

#include "axType.h"
#include "axAttribute.h"

class axTypeManager : public axNonCopyable {
public:
	static axTypeManager* s_instance();

	axTypeManager();
	~axTypeManager();

	const axType* addType(const axSourceLoc& loc, axStrLiteral8 internalUniqueName, axType* type, const axType* baseClass, axStrLiteral name, axType::InternalType internalType, axType::InitFunc initFunc);

private:
	axDList<axType>	_typeList;
};

//---------------------

template<class T> inline
void axIArray_initRtti(axType* type) {
	type->initArray(ax_typeof<T>());
}

template<class T> inline
const axType* axIArray<T>::s_rtti() {
	static axType_Init type(AX_LOC, AX_FUNC_FULLNAME, nullptr, "Array", axType::InternalType::Array, &axIArray_initRtti<T>);
	return &type._type;
}

template<class T> inline
void axSPtr_initRtti(axType* type) {
	type->initSPtr(ax_typeof<T>());
}

template<class T> inline
const axType* axSPtr<T>::s_rtti() {
	static axType_Init type(AX_LOC, AX_FUNC_FULLNAME, nullptr, "SPtr", axType::InternalType::SPtr, &axSPtr_initRtti<T>);
	return &type._type;
}
