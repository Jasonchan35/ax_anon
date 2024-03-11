#pragma once

#include "../Core/AnonObject.h"
#include "SCN_Object.h"
#include "Lights/SCN_LightBase.h"

namespace Anon {

AX_CLASS()
class SCN_System : public AnonObject {
	AX_CLASS_SPEC(SCN_System, AnonObject)
public:
	static SCN_System*	s_instance();

	SCN_System();
	~SCN_System();

	void create();

	void loadSceneFile(axStrView filename);
	void loadSceneJson(axStrView json, axStrView filename);

	void saveCurrentSceneFile();
	void saveAsSceneFile(axStrView filename);

	void revertCurrentSceneFromFile();

	SCN_Object* rootObject() { return _rootObject.ptr(); }

	SCN_Object*	findObject(axStrView path);
	
	SCN_Object*		newObject(SCN_Object* parent);
	SCN_Component*	newComponent(const axSourceLoc& loc, axStrView type, SCN_Object* parent);

	template<class T> void addNodeType() { _componentFactory.addEntry<T>(); }
	axSpan<const axType*>	componentTypes()	{ return _componentFactory.entries(); }

	using LightList = axDList<SCN_LightBase>;
	using IRendererList = axDList<SCN_RendererBase>;

	LightList::Enumerator lights() { return _lightList.each(); }


	void render(axRenderRequest& req);
	void _addRenderer	(SCN_RendererBase*	comp)	{ _rendererList.append(comp); }
	void _addLight		(SCN_LightBase*		comp)	{ _lightList.append(comp); }

private:
	IRendererList	_rendererList;
	LightList		_lightList;

	using ComponentFactory = axTypeFactoryT<SCN_Component, SCN_Object*>;
	ComponentFactory _componentFactory;

	axUPtr<SCN_Object> _rootObject;

	axString	_sceneFilename;
};

} //namespace
