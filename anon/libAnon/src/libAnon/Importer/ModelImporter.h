#pragma once

#include <libAnon/libAnon_config.h>

namespace Anon {

class ModelImporter : public axNonCopyable {
public:
	struct Option {
		float scale = 1;
	};
	Option option;
};

} //namespace