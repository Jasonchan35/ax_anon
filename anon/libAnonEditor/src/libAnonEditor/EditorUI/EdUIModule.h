#pragma once
#include "Fields/EdUIField.h"
#include "Diagram/EdUIDiaNode.h"
#include "EdUIObjectInspector.h"
#include "../App/EditorModuleBase.h"

namespace Anon { namespace Editor {

class EdUIModule : public EditorModuleBase {
public:
	void create();

	EdUIField*				createField(const axSourceLoc& loc, axAllocator* allocator, EdUIField::CreateDesc& desc);

	template<class T, class T_VIEW> void addObjectInspectorType() { _objectViewFactory.addEntry<T, T_VIEW>(); }
	EdUIObjectInspector*	createObjectInspector(const axSourceLoc& loc, axAllocator* allocator, EdUIObjectInspector::CreateDesc& desc);

	template<class T, class T_VIEW> void addDiaNodeType() { _diaNodeFactory.addEntry<T, T_VIEW>(); }
	EdUIDiaNode*		createDiaNode(const axSourceLoc& loc, axAllocator* allocator, EdUIDiaNode::CreateDesc& desc);

private:
	using ObjectInspectorFactory  = axTypeFactoryT<EdUIObjectInspector, EdUIObjectInspector::CreateDesc&>;
	ObjectInspectorFactory	_objectViewFactory;

	using DiaNodeFactory = axTypeFactoryT<EdUIDiaNode, EdUIDiaNode::CreateDesc&>;
	DiaNodeFactory _diaNodeFactory;

	using FieldFactory = axTypeFactoryT<EdUIField, EdUIField::CreateDesc&>;
	FieldFactory	_fieldFactory;
	FieldFactory	_sliderFieldFactory;

};

}}
