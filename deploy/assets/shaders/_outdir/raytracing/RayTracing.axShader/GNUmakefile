all:

source.hlsl: info.json
	$(axFileCmd) -touch source.hlsl

CURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST))

ifeq ($(BUILD_DX11), true)

endif

ifeq ($(BUILD_DX12), true)

-include DX12_pass0.lib_6_3.bin.dep
all: DX12_pass0.lib_6_3.bin
DX12_pass0.lib_6_3.bin: $(CURRENT_MAKEFILE) $(axShaderCompiler) 
DX12_pass0.lib_6_3.bin: source.hlsl 
	"$(axShaderCompiler)" -hlsl -file="$<" -out="$@" -profile=lib_6_3  -I="../../../raytracing"
	"$(dxc)" -Fo "$@" "$<" -T lib_6_3  -I "../../../raytracing"
 
endif

ifeq ($(BUILD_SPIRV), true)

endif

ifeq ($(BUILD_GLSL), true)

endif

ifeq ($(BUILD_GLSL_ES), true)

endif

ifeq ($(BUILD_METAL), true)

endif

ifeq ($(BUILD_METAL_IOS), true)

endif

ifeq ($(BUILD_ISPC), true)

endif

