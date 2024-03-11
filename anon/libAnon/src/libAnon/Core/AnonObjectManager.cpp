#include "AnonObjectManager.h"

namespace Anon {

AnonObjectManager::AnonObjectManager() {
	_nextObjectId._v = 1000;
}

AnonObjectManager::~AnonObjectManager() {
	AX_ASSERT(!_objectMap.size());
}

AnonObject* AnonObjectManager::findObject(AnonObjectId nodeId) {
	return _objectMap.find(nodeId);
}

void AnonObjectManager::onNewObject(AnonObject* p) {
	p->_objectId = _nextObjectId;
	_nextObjectId._v++;
	if (_nextObjectId._v <= 0) {
		throw axError_Undefined(AX_LOC);
	}

	_objectMap.add(p->_objectId, p);
}

void AnonObjectManager::onDeleteObject(AnonObject* p) {
	_objectMap.removeNode(p);
}

}