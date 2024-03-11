#pragma once

#include "../Core/AnonObject.h"

namespace Anon {

class SCN_Object;
class SCN_System;

AX_CLASS()
class SCN_Component : public AnonObject, public axDListNode<SCN_Component> {
	AX_CLASS_SPEC(SCN_Component, AnonObject)
public:
	SCN_Component(SCN_Object* object);

	void setName(axStrView name) { _name = name; }
	axStrView name() const { return _name; }

	SCN_Object* sceneObject() { return _sceneObject; }
	axNameId	sceneObjectName() const;

	void removeFromObject();

	static SCN_Component* s_newFromJson(axJsonReader& rd, SCN_Object* parentObject);

	SCN_System*	system();

	void onJsonWriter(axJsonWriter& wr);

friend class SCN_Object;
protected:

	SCN_Object* _sceneObject = nullptr;
	axString _name;
};

class SCN_RendererBase : public SCN_Component, public axDListNode<SCN_RendererBase> {
	AX_RTTI_CLASS(SCN_RendererBase, SCN_Component);
public:
	SCN_RendererBase(SCN_Object* object);
	virtual ~SCN_RendererBase();

	virtual void onRender(axRenderRequest& req) {}
};

} //namespace
