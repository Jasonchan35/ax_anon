#include "SCN_Object_Inspector.h"

namespace Anon{ namespace Editor {

SCN_Object_Inspector::SCN_Object_Inspector(CreateDesc& desc)
	: Base(desc)
	, _addComponentButton(this, "Add Component")
{
	_addComponentButton.evClick.bind(this,
		[this](auto& ev) 
	{
		auto* menu = ax_new axUIContextMenu(this);
		for (auto& t : SCN_System::s_instance()->componentTypes()) {
			menu->addItem(t->name(), 
				this, [this, t]()
			{
				this->_addComponent(t);
			});
		}			
	});
}

void SCN_Object_Inspector::_addComponent(const axType* type) {
	auto& ob = objectBinding();
	for (auto& obj : ob.objects) {
		auto* scnObj = SCN_Object::s_find(obj);
		if (!scnObj) continue;
		scnObj->newComponent(AX_LOC, type->name());
	}
}

void SCN_Object_Inspector::onReloadContent() {
	Base::onReloadContent();

	auto& ob = objectBinding();
	for (auto& obj : ob.objects) {
		auto* scnObj = SCN_Object::s_find(obj);
		if (!scnObj) continue;

		for (auto& c : scnObj->components()) {
			evRelatedObject.emit(&c);
		}
	}
}

}} //namespace
