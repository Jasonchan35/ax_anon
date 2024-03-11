#include "axError.h"
#include "ax_utils.h"

bool axError::s_enableAssertion = true;

void axError::_ctor() {
	if (s_enableAssertion) {
		_ax_assert_impl("axError_Undefined", "", loc.func, loc.file, static_cast<int>(loc.line), "");
	}
}

void axError::s_setEnableAssertion(bool b) {
	s_enableAssertion = b;
}

void axError::onFormat(axFormat& f) const {
	f.format("axError({?})", loc);
}
