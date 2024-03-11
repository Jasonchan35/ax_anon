#version 330 es
precision mediump float;
precision highp int;

uniform highp sampler2D SPIRV_Cross_CombinedmainTex0_axSamplerState_mainTex0;

layout(location = 0) in highp vec4 _axVsToPs_00;
layout(location = 1) in highp vec2 _axVsToPs_01;
layout(location = 0) out highp vec4 _entryPointOutput;

void main()
{
    _entryPointOutput = _axVsToPs_00 * texture(SPIRV_Cross_CombinedmainTex0_axSamplerState_mainTex0, _axVsToPs_01);
}

