#include "EdUIStructField.h"
#include <libAnonEditor/App/EditorApp.h>

namespace Anon { namespace Editor {

EdUIStructField::EdUIStructField(CreateDesc& desc) 
	: Base(desc) 
{
//	setPadding(8);

	bool singleRow = desc.fieldInfo->type()->isSingleRowLayout();
	_fieldBox.boxLayout.setIsVertical(!singleRow);

	CreateDesc memberDesc = desc;
	memberDesc.level++;

	auto parentView = &_fieldBox;

	auto* app = EditorApp::s_instance();

	axTempString fp;

	for (auto& f : desc.fieldInfo->type()->fields()) {
		memberDesc.parent = parentView;
		memberDesc.fieldInfo = f;
		fp.set(_fieldPath, ".", f->name());
		memberDesc.fieldPath = fp;
		app->modules.editorUI.createField(AX_LOC, ax_scope_allocator(), memberDesc);
	}

}

}}