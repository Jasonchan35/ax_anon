#version 330
#extension GL_ARB_separate_shader_objects : require

layout(std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
    vec4 color0;
    vec4 color1;
    vec4 color2;
    vec4 color3;
} _42;

uniform sampler2D SPIRV_Cross_CombinedmainTex0_axSamplerState_mainTex0;

in vec4 _axVsToPs_00;
in vec2 _axVsToPs_01;
layout(location = 0) out vec4 _entryPointOutput;

void main()
{
    vec4 _95 = texture(SPIRV_Cross_CombinedmainTex0_axSamplerState_mainTex0, _axVsToPs_01);
    _entryPointOutput = ((((_42.color0 * _95.x) + (_42.color1 * _95.y)) + (_42.color2 * _95.z)) + (_42.color3 * _95.w)) * _axVsToPs_00;
}

