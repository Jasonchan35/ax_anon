#include "axUIChoiceBox.h"

axUIChoiceBox::axUIChoiceBox(axUIView* parent) 
	: Base(parent) 
	, _displayText(this)
{
	_displayText.setText("---");

	setBorderColor(0,0,0,1);
	layout.setFitContentW();
	layout.setFitContentH();
}

void axUIChoiceBox::addItem(axInt id, axStrView text) {
	auto& p = _items.emplaceBack();
	p.id = id;
	p.text = text;
	setNeedToMeasure();
}

void axUIChoiceBox::onMouseEvent(axUIMouseEvent& ev) {
	if (ev.isLeftDown()) {
		auto menu = ax_new axUIContextMenu(this);
		menu->setWorldPos(worldRect().bottomLeft());
		for (auto& it : _items) {
			menu->addItem(it.text, this, [&]() {
				this->selectItemById(it.id);

				axUIChoiceBoxEvent ev;
				ev.selectedId = it.id;
				onValueChanged(ev);
			});
		}
	}
}

void axUIChoiceBox::onRender(axRenderRequest& req) {
	Color color(.5f, .5f, .5f);
	req.ui.drawRect(AX_LOC, localRect(), color);
}

void axUIChoiceBox::selectItemById(axInt id) {
	auto* p = findItemById(id);
	if (p) {
		_selectedId = id;
		_displayText.setText(p->text);
	}
}

axUIChoiceBox::Item* axUIChoiceBox::findItemById(axInt id) {
	for (auto& p : _items) {
		if (p.id == id)
			return &p;
	}
	return nullptr;
}
