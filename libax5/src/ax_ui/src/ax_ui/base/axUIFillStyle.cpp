#include "axUIFillStyle.h"

void axUIFillStyle::draw(const axSourceLoc& loc, axRenderRequest& req, const Rect2& rect) {
	if (sprite.texture()) {
		req.ui.drawSprite(loc, sprite, rect, {0,0,1,1}, color);
	}
}
