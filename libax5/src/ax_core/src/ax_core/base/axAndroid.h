#pragma once

#include "ax_base.h"

#if AX_OS_ANDROID

class axAndroid : public axNonCopyable {
	using This = axAndroid;
public:
	static This* s_instance();

	void _create(JNIEnv* env, jobject activity);

private:
	jobject		_activity = nullptr;
//	jclass		_activity_class = nullptr;
//	jmethodID	_activity_finish = nullptr;
//	jmethodID	_activity_getPackageResourcePath = nullptr;
};

#endif