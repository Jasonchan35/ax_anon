#include "axUITextBox.h"
#include "../base/axUIDefaultAssets.h"

axUITextBoxBase::axUITextBoxBase(axUIView* parent) 
	: Base(parent)
	, _caretShow(false)
	, _editingMode(false)
{
	_displayMesh.setColor(axUIDefaultAssets::s_instance()->fontStyles.std->color);
	setPadding({4, 2});
	setAcceptFocus(true);
	setBorderColor(0,0,0,1);
	setClipContent(true);
}

void axUITextBoxBase::onRender(axRenderRequest& req) {
	_displayMesh.renderInRect(req, contentRect());

	if (hasFocus()) {
		req.ui.drawRectBorder(AX_LOC, localRect(), Margin2(2), Color(.6f, .6f, .2f, 1));
	}

	if (_caretShow) {
		_updateCaretRect();
		req.ui.drawRect(AX_LOC, _caretRect, Color(.8f, .8f, .8f, 1));
	}
}

void axUITextBoxBase::onKeyEvent(axUIKeyEvent& ev) {
	Base::onKeyEvent(ev);

	if (!_editingMode) return;

	using Type = axUIKeyEventType;
	using KeyCode = axUIKeyCode;

	switch (ev.type) {
		case Type::Down: {
			switch (ev.key) {
				case KeyCode::LeftArrow:	setCaretPos(_caretPos - 1); return;
				case KeyCode::RightArrow:	setCaretPos(_caretPos + 1); return;
				//case KeyCode::UpArrow:	setCaretPos(_caretLine - 1, _caretCol); return;
				//case KeyCode::DownArrow:	setCaretPos(_caretLine + 1, _caretCol); return;
				case KeyCode::Delete:		_deleteChar(); return;
				case KeyCode::Escape:		_endEditingMode(false); return;
				case KeyCode::Enter:		_endEditingMode(true ); return;
			}
		}break;
	}

	if (ev.type == Type::Char) {
		if (!ev.text) return;
		auto key = ev.text[0];
		switch (key) {
			case '\b': {
				_backspace();
			}break;

			case '\t':
			case '\r':
			case '\n': {
			}break;

			default: {
				_insertText(ev.text);
			}break;
		}
	}
}

void axUITextBoxBase::_insertText(axStrViewU t) {
	axTempStringU tmp{_displayText};
	ax_clamp_it(_caretPos, axInt(0), tmp.size());
	tmp.insert(_caretPos, t);
	_caretPos += t.size();
	setDisplayText(tmp);
}

void axUITextBoxBase::_backspace() {
	if (!_text.size()) return;
	if (_caretPos <= 0) return;

	axTempStringU tmp{_displayText};
	tmp.removeChars(_caretPos - 1, 1);
	_caretPos--;
	setDisplayText(tmp);
}

void axUITextBoxBase::_deleteChar() {
	if (!_text.size()) return;
	if (_caretPos >= _text.size()) return;

	axTempStringU tmp{_displayText};
	tmp.removeChars(_caretPos, 1);
	setDisplayText(tmp);
}

void axUITextBoxBase::_startEditingMode() {
	if (_editingMode) return;
	_editingMode = true;
	_caretShow = true;
	_caretTimer.start(this, [this]{
		_caretShow = !_caretShow;
		setNeedToRender();
	}, 250, true);
}

void axUITextBoxBase::_endEditingMode(bool keepChange) {
	setFocus(false);

	if (!_editingMode) return;

	_editingMode = false;
	_caretTimer.stop();
	_caretShow = false;

	if (keepChange) {
		axStringU tmp = _displayText; // onSetValueByText will change _diplayText deep inside, so make a copy first
		if (onSetValueByText(tmp, true)) {
			return;
		}
	}

	// restore display text
	if (_valueState == ValueState::NoValue) {
		setIsMultipleValue();
	} else {
		setDisplayText(_text);
	}
}

void axUITextBoxBase::onMeasureLayout(axUIMeasureRequest& req) {
	_displayMesh.onMeasureLayout(req);
}

void axUITextBoxBase::setNoValue() {
	_text.set(AX_STR_U(""));
	_valueState = ValueState::NoValue;
	setDisplayText(_text);
}

void axUITextBoxBase::setIsMultipleValue() {
	_text.set(AX_STR_U(""));
	_valueState = ValueState::IsMultipleValue;
	setDisplayText(AX_STR_U("--"));
}

void axUITextBoxBase::setCaretPos(axInt pos) {
	_caretPos = ax_clamp(pos, axInt(0), _text.size());
//	AX_DUMP_VAR(_caretPos);
	setNeedToRender();
}

void axUITextBoxBase::_updateCaretRect() {
	_caretRect.pos = _padding.topLeft();
	_caretRect.size.set(2,0);

	auto glyphs = _displayMesh.block().glyphs();

	if (!glyphs.size()) {
		_caretPos = 0;
	} else {
		ax_clamp_it<axInt>(_caretPos, 0, glyphs.size());

		if (_caretPos == 0) {
			auto& g = glyphs[0];
			_caretRect.h = g.outerRect.h;
		} else {
			auto& g = glyphs[_caretPos - 1];
			_caretRect.pos += g.outerRect.xMaxYMin();
			_caretRect.h = g.outerRect.h;
		}
	}
}

void axUITextBoxBase::setBaseText(axStrViewU text) {
	if (_valueState == ValueState::HasValue && _text == text)
		return;

	auto re = _re_setBaseText.enter();
	if (!re) return;

	_text.set(text);
	_valueState = ValueState::HasValue;
	setDisplayText(text);

	onSetBaseText();
}

bool axUITextBoxBase::onSetValueByText(axStrViewU text, bool fireEvent) {
	setBaseText(text);
	return true;
}

void axUITextBoxBase::setDisplayText(axStrViewU text) {
	_displayMesh.setText(text);
	_displayText.set(text);
	setNeedToMeasure();
	setNeedToRender();
}

void axUITextBoxBase::onSetFocus(bool b) {
	if (b) {
		_startEditingMode();
	} else {
		_endEditingMode(true);
	}
}

void axUITextBox::setText(axStrViewU text) {
	setBaseText(text); 
}

void axUITextBox::getText(axIString& outText) {
	outText.convert(getBaseText());
}

bool axUITextBox::onSetValueByText(axStrViewU text, bool fireEvent) {
	Base::onSetValueByText(text, fireEvent);
	if (fireEvent) evSetValue.emit(text);
	return true;
}
