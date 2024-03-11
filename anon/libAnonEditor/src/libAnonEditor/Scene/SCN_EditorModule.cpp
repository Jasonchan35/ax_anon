#include <libanonEditor/App/EditorApp.h>
#include "SCN_EditorModule.h"
#include "SCN_Object_Inspector.h"
#include "SCN_Component_Inspector.h"
#include "SCN_Geometry_Inspector.h"

namespace Anon { namespace Editor {

void SCN_EditorModule::onCreate() {
	auto* app = EditorApp::s_instance();

	auto& editorUI = app->modules.editorUI;

	#define E(T) \
		editorUI.addObjectInspectorType<T, T##_Inspector>();
	//-----
		E(SCN_Object)
		E(SCN_Component)
		E(SCN_Geometry)
	#undef E


}

}} // namespace