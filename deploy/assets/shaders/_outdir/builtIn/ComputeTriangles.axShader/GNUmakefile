all:

source.hlsl: info.json
	$(axFileCmd) -touch source.hlsl

CURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST))

ifeq ($(BUILD_DX11_COMPUTE), true)

-include DX11_pass0.cs_5_0.bin.dep
all: DX11_pass0.cs_5_0.bin
DX11_pass0.cs_5_0.bin: $(CURRENT_MAKEFILE) $(axShaderCompiler) 
DX11_pass0.cs_5_0.bin: source.hlsl 
	"$(axShaderCompiler)" -hlsl -file="$<" -out="$@" -profile=cs_5_0 -entry=cs_main -I="../../../builtIn"

endif

ifeq ($(BUILD_DX12_COMPUTE), true)

-include DX12_pass0.cs_6_0.bin.dep
all: DX12_pass0.cs_6_0.bin
DX12_pass0.cs_6_0.bin: $(CURRENT_MAKEFILE) $(axShaderCompiler) 
DX12_pass0.cs_6_0.bin: source.hlsl 
	"$(axShaderCompiler)" -hlsl -file="$<" -out="$@" -profile=cs_6_0 -entry=cs_main -I="../../../builtIn"
	"$(dxc)" -Fo "$@" "$<" -T cs_6_0 -E cs_main -I "../../../builtIn"
 
endif

ifeq ($(BUILD_SPIRV_COMPUTE), true)

all: SPIRV_pass0.cs_1_1.bin
SPIRV_pass0.cs_1_1.bin: $(CURRENT_MAKEFILE) $(glslc) 
SPIRV_pass0.cs_1_1.bin: source.hlsl
	"$(glslc)" -fshader-stage=compute -fentry-point=cs_main -fssbo-binding-base 4 -ftexture-binding-base 8 -fsampler-binding-base 8 -I"../../../builtIn" -o "$@" "$<"

all: SPIRV_pass0.cs_1_1.reflect.json
SPIRV_pass0.cs_1_1.reflect.json: $(CURRENT_MAKEFILE) $(spirv_cross)
SPIRV_pass0.cs_1_1.reflect.json: SPIRV_pass0.cs_1_1.bin
	"$(spirv_cross)" "$<" --output "$@" \
		--reflect\
		--remove-unused-variables \

	"$(axFileCmd)" -dos2unix "$@" 


all: SPIRV_pass0.cs_1_1.bin.json
SPIRV_pass0.cs_1_1.bin.json: $(CURRENT_MAKEFILE) $(axShaderCompiler)
SPIRV_pass0.cs_1_1.bin.json: SPIRV_pass0.cs_1_1.reflect.json
	"$(axShaderCompiler)" -genSpirvInfo -file="$<" -out="$@" \


endif

ifeq ($(BUILD_GLSL_COMPUTE), true)

all: GLSL_pass0.cs_430.glsl
GLSL_pass0.cs_430.glsl: $(CURRENT_MAKEFILE) $(spirv_cross)
GLSL_pass0.cs_430.glsl: SPIRV_pass0.cs_1_1.bin
	"$(spirv_cross)" "$<" --output "$@" \
		--version 430 --no-es \
		--extension GL_ARB_separate_shader_objects \
		--remove-unused-variables \
		--no-420pack-extension \
		--rename-interface-variable in   0 "_axVsToPs_00"  \
		--rename-interface-variable in   1 "_axVsToPs_01"  \
		--rename-interface-variable in   2 "_axVsToPs_02"  \
		--rename-interface-variable in   3 "_axVsToPs_03"  \
		--rename-interface-variable in   4 "_axVsToPs_04"  \
		--rename-interface-variable in   5 "_axVsToPs_05"  \
		--rename-interface-variable in   6 "_axVsToPs_06"  \
		--rename-interface-variable in   7 "_axVsToPs_07"  \
		--rename-interface-variable in   8 "_axVsToPs_08"  \
		--rename-interface-variable in   9 "_axVsToPs_09"  \
		--rename-interface-variable in  10 "_axVsToPs_10"  \
		--rename-interface-variable in  11 "_axVsToPs_11"  \
		--rename-interface-variable in  12 "_axVsToPs_12"  \
		--rename-interface-variable in  13 "_axVsToPs_13"  \
		--rename-interface-variable in  14 "_axVsToPs_14"  \
		--rename-interface-variable in  15 "_axVsToPs_15"  \
		--rename-interface-variable in  16 "_axVsToPs_16"  \

	"$(axFileCmd)" -dos2unix "$@" 


