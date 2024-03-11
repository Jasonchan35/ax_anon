#include "axTypeManager.h"
#include "../other/axGlobalSingleton.h"

axTypeManager* axTypeManager::s_instance() {
	static axGlobalSingleton<axTypeManager> s;
	return s.ptr();
}

axTypeManager::axTypeManager() {
	ax_core_init();
}

axTypeManager::~axTypeManager() {
	_typeList.removeAll(); // node is from static var, so don't delete it by clear()
}

const axType* axTypeManager::addType(const axSourceLoc& loc, axStrLiteral8 internalUniqueName, axType* type, const axType* baseClass, axStrLiteral name, axType_InternalType internalType, axType_InitFunc initFunc) {
	if (!type) throw axError_Undefined(AX_LOC);
	if (type->_internalType != axType_InternalType::Unknown) throw axError_Undefined(AX_LOC);

//	AX_LOG("addType {?} {?}", name, fullname);

	_typeList.append(type);
	type->_loc = loc;
	type->_baseClass = baseClass;
	type->_name = name;
	type->_internalUniqueName = internalUniqueName;
	type->_internalType = internalType;

	if (initFunc) {
		initFunc(type);
	}

	using E = axType_InternalType;

	switch (internalType)
	{
		case E::Array:	AX_ASSERT(!type->_dictKeyType); AX_ASSERT(type->_elementType); break;
		case E::SPtr:	AX_ASSERT(!type->_dictKeyType); AX_ASSERT(type->_elementType); break;
		case E::Dict:	AX_ASSERT(type->_dictKeyType);	AX_ASSERT(type->_elementType); break;
		default:		AX_ASSERT(!type->_dictKeyType);	AX_ASSERT(!type->_elementType); break;
	}

	return type;
}
