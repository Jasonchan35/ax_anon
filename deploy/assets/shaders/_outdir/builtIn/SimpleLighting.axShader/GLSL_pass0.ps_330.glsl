#version 330
#extension GL_ARB_separate_shader_objects : require

layout(std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
    vec4 color;
    vec3 cameraPos;
    vec3 lightPos;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
} _26;

in vec3 _axVsToPs_00;
in vec4 _axVsToPs_01;
in vec3 _axVsToPs_02;
layout(location = 0) out vec4 _entryPointOutput;

void main()
{
    vec3 _124 = normalize(_axVsToPs_02);
    vec3 _130 = normalize(_26.lightPos - _axVsToPs_00);
    _entryPointOutput = ((_26.ambient + (_26.diffuse * clamp(dot(_124, _130), 0.0, 1.0))) + (_26.specular * pow(clamp(dot(_124, normalize(_130 + normalize(_26.cameraPos - _axVsToPs_00))), 0.0, 1.0), _26.shininess))) * _axVsToPs_01;
}

