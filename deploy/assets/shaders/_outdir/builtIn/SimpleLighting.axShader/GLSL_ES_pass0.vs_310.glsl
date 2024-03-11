#version 330 es

layout(binding = 0, std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
    vec4 color;
    vec3 cameraPos;
    vec3 lightPos;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
} _23;

layout(location = 0) in vec4 i_pos;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec3 i_normal;
layout(location = 0) out vec3 _axVsToPs_00;
layout(location = 1) out vec4 _axVsToPs_01;
layout(location = 2) out vec3 _axVsToPs_02;

mat4 spvWorkaroundRowMajor(mat4 wrap) { return wrap; }

void main()
{
    gl_Position = spvWorkaroundRowMajor(_23.matMvp) * i_pos;
    _axVsToPs_00 = i_pos.xyz;
    _axVsToPs_01 = _23.color * i_color;
    _axVsToPs_02 = i_normal;
}

