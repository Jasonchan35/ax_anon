#pragma once

class axDynamicLinkLibrary : axNonCopyable {
#if AX_OS_WINDOWS
	using FuncAddr = FARPROC;
	HMODULE _h = nullptr;

#else
	using FuncAddr = void*;
	void* _h = nullptr;
#endif

public:
	~axDynamicLinkLibrary() { unload(); }

	bool load(axStrView filename);
	void unload();

	FuncAddr getProcAddress(axStrView name);

	template<class FUNC>
	void getProc(FUNC& func, axStrView name) {
		func = reinterpret_cast<FUNC>(getProcAddress(name));
		AX_ASSERT(func);
	}

private:

};