#pragma once

#include "../base/axUIView.h"
#include "../base/axUITimer.h"
#include "../text/axUITextMesh.h"

class axUITextBoxBase : public axUIView {
	AX_RTTI_CLASS(axUITextBoxBase, axUIView)
public:
	axUITextBoxBase(axUIView* parent);

	enum class ValueState {
		NoValue,
		HasValue,
		IsMultipleValue
	};

	void setNoValue();
	void setIsMultipleValue();

	ValueState	valueState() const { return _valueState; }

	bool noValueState			() const { return _valueState == ValueState::NoValue; }
	bool hasValueState			() const { return _valueState == ValueState::HasValue; }
	bool isMultipleValueState	() const { return _valueState == ValueState::IsMultipleValue; }

	void setCaretPos(axInt pos);

protected:
	void setBaseText(axStrViewU text);
	void setBaseText(axStrView  text) { setBaseText(axTempStringU::s_convert(text)); }
	void setDisplayText(axStrViewU text);

	void _updateCaretRect();

	virtual void onSetFocus		(bool b) override;
	virtual void onRender		(axRenderRequest& req) override;
	virtual void onMeasureLayout(axUIMeasureRequest& req) override;
	virtual void onKeyEvent		(axUIKeyEvent& ev) override;

	virtual bool onSetValueByText(axStrViewU text, bool fireEvent);

	virtual void onSetBaseText() {}

	axStrViewU	getBaseText() const { return _text; }

private:
	void	_insertText(axStrViewU t);
	void	_backspace();
	void	_deleteChar();

	void	_startEditingMode();
	void	_endEditingMode(bool keepChange);

	ValueState		_valueState = ValueState::NoValue;
	axStringU		_text;
	axStringU		_displayText;
	axUITextMesh	_displayMesh;
	axInt			_caretPos = 0;
	Rect2			_caretRect;
	axUITimer		_caretTimer;
	bool			_caretShow : 1;
	bool			_editingMode : 1;

	axEnterOnce		_re_setBaseText;
};

class axUITextBox : public axUITextBoxBase {
	AX_RTTI_CLASS(axUITextBox, axUITextBoxBase)
public:
	axUITextBox(axUIView* parent) : Base(parent) {}
	
	void setText(axStrViewU text);
	void setText(axStrView  text) { setText(axTempStringU::s_convert(text)); }

	void getText(axIString& outText);

	axEvent<void (axStrViewU text)> evSetValue;

protected:
	virtual bool onSetValueByText(axStrViewU text, bool fireEvent) override;
};
