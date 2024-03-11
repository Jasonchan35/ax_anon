#pragma once

#include <ax_detect_platform.h>

#include "ax_core-pch.h"

#include <ax_core/allocator/_ALL_allocator.h>
#include <ax_core/app/_ALL_app.h>
#include <ax_core/ax_core_config.h>
#include <ax_core/container/_ALL_container.h>
#include <ax_core/data_format/_ALL_data_format.h>
#include <ax_core/file/_ALL_file.h>
#include <ax_core/graph/_ALL_graph.h>
#include <ax_core/log/_ALL_log.h>
#include <ax_core/math/_ALL_math.h>
#include <ax_core/native_ui/_ALL_native_ui.h>
#include <ax_core/net/_ALL_net.h>
#include <ax_core/other/_ALL_other.h>
#include <ax_core/pointer/_ALL_pointer.h>
#include <ax_core/profiler/_ALL_profiler.h>
#include <ax_core/reflection/_ALL_reflection.h>
#include <ax_core/rtti/_ALL_rtti.h>
#include <ax_core/string/_ALL_string.h>
#include <ax_core/system/_ALL_system.h>
#include <ax_core/thread/_ALL_thread.h>
#include <ax_core/time/_ALL_time.h>

#if AX_COMPILER_VC
#pragma comment( lib, "kernel32.lib")
#pragma comment( lib, "ws2_32.lib"	)
#pragma comment( lib, "winmm.lib"	)
#pragma comment( lib, "Opengl32.lib")
#pragma comment( lib, "Glu32.lib")
#endif
