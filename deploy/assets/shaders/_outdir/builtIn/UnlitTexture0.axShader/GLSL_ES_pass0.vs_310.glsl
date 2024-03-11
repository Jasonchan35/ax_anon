#version 330 es

layout(binding = 0, std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
    vec4 color;
} _22;

layout(location = 0) in vec4 i_pos;
layout(location = 1) in vec4 i_color;
layout(location = 0) out vec4 _axVsToPs_00;

mat4 spvWorkaroundRowMajor(mat4 wrap) { return wrap; }

void main()
{
    gl_Position = spvWorkaroundRowMajor(_22.matMvp) * i_pos;
    _axVsToPs_00 = _22.color * i_color;
}

