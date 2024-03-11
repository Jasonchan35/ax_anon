#pragma once

#include "../Core/AnonObject.h"

namespace Anon {

class Asset : public AnonObject {
	AX_RTTI_CLASS(Asset, AnonObject)
public:

	axStrView	filename() const { return _filename; }

private:
	axString	_filename;
};

} //namespace