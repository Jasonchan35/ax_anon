#pragma once

#include "axUILabel.h"
#include "../menu/axUIContextMenu.h"

class axUIChoiceBoxEvent {
public:
	axInt	selectedId;
};

class axUIChoiceBox : public axUIView {
	AX_RTTI_CLASS(axUIChoiceBox, axUIView)
public:
	axUIChoiceBox(axUIView* parent);

	struct Item {
		axInt		id;
		axString	text;
	};

	void addItem(axInt id, axStrView text);

	virtual void onRender(axRenderRequest& req) override;
	virtual void onMouseEvent(axUIMouseEvent& ev) override;

	void selectItemById(axInt id);

	Item*	findItemById(axInt id);

	axDelegate<void (axUIChoiceBoxEvent& ev)> evValueChanged;
	virtual void onValueChanged(axUIChoiceBoxEvent& ev) { evValueChanged.emit(ev); }

private:
	axArray<Item>	_items;
	axInt			_selectedId = -1;
	axUILabel		_displayText;
};