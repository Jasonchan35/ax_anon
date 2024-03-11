#pragma once

#include "SCN_Component.h"

namespace Anon {

class SCN_System;

AX_CLASS()
class SCN_Object : public AnonObject, public axDListNode<SCN_Object> {
	AX_CLASS_SPEC(SCN_Object, AnonObject)

protected:
	SCN_Object(SCN_Object* parent);
public:
	~SCN_Object();

	void addChild(SCN_Object* c);
	axDList<SCN_Object>::Enumerator	children()		{ return _children.each(); }

	axDList<SCN_Component>::Enumerator	components()	{ return _components.each(); }

	SCN_Object* findChild(axStrView path);

	void clearChildren();
	void clearComponents();
	void removeFromParent();

	template<class COMP>
	COMP*	findComponent() {
		auto* comp = findComponent(ax_typeof<COMP>());
		return ax_type_cast<COMP>(comp);
	}

	SCN_Component*	findComponent(const axType* type);

	SCN_Component* newComponent(const axSourceLoc& loc, axStrView type);
	void addComponent(SCN_Component* c);
	void removeComponent(SCN_Component* c);

	static SCN_Object*	s_newFromJson(axJsonReader& rd, SCN_Object* parent);

	void onJsonWriter(axJsonWriter& wr);

	SCN_System* system();

	Vec3f worldPos() { return localToWorldMatrix().position(); }

	Mat4f parentToWorldMatrix() { return _parent ? _parent->localToWorldMatrix() : Mat4f::kIdentity(); }
	const Mat4f& localToWorldMatrix() & { compute(); return _localToWorldMatrix; }
	
	virtual void onDrawGizmo(axRenderRequest& req) override;

	Vec3f translate() const { return _translate; }
	Vec3f rotate() const { return _rotate; }
	Vec3f scale() const { return _scale; }

	void setTranslate(const axVec3f& v) { _translate = v; setComputeNeeded(); }

friend class SCN_System;
protected:
	virtual void onSetComputeNeeded() override;
	virtual void onCompute() override;

	axDList<SCN_Component>	_components;
	axDList<SCN_Object>		_children;
	SCN_Object*				_parent = nullptr;

	SCN_Object(SCN_System& system);
	void _init();

private:
	AX_PROP()	Vec3f	_translate	= Vec3f(0,0,0);
	AX_PROP()	Vec3f	_rotate		= Vec3f(0,0,0);
	AX_PROP()	Vec3f	_scale		= Vec3f(1,1,1);

	Mat4f	_localToWorldMatrix;
};

} //namespace

