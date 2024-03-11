#pragma once

#include "../libanon_config.h"

namespace Anon {

class ModelImporter : public axNonCopyable {
public:
	struct Option {
		float scale = 1;
	};
	Option option;
};

} //namespace