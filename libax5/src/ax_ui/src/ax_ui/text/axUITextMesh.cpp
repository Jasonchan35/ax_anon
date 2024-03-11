#include "axUITextMesh.h"

void axUITextMesh::addText(axStrView text, const Style* style) { axTempStringU tmp; tmp.setUtf(text); addText(tmp, style); }
void axUITextMesh::setText(axStrView text, const Style* style) { axTempStringU tmp; tmp.setUtf(text); setText(tmp, style); }

void axUITextMesh::addText(axStrViewU text, const Style* style) {
	_meshDirty = true;
	_block.addText(text, style);
}

void axUITextMesh::setText(axStrViewU text, const Style* style) {
	clear();
	addText(text, style);
}

void axUITextMesh::clear() {
	_block.clear();
	_meshDirty = true;
}

void axUITextMesh::updateMesh() {
	if (!_block.layout() && !_meshDirty) return;
	_meshDirty = false;
	_textMesh.clear();
	  _bgMesh.clear();

	auto editTextMesh = _textMesh.edit();
	auto editBgMesh   = _bgMesh.edit();
	auto bgUv = Rect2(0,0,0,0);
	auto color = Color(1,1,1);

	for (auto& line : _block.lines()) {
		for (auto& g : line.glyphs) {
			if (g.glyphIndex == 0)
				continue;

			editTextMesh.addRect(axRenderVertex_PosUvColor::s_desc(), g.vertexRect, g.uv, color);
			  editBgMesh.addRect(axRenderVertex_PosUvColor::s_desc(), g.outerRect,  bgUv, color);
		}	
	}
}

const axBBox2f& axUITextMesh::boundingBox() {
	return block().boundingBox();
}

void axUITextMesh::onMeasureLayout(axUIMeasureRequest& req) {
	auto& box = boundingBox();
	if (!box.isValid()) {
		req.result.min.set(0, 0);
	} else {
		req.result.min.set(box.xMax(), box.yMax()); 
	}
}

void axUITextMesh::setIsInlineDraw(bool b) {
	_textMesh.setIsInlineDraw(b);
	_bgMesh.setIsInlineDraw(b);
}

void axUITextMesh::render(axRenderRequest& req) {
	updateMesh();

	if (_bgColor.a > 0) {
		req.ui.drawMesh(AX_LOC, _bgMesh, Mat4::kIdentity(), _bgColor);
	}

	if (_color.a > 0) {
		auto* glyphTex = axFontManager::s_instance()->getGlyphTexture();
		auto mtl = req.ui.createMaterial_UITextTexture0(glyphTex, Mat4::kIdentity(), _color);
		req.addDrawCall(AX_LOC, mtl, _textMesh);
	}
}

void axUITextMesh::render(axRenderRequest& req, const Mat4& mat) {
	auto mv = req.ui.modelview.push();
	mv *= mat;
	render(req);
}

void axUITextMesh::render(axRenderRequest& req, const Vec2& pos) {
	render(req, axMat4f::s_translate(pos));
}

void axUITextMesh::renderInRect(axRenderRequest& req, const Rect2& rect) {
	auto pos = _block.align().posInRect(rect, _block.boundingBox().size());
	render(req, pos);
}

void axUITextMesh::setSelectionStart(axInt i) {
	if (_selectionStart == i) return;
	_meshDirty = true;
	_selectionStart = i;
}

void axUITextMesh::setSelectionEnd(axInt i) {
	if (_selectionEnd == i) return;
	_meshDirty = true;
	_selectionEnd = i;
}

