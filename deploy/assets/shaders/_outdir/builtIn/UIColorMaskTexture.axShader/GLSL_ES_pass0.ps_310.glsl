#version 330 es
precision mediump float;
precision highp int;

layout(binding = 0, std140) uniform _Global
{
    layout(row_major) highp mat4 matMvp;
    highp vec4 color0;
    highp vec4 color1;
    highp vec4 color2;
    highp vec4 color3;
} _42;

uniform highp sampler2D SPIRV_Cross_CombinedmainTex0_axSamplerState_mainTex0;

layout(location = 0) in highp vec4 _axVsToPs_00;
layout(location = 1) in highp vec2 _axVsToPs_01;
layout(location = 0) out highp vec4 _entryPointOutput;

void main()
{
    highp vec4 _95 = texture(SPIRV_Cross_CombinedmainTex0_axSamplerState_mainTex0, _axVsToPs_01);
    _entryPointOutput = ((((_42.color0 * _95.x) + (_42.color1 * _95.y)) + (_42.color2 * _95.z)) + (_42.color3 * _95.w)) * _axVsToPs_00;
}

