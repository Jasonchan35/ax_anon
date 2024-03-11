#pragma once

#include <ax_detect_platform.h>

#define ax_TRY_USE_SIMD			1

//------
#define AX_TRY_USE_OPENGL		1
#define ax_TRY_USE_DX11			1
#define ax_TRY_USE_DX12			1
#define AX_TRY_USE_VULKAN		1
#define AX_TRY_USE_METAL		1

//------
#define ax_TRY_USE_X11			1

#if AX_OS_WINDOWS
	#define TRACY_ENABLE	0
	// #define TRACY_CALLSTACK 60
#endif

#if TRACY_ENABLE
	#define AX_FrameMark					FrameMark
	#define AX_FrameMarkNamed(...)			FrameMarkNamed(__VA_ARGS__)

	#define AX_ZoneScoped					ZoneScoped
	#define AX_ZoneScopedN(...)				ZoneScopedN(__VA_ARGS__)
	#define AX_ZoneText(...)				ZoneText(__VA_ARGS__)

	#define AX_TracyGpuContext				TracyGpuContext
	#define AX_TracyGpuContextName(...)		TracyGpuContextName(__VA_ARGS__)
	#define AX_TracyGpuNamedZone(...)		TracyGpuNamedZone(__VA_ARGS__)
	#define AX_TracyGpuNamedZoneC(...)		TracyGpuNamedZoneC(__VA_ARGS__)
	#define AX_TracyGpuZone(...)			TracyGpuZone(__VA_ARGS__)
	#define AX_TracyGpuZoneC(...)			TracyGpuZoneC(__VA_ARGS__)
	#define AX_TracyGpuZoneTransient(...)	TracyGpuZoneTransient(__VA_ARGS__)
	#define AX_TracyGpuCollect				TracyGpuCollect

#else
	#define AX_FrameMark
	#define AX_FrameMarkNamed(...)

	#define AX_ZoneScoped
	#define AX_ZoneScopedN(...)
	#define AX_ZoneText(...)

	#define AX_TracyGpuContext
	#define AX_TracyGpuContextName(...)
	#define AX_TracyGpuNamedZone(...)
	#define AX_TracyGpuNamedZoneC(...)
	#define AX_TracyGpuZone(...)
	#define AX_TracyGpuZoneC(...)
	#define AX_TracyGpuZoneTransient(...)
	#define AX_TracyGpuCollect
#endif

