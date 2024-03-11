#version 330 es
precision mediump float;
precision highp int;

uniform highp sampler2D SPIRV_Cross_CombinedglyphTex_axSamplerState_glyphTex;

layout(location = 0) in highp vec4 _axVsToPs_00;
layout(location = 1) in highp vec2 _axVsToPs_01;
layout(location = 0) out highp vec4 _entryPointOutput;

void main()
{
    highp vec4 _68 = texture(SPIRV_Cross_CombinedglyphTex_axSamplerState_glyphTex, _axVsToPs_01);
    _entryPointOutput = _axVsToPs_00 * _68.x;
}

