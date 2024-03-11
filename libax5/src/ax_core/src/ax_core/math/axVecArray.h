#pragma once

#include "../container/axArray.h"
#include "axVec4.h"
#include "../graph/axColor.h"

template<axInt LOCAL_BUFFER_SIZE = 0>	using axVec2fArray = axArray<axVec2f, LOCAL_BUFFER_SIZE>;
template<axInt LOCAL_BUFFER_SIZE = 0>	using axVec3fArray = axArray<axVec3f, LOCAL_BUFFER_SIZE>;
template<axInt LOCAL_BUFFER_SIZE = 0>	using axVec4fArray = axArray<axVec4f, LOCAL_BUFFER_SIZE>;
template<axInt LOCAL_BUFFER_SIZE = 0>	using axColorArray = axArray<axColor, LOCAL_BUFFER_SIZE>;

