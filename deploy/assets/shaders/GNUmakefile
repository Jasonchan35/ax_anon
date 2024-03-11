.PHONY: all FORCE start
# "all" must be first target
all:

#----------------------

outdir=./_outdir
input_files=\
	builtIn/ComputeTriangles.axShader \
	builtIn/BlinnPhong.axShader \
	builtIn/SimpleLighting.axShader \
	builtIn/TextBillboardTexture0.axShader \
	builtIn/UIColorMaskTexture.axShader \
	builtIn/UITexture0.axShader \
	builtIn/UITexture1.axShader \
	builtIn/UITexture2.axShader \
	builtIn/Unknown.axShader \
	builtIn/UIText.axShader \
	builtIn/UITextTexture0.axShader \
	builtIn/UnlitTexture0.axShader \
	builtIn/UnlitTexture1.axShader \
	builtIn/UnlitTexture2.axShader \
	builtIn/Wireframe.axShader \
	builtIn/Gizmo.axShader \
	raytracing/RayTracing.axShader \
#-------------
#	builtIn/ComputeTest.axShader \

ifeq ($(OS), Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    AX_PLATFORM_OS := Windows

else ifeq ($(shell uname), Darwin)
	AX_PLATFORM_OS := MacOSX

else ifeq ($(shell uname), Linux)
	AX_PLATFORM_OS := Linux
else
	$(error Unsupported OS $(UNAME_OS))
endif

ifeq ($(AX_PLATFORM_OS), Windows)
	ax_tools_bin = $(realpath ../../bin/x64-windows/Debug)
	vulkan_sdk_bin  = C:/VulkanSDK/1.2.198.1/Bin

# DXIL.dll needs win sdk
	windows_sdk_bin = C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64

	exe_suffix=.exe
	export exe_suffix

	PATH:=$(windows_sdk_bin);$(PATH)
	export PATH

	BUILD_DX11				:= true
	BUILD_DX11_COMPUTE		:= true
	BUILD_DX12				:= true
	BUILD_DX12_COMPUTE		:= true

	BUILD_SPIRV				:= true
	BUILD_SPIRV_COMPUTE		:= true
	BUILD_GLSL				:= true
	BUILD_GLSL_COMPUTE		:= true
	BUILD_GLSL_ES			:= true
	BUILD_GLSL_ES_COMPUTE	:= true

# not in spirv-cross main branch yet
#	BUILD_ISPC				:= true
#	BUILD_ISPC_COMPUTE		:= true

else ifeq ($(AX_PLATFORM_OS), MacOSX)
	ax_tools_bin = $(realpath ../../_build/anon3-xcode-clang-macosx-x86_64/bin/Debug)
	vulkan_sdk_bin = /usr/local/bin
	xcode_toolchain_bin = /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin

	BUILD_SPIRV				:= true
	BUILD_SPIRV_COMPUTE		:= true
	BUILD_GLSL				:= true
	BUILD_GLSL_COMPUTE		:= true
	BUILD_GLSL_ES			:= true
	BUILD_GLSL_ES_COMPUTE	:= true

	BUILD_METAL				:= true
	BUILD_METAL_COMPUTE		:= true
	BUILD_METAL_IOS 		:= true
	BUILD_METAL_IOS_COMPUTE	:= true

else ifeq ($(AX_PLATFORM_OS), Linux)
	ax_tools_bin = $(realpath ../../_build/anon3-makefile-gcc-linux-x86_64/bin/Debug)
	vulkan_sdk_bin = /usr/bin

	BUILD_SPIRV				:= true
#	BUILD_SPIRV_COMPUTE		:= true
	BUILD_GLSL				:= true
#	BUILD_GLSL_COMPUTE		:= true
	BUILD_GLSL_ES			:= true
#	BUILD_GLSL_ES_COMPUTE	:= true

endif

export BUILD_DX11
export BUILD_DX11_COMPUTE
export BUILD_DX12
export BUILD_DX12_COMPUTE
export BUILD_SPIRV
export BUILD_SPIRV_COMPUTE
export BUILD_GLSL
export BUILD_GLSL_COMPUTE
export BUILD_GLSL_ES
export BUILD_GLSL_ES_COMPUTE
export BUILD_METAL
export BUILD_METAL_COMPUTE
export BUILD_METAL_IOS
export BUILD_METAL_IOS_COMPUTE
export BUILD_ISPC
export BUILD_ISPC_COMPUTE

output_targets=$(addprefix $(outdir)/, $(addsuffix /info.json, $(input_files)))

axFileCmd=$(ax_tools_bin)/axFileCmd
export axFileCmd

AX_PLATFORM_OS = $(shell $(axFileCmd) -platform_os)
export AX_PLATFORM_OS

axShaderCompiler=$(ax_tools_bin)/axShaderCompiler$(exe_suffix)
export axShaderCompiler

dxc=$(vulkan_sdk_bin)/dxc$(exe_suffix)
export dxc

glslc=$(vulkan_sdk_bin)/glslc$(exe_suffix)
export glslc

spirv_cross=$(vulkan_sdk_bin)/spirv-cross$(exe_suffix)
export spirv_cross

all: start $(output_targets)
	@echo ===== End =====

start:
	@echo ==========================
	@echo ax_tools_bin=$(ax_tools_bin)
	@echo AX_PLATFORM_OS=$(AX_PLATFORM_OS)
	@echo windows_sdk_bin=$(windows_sdk_bin)
	@echo vulkan_sdk_bin=$(vulkan_sdk_bin)	
	@echo ==========================

$(outdir)/%.axShader/info.json: %.axShader start FORCE
	@echo [==== $(realpath $(outdir)/$<) ====]
	@$(axShaderCompiler) -genMakefile -file="$<" -out="$(outdir)/$<"
	@$(MAKE) --silent -C "$(outdir)/$<"

$(outdir)/%.axComputeShader/info.json: %.axComputeShader start FORCE
	@echo [==== $(realpath $(outdir)/$<) ====]
	@$(axShaderCompiler) -genMakefile -file="$<" -out="$(outdir)/$<"
	@$(MAKE) --silent -C "$(outdir)/$<"

FORCE:
