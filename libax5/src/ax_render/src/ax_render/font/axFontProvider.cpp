#include "axFontProvider.h"

axSPtr<axFont> axFontProvider::createFont(const axFontDesc& desc) {
	return onCreateFont(desc);
}