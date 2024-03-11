#version 330
#extension GL_ARB_separate_shader_objects : require

uniform sampler2D SPIRV_Cross_CombinedmainTex0_axSamplerState_mainTex0;

in vec4 _axVsToPs_00;
in vec2 _axVsToPs_01;
layout(location = 0) out vec4 _entryPointOutput;

void main()
{
    _entryPointOutput = _axVsToPs_00 * texture(SPIRV_Cross_CombinedmainTex0_axSamplerState_mainTex0, _axVsToPs_01);
}

