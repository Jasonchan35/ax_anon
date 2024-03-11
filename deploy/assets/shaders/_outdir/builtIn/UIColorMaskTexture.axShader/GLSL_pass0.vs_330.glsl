#version 330
#extension GL_ARB_separate_shader_objects : require

layout(std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
    vec4 color0;
    vec4 color1;
    vec4 color2;
    vec4 color3;
} _23;

layout(location = 0) in vec4 i_pos;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec2 i_uv0;
out vec4 _axVsToPs_00;
out vec2 _axVsToPs_01;

mat4 spvWorkaroundRowMajor(mat4 wrap) { return wrap; }

void main()
{
    gl_Position = spvWorkaroundRowMajor(_23.matMvp) * i_pos;
    _axVsToPs_00 = i_color;
    _axVsToPs_01 = i_uv0;
}

