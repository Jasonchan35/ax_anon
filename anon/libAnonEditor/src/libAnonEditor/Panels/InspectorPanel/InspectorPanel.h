#pragma once

#include "../EditorPanel.h"
#include "../../EditorUI/EdUIObjectInspector.h"

namespace Anon{ namespace Editor {

class InspectorPanel : public EditorPanel {
	AX_RTTI_CLASS(InspectorPanel, EditorPanel)
public:
	using ObjectInspector = EdUIObjectInspector;
	InspectorPanel(axUIDockZone* zone);

protected:
	virtual void onMouseEvent(axUIMouseEvent& ev) override;
	virtual void onEditorSelectionChanged() override;
	virtual void onAnonContextChanged(AnonContext::ChangedFlags flags) override;

private:
	class ObjectGroup : public axUIVBoxView {
		AX_RTTI_CLASS(ObjectGroup, axUIVBoxView);
	public:
		ObjectGroup(InspectorPanel* parent);

		void addObject(AnonObject* object);

		virtual void onReloadContent() override;

		void onEvRelatedObject(AnonObject* obj) { addObject(obj); }

		ObjectInspector* newObjectInspector		(const axType* objectType);
		ObjectInspector* findObjectInspector		(const axType* objectType);
		ObjectInspector* findOrNewObjectInspector	(const axType* objectType);

	private:
		InspectorPanel* _inspectorPanel;
		axLinkArray<AnonObject>	_objects;
	};

	ObjectGroup _objectGroup;
};


}} //namespace
