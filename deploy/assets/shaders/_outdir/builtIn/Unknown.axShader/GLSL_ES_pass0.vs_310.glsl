#version 330 es

layout(binding = 0, std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
} _22;

layout(location = 0) in vec4 i_pos;

mat4 spvWorkaroundRowMajor(mat4 wrap) { return wrap; }

void main()
{
    gl_Position = spvWorkaroundRowMajor(_22.matMvp) * i_pos;
}