endif

ifeq ($(BUILD_GLSL_ES_COMPUTE), true)

all: GLSL_ES_pass0.cs_310.glsl
GLSL_ES_pass0.cs_310.glsl: $(CURRENT_MAKEFILE) $(spirv_cross)
GLSL_ES_pass0.cs_310.glsl: SPIRV_pass0.cs_1_1.bin
	"$(spirv_cross)" "$<" --output "$@" \
		--version 430 --es \
		--remove-unused-variables \
		--no-420pack-extension \
		--rename-interface-variable in   0 "_axVsToPs_00"  \
		--rename-interface-variable in   1 "_axVsToPs_01"  \
		--rename-interface-variable in   2 "_axVsToPs_02"  \
		--rename-interface-variable in   3 "_axVsToPs_03"  \
		--rename-interface-variable in   4 "_axVsToPs_04"  \
		--rename-interface-variable in   5 "_axVsToPs_05"  \
		--rename-interface-variable in   6 "_axVsToPs_06"  \
		--rename-interface-variable in   7 "_axVsToPs_07"  \
		--rename-interface-variable in   8 "_axVsToPs_08"  \
		--rename-interface-variable in   9 "_axVsToPs_09"  \
		--rename-interface-variable in  10 "_axVsToPs_10"  \
		--rename-interface-variable in  11 "_axVsToPs_11"  \
		--rename-interface-variable in  12 "_axVsToPs_12"  \
		--rename-interface-variable in  13 "_axVsToPs_13"  \
		--rename-interface-variable in  14 "_axVsToPs_14"  \
		--rename-interface-variable in  15 "_axVsToPs_15"  \
		--rename-interface-variable in  16 "_axVsToPs_16"  \

	"$(axFileCmd)" -dos2unix "$@" 


endif

ifeq ($(BUILD_METAL_COMPUTE), true)

all: MTL_pass0.cs_mac.metal
MTL_pass0.cs_mac.metal: $(CURRENT_MAKEFILE) $(spirv_cross)
MTL_pass0.cs_mac.metal: SPIRV_pass0.cs_1_1.bin
	"$(spirv_cross)" "$<" --output "$@" --msl 


all: MTL_pass0.cs_mac.air
MTL_pass0.cs_mac.air: $(CURRENT_MAKEFILE)
MTL_pass0.cs_mac.air: MTL_pass0.cs_mac.metal
	xcrun -sdk macosx metal -c "$<" -o "$@" 


all: MTL_pass0.lib_mac.metallib
MTL_pass0.lib_mac.metallib: $(CURRENT_MAKEFILE) $(metallib)
MTL_pass0.lib_mac.metallib:  MTL_pass0.cs_mac.air
	xcrun -sdk macosx metallib  MTL_pass0.cs_mac.air  -o "$@" 


endif

ifeq ($(BUILD_METAL_IOS_COMPUTE), true)

all: MTL_pass0.cs_ios.metal
MTL_pass0.cs_ios.metal: $(CURRENT_MAKEFILE) $(spirv_cross)
MTL_pass0.cs_ios.metal: SPIRV_pass0.cs_1_1.bin
	"$(spirv_cross)" "$<" --output "$@" --msl --msl-ios


all: MTL_pass0.cs_ios.air
MTL_pass0.cs_ios.air: $(CURRENT_MAKEFILE)
MTL_pass0.cs_ios.air: MTL_pass0.cs_ios.metal
	xcrun -sdk iphoneos metal -c "$<" -o "$@" 


all: MTL_pass0.lib_ios.metallib
MTL_pass0.lib_ios.metallib: $(CURRENT_MAKEFILE) $(metallib)
MTL_pass0.lib_ios.metallib:  MTL_pass0.cs_ios.air
	xcrun -sdk iphoneos metallib  MTL_pass0.cs_ios.air  -o "$@" 


endif

ifeq ($(BUILD_ISPC_COMPUTE), true)

all: ISPC_pass0.cs.ispc.c
ISPC_pass0.cs.ispc.c: $(CURRENT_MAKEFILE) $(spirv_cross)
ISPC_pass0.cs.ispc.c: SPIRV_pass0.cs_1_1.bin
	"$(spirv_cross)" "$<" --output "$@" --ispc


endif

