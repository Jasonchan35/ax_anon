#include "ax_utils.h"
#include <ax_core/profiler/axProfiler.h>
#include <ax_core/string/axString.h>

struct ax_core_init_helper {
	ax_core_init_helper() {
		ax_fallback_allocator();
		ax_default_allocator();

		ax_default_array_allocator();
		ax_default_string_allocator();

		ax_default_list_allocator();
		ax_default_dict_allocator();

		axProfiler_init();
	}
};

void ax_core_init() {
	static ax_core_init_helper helper;
}

