#pragma once

#include "../container/axDList.h"
#include "../thread/axMutex.h"

class axGlobalSingletonBase : public axDListNode<axGlobalSingletonBase> {
	using This = axGlobalSingletonBase;
public:
	class AllocatorInit {};

	virtual ~axGlobalSingletonBase() {}
	virtual void callDestructor() {}
};

//! Guarantee the destructor order in global scope
template<class T>
class axGlobalSingleton : public axGlobalSingletonBase {
public:
	template<class... Args>
	axGlobalSingleton(Args&&... args);
	virtual ~axGlobalSingleton();

	T* ptr() { return reinterpret_cast<T*>(_buffer); }
	operator T* () { return ptr(); }

	virtual void callDestructor() override;

private:
	template<class... Args>
	void _ctor(Args&&... args);

	char _buffer[sizeof(T)];
	bool _inited;
};

class axGlobalSingletonManager : public axNonCopyable {
public:
	using List = axDList<axGlobalSingletonBase>;

	void add(axGlobalSingletonBase* p);

	static axGlobalSingletonManager* s_instance();

	void shutdown();
	~axGlobalSingletonManager() { shutdown(); }

private:
	struct Data {
		List list;
	};
	axInternalSpinLockProtected<Data> _data;
};

//---------------------

template<class T>
template<class... Args>
axGlobalSingleton<T>::axGlobalSingleton(Args&&... args) {
	_inited = true;
	ax_call_constructor(reinterpret_cast<T*>(_buffer), ax_forward(args)...);

//	ax_log_func_name();
	axGlobalSingletonManager::s_instance()->add(this);
}

template<class T>
inline axGlobalSingleton<T>::~axGlobalSingleton() {
	axGlobalSingletonManager::s_instance()->shutdown();
}

template<class T>
inline void axGlobalSingleton<T>::callDestructor() {
//	ax_log_func_name();

	AX_ASSERT(_inited);
	ax_call_destructor(reinterpret_cast<T*>(_buffer));
	_inited = false;
}

