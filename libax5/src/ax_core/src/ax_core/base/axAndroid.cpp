#include "axAndroid.h"
#include "../other/axGlobalSingleton.h"

#if AX_OS_ANDROID

axAndroid* axAndroid::s_instance() {
	static axGlobalSingleton<axAndroid> s;
	return s.ptr();
}

void axAndroid::_create(JNIEnv* env, jobject activity) {
	_activity = activity;
}

extern "C" {

JNIEXPORT void Java_com_awenix_libax_axAndroid_jniOnInit( JNIEnv* env, jobject obj, jobject activity ) {
	AX_LOG("================== axAndroid_jniOnInit ===================");
	auto* an = axAndroid::s_instance();
	an->_create(env, activity);
}

}//extern "C"


#endif