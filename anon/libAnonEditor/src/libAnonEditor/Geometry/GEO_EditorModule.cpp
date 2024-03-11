#include <libanonEditor/App/EditorApp.h>
#include "GEO_EditorModule.h"
#include "GEO_Node_UIDiaNode.h"

namespace Anon { namespace Editor {

void GEO_EditorModule::onCreate() {
	auto* app = EditorApp::s_instance();

	auto& editorUI = app->modules.editorUI;

	#define ax_ENTRY(T) \
		editorUI.addDiaNodeType<T, T##_UIDiaNode>();
	//-----
		ax_ENTRY(GEO_Node)
	#undef ax_entry

}

}} // namespace