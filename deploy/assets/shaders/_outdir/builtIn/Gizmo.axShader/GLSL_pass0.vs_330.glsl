#version 330
#extension GL_ARB_separate_shader_objects : require

layout(std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
    vec4 color;
} _22;

layout(location = 0) in vec4 i_pos;
layout(location = 1) in vec4 i_color;
out vec4 _axVsToPs_00;

mat4 spvWorkaroundRowMajor(mat4 wrap) { return wrap; }

void main()
{
    gl_Position = spvWorkaroundRowMajor(_22.matMvp) * i_pos;
    _axVsToPs_00 = _22.color * i_color;
}

