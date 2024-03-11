#version 330
#extension GL_ARB_separate_shader_objects : require

uniform sampler2D SPIRV_Cross_CombinedglyphTex_axSamplerState_glyphTex;

in vec4 _axVsToPs_00;
in vec2 _axVsToPs_01;
layout(location = 0) out vec4 _entryPointOutput;

void main()
{
    vec4 _68 = texture(SPIRV_Cross_CombinedglyphTex_axSamplerState_glyphTex, _axVsToPs_01);
    _entryPointOutput = _axVsToPs_00 * _68.x;
}

