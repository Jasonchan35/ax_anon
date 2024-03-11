#include <libanonEditor/App/EditorApp.h>
#include "SCN_Geometry_Inspector.h"

namespace Anon{ namespace Editor {

SCN_Geometry_Inspector::SCN_Geometry_Inspector(CreateDesc& desc)
	: Base(desc)
	, _diagramButton(this, "Open Diagram")
{
	_diagramButton.evClick.bind(this, &This::_openDiagram);
}

void SCN_Geometry_Inspector::onReloadContent() {
	Base::onReloadContent();

	auto& ob = objectBinding();
	for (auto& obj_ : ob.objects) {
		auto* obj = SCN_Geometry::s_find(obj_);
		if (!obj) continue;

		auto* dia = obj->diagram();
		if (!dia) continue;

		if (ob.objects.size() == 1) {
			if (auto* outputNode = dia->outputNode()) {
				evRelatedObject.emit(outputNode);
			}
		}
	}
}

void SCN_Geometry_Inspector::_openDiagram(axUIButtonEvent& ev) {
	auto& ob = objectBinding();
	if (ob.objects.size() == 1) {
		if (auto* obj = SCN_Geometry::s_find(ob.objects[0])) {
			auto* dia = obj->diagram();
			if (dia) {
				EditorApp::s_instance()->openDiagramPanel(dia);
			}
		}
	}
}

}} //namespace
