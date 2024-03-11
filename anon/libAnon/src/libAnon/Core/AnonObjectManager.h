#pragma once
#include "AnonObject.h"

namespace Anon {

class AnonObjectManager : public axNonCopyable {
public:

	AnonObjectManager();
	~AnonObjectManager();

	AnonObject* findObject(AnonObjectId nodeId);

friend class AnonObject;
protected:

	void onNewObject(AnonObject* p);
	void onDeleteObject(AnonObject* p);

private:

	axMap<AnonObjectId, AnonObject> _objectMap;
	AnonObjectId _nextObjectId;
};

}