#include "SCN_System.h"
#include "SCN_Object.h"
#include "Lights/SCN_PointLight.h"
#include <libAnon/Core/AnonContext.h>

namespace Anon {

SCN_System* SCN_System_instance;

SCN_System* SCN_System::s_instance() { return SCN_System_instance; }

SCN_System::SCN_System() {
	AX_ASSERT(SCN_System_instance == nullptr);
	SCN_System_instance = this;
}

SCN_System::~SCN_System() {
	_rootObject.unref();
}

void SCN_System::create() {
	_rootObject.ref(ax_new SCN_Object(*this));
	_rootObject->setName("<Root>");
}

void SCN_System::loadSceneFile(axStrView filename) {
	_sceneFilename = filename;

	axMemMap mm(filename);
	loadSceneJson(mm.strView8(), filename);
}

void SCN_System::loadSceneJson(axStrView json, axStrView filename) {
	create();

	axJsonReader rd(json, filename);

	rd.beginObject();
	while (!rd.endObject()) {
		if (rd.beginArray("objects")) {
			while (!rd.endArray()) {
				SCN_Object::s_newFromJson(rd, nullptr);
			}
			continue;
		}
		rd.skipUnhandledMember(true);
	}
}

void SCN_System::saveCurrentSceneFile() {
	saveAsSceneFile(axString(_sceneFilename));
}

void SCN_System::revertCurrentSceneFromFile() {
	axTempString filename(_sceneFilename.view()); // amke a copy, because loadSceneFile will change `_sceneFilename`
	loadSceneFile(filename);
}

void SCN_System::saveAsSceneFile(axStrView filename) {
	axTempString8	json;
	axJsonWriter wr(json);

	{
		auto AX_NO_NAME = wr.object();
		if (_rootObject) {
			auto AX_NO_NAME = wr.array("objects");
			for (auto& c : _rootObject->children()) {
				c.onJsonWriter(wr);
			}
		}
	}
	axFile::writeFileIfChanged(filename, json, true, true, true);
}

SCN_Object* SCN_System::findObject(axStrView path) {
	if (!_rootObject) return nullptr;
	return _rootObject->findChild(path);
}

SCN_Object* SCN_System::newObject(SCN_Object* parent) {
	if (!parent) {
		parent = rootObject();
	}
	auto* obj = ax_new SCN_Object(parent);
	return obj;
}

SCN_Component* SCN_System::newComponent(const axSourceLoc& loc, axStrView type, SCN_Object* parent) {
	auto* allocator = ax_scope_allocator();
	if (auto comp = _componentFactory.createByName(loc, allocator, type, parent)) {
		comp->initProps();
		return comp.detach();
	}
	return nullptr;
}

void SCN_System::render(axRenderRequest& req) {
	AX_FrameMarkNamed("SCN_System");
	AX_ZoneScoped;

	for (auto& p_ :  _lightList) {
		if (auto* p = ax_type_cast<SCN_PointLight>(&p_)) {
			req.world.lightPos = p->sceneObject()->worldPos();
			req.world.lightPower = p->lightPower;
			req.world.lightColor = p->lightColor;
		}
	}

	for (auto& p : _rendererList) {
		p.onRender(req);
	}

}

} //namespace
