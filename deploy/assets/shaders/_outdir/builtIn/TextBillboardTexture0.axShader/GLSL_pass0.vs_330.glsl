#version 330
#extension GL_ARB_separate_shader_objects : require

layout(std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
    vec4 color;
    vec2 invViewportSize;
} _40;

layout(location = 0) in vec4 i_pos;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec2 i_uv0;
layout(location = 3) in vec2 i_uv1;
out vec4 _axVsToPs_00;
out vec2 _axVsToPs_01;

mat4 spvWorkaroundRowMajor(mat4 wrap) { return wrap; }

void main()
{
    vec4 _192 = i_pos;
    vec2 _129 = i_pos.xy - i_uv1;
    _192.x = _129.x;
    _192.y = _129.y;
    vec4 _138 = spvWorkaroundRowMajor(_40.matMvp) * _192;
    vec4 _167 = _138;
    float _146 = _167.w;
    vec2 _152 = _138.xy + (((i_uv1 * _40.invViewportSize) * _146) * vec2(2.0, -2.0));
    _167.x = _152.x;
    _167.y = _152.y;
    gl_Position = _167;
    _axVsToPs_00 = _40.color * i_color;
    _axVsToPs_01 = i_uv0;
}

