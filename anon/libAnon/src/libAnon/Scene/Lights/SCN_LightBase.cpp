#include "SCN_LightBase.h"
#include "../SCN_System.h"

namespace Anon {

SCN_LightBase::SCN_LightBase(SCN_Object* object) 
	: Base(object) 
{
	system()->_addLight(this);
}

SCN_LightBase::~SCN_LightBase() {
	axDListNode<SCN_LightBase>::removeFromList();
}


}