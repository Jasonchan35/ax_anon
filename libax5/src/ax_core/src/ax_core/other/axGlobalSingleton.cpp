#include "axGlobalSingleton.h"

void axGlobalSingletonManager::add(axGlobalSingletonBase * p) { 
	auto data = _data.scopedLock();
	data->list.append(p); 
}

void axGlobalSingletonManager::shutdown() {
	auto data = _data.scopedLock();

	while(auto* p = data->list.popTailPtr()) {
		p->callDestructor();
	}
}

axGlobalSingletonManager* axGlobalSingletonManager::s_instance() {
	static axGlobalSingletonManager s;
	return &s;
}
