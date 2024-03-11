#version 330
#extension GL_ARB_separate_shader_objects : require

layout(std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
    layout(row_major) mat4 matObjectToWorld;
    vec3 cameraPos;
    vec3 lightPos;
    float lightPower;
    vec3 lightColor;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} _23;

layout(location = 0) in vec4 i_pos;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec3 i_normal;
out vec3 _axVsToPs_00;
out vec4 _axVsToPs_01;
out vec3 _axVsToPs_02;

mat4 spvWorkaroundRowMajor(mat4 wrap) { return wrap; }

void main()
{
    gl_Position = spvWorkaroundRowMajor(_23.matMvp) * i_pos;
    _axVsToPs_00 = (spvWorkaroundRowMajor(_23.matObjectToWorld) * i_pos).xyz;
    _axVsToPs_01 = i_color;
    _axVsToPs_02 = i_normal;
}

