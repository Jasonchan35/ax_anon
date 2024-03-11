#pragma once

#if ax_TRY_USE_DX11
	#if AX_OS_WINDOWS
		#define AX_USE_DX11		1
	#endif
#endif

#if ax_TRY_USE_DX12
	#if AX_OS_WINDOWS
		#define AX_USE_DX12		1
//		#define AX_USE_DX12_DXR	1
	#endif
#endif

#if AX_TRY_USE_OPENGL
	#if AX_OS_WINDOWS || AX_OS_OSX || AX_OS_LINUX || AX_OS_IOS || AX_OS_ANDROID
		#define AX_USE_OPENGL	1
	#endif

	#if AX_OS_IOS || axOS_Android
		#define AX_USE_OpenGL_ES	1
	#endif
#endif

#if AX_TRY_USE_VULKAN
	#if AX_OS_WINDOWS || AX_OS_OSX || AX_OS_LINUX
		#define AX_USE_VULKAN	1
//		#define AX_USE_VulkanRT	1
	#endif
#endif

#if AX_TRY_USE_METAL
	#if AX_OS_OSX
		#define AX_USE_METAL	1
		#define AX_USE_MetalRT	1
	#endif
#endif

static const axInt axRender_kMaxRenderTargetCount = 8;
static const axInt axRender_kMaxFrameAheadCount = 3;
static const axInt axRender_kMaxFrameAheadCountHardLimit = 8;

constexpr axVec3i axRender_kNumThreads1d = axVec3i(64, 1, 1); // NV=32 AMD=64 Intel=8, so 64 is multiple for all
constexpr axVec3i axRender_kNumThreads2d = axVec3i( 8, 8, 1); // in 2 dimension 8 x 8 = 64
constexpr axVec3i axRender_kNumThreads3d = axVec3i( 4, 4, 4); // in 3 dimension 4 x 4 x 4 = 64

using axRenderSeqId = axInt64;
