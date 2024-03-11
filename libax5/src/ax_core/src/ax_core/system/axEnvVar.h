//
//  axEnvVar.hpp
//  libax_core
//
//  Created by Jason on 25/4/2020.
//

#pragma once

#include "../string/axStrView.h"

namespace axEnvVar {
	bool putValue(axStrView string); // same as "name=value" -> setValue(name, value, true);
	bool setValue(axStrView name, axStrView value, bool overwrite = true);
	bool unsetValue(axStrView name);

	bool getValue(axIString& outValue, axStrView name);
};
