#include <libanonEditor/App/EditorApp.h>
#include "EdUIObjectInspector.h"

namespace Anon { namespace Editor {

EdUIObjectInspector::EdUIObjectInspector(CreateDesc& desc)
	: Base(desc.parent)
	, _objectType(desc.objectType)
	, _titleBar(this)
	, _typeLabel(&_titleBar)
	, _optionButton(&_titleBar, "...")
	, _nameRow(this)
	, _nameLabel(&_nameRow, "Name: ")
	, _nameBox(&_nameRow)
	, _fieldsView(this)
	, _objectInfo(this)
{
//	setClipContent(true);
	layout.setFitContentH();

	_nameRow.layout.setFitContentH();
	_nameLabel.layout.setExpandW();
	_nameBox.layout.setExpandProportionW(4);
	_nameBox.evSetValue.bind(this, 
		[this](auto text)
	{
		auto& objs = _objectBinding.objects;
		if (objs.size() == 1) {
			if (auto* obj = AnonObject::s_find(objs[0])) {
				axTempString tmp;
				tmp.convert(text);
				obj->setName(axNameId::s_make(tmp));
				AnonContext::s_instance()->markChangedFlags(AnonContext::ChangedFlags::RenameObject);
			}
		}
	});

	_titleBar.setBgColor(0,0,0);
	_titleBar.layout.setFitContentH();

	_typeLabel.layout.setExpandW();
	_typeLabel.setText(desc.objectType->name());

	_optionButton.layout.setFitContentW();
	_optionButton.evClick.bind(this, &This::_onOptionButton);

	_fieldsView.layout.setExpandW();

	desc.parent = this;
	_addFields(desc.objectType);

	_objectInfo.setBgColor(.12f, .12f, .12f);
	_objectInfo.setBorderColor(0,0,0);
	_objectInfo.layout.setExpandW();

	_objectBinding.evValueChanged.bind(this, &This::_onObjectValueChanged);
}

const axType* EdUIObjectInspector::objectType() const {
	return _objectType;
}

void EdUIObjectInspector::addObject(AnonObject* object) {
	_objectBinding.addObject(object);
}

void EdUIObjectInspector::onAddField(EdUIField::CreateDesc& desc) {
	auto* app = EditorApp::s_instance();
	app->modules.editorUI.createField(AX_LOC, ax_scope_allocator(), desc);
}

void EdUIObjectInspector::onReloadContent() {
	Base::onReloadContent();
	axTempString tmp("---- Info ----\n");

	auto& ob = _objectBinding;

	ob.onGetObjectInfo(tmp);
	_objectInfo.setText(tmp);

	if (ob.objects.size() == 1) {
		if (auto* obj = AnonObject::s_find(ob.objects[0])) {
			_nameBox.setText(obj->name().toString());
		}
	} else {
		_nameBox.setIsMultipleValue();
	}
}

void EdUIObjectInspector::_onObjectValueChanged() {
	reloadContent();
	setNeedToRender();
}

void EdUIObjectInspector::_onOptionButton(axUIButtonEvent& ev) {
	auto* menu = ax_new EdUIContextMenu(this);

	menu->layout.excludeFromLayout();
	menu->addItem("Reset To Default", this, [this](){
		_objectBinding.setToDefaultValue();
	});

	menu->addItem("Testing", nullptr, nullptr);
	menu->addItem("Testing", nullptr, nullptr);

	menu->updateLayout();

	menu->moveWithinWorldRect(uiScreen().worldRect());
	menu->showOnScreen();
}

void EdUIObjectInspector::_addFields(const axType* type) {
	EdUIField::CreateDesc desc;
	desc.objectBinding = &_objectBinding;
	desc.parent = &_fieldsView;

	for (auto& field : type->fields()) {
		desc.fieldInfo = field;
		desc.fieldPath = field->name();
		onAddField(desc);
	}
}

}}
