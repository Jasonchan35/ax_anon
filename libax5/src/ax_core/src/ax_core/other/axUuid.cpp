//
//  axUUID.cpp
//  ax_core
//
//  Created by Jason on 2013-11-14.
//
//

#include "axUuid.h"

void axUuid::onFormat(axFormat& f) const {
	auto* hex = axCStr::kLowHexChars();

	axTempString tmp;
	for (axInt i = 0; i < kDataSize; i++) {
		if (i == 4 || i == 6 || i == 8 || i == 10) {
			tmp.append("-");
		}

		tmp.append(hex[(value.data[i] >> 4) & 0xf]);
		tmp.append(hex[ value.data[i] & 0xf]);
	}

	f.onOutput(tmp);
}

bool axUuid::tryParse(axStrView str) {
	static constexpr axInt N = 36;

	if (str.size() < N) return false;

	try {
		axInt t = 0;
		for (axInt i = 0; i < N; i++) {
			auto ch = str[i];

			if (i == 8 || i == 13 || i == 18 || i == 23) {
				if (ch != '-')
					return false;
				continue;
			}

			if (t % 2) {
				value.data[t/2] |= axCStr::hexToInt(ch);
			} else {
				value.data[t/2] = axCStr::hexToInt(ch) << 4;
			}
			t++;
		}
	} catch(axError&) {
		return false;
	}

	return true;
}


#if AX_OS_WINDOWS

void axUuid::generate() {
	AX_STATIC_ASSERT(sizeof(UUID) == kDataSize);
	RPC_STATUS ret = ::UuidCreate(reinterpret_cast<UUID*>(value.data));
	if (ret != RPC_S_OK) throw axError_Undefined(AX_LOC);
}

#else //=====================================

#include <uuid/uuid.h>

void axUuid::generate	() {
	uuid_generate(value.data);
}

#endif
