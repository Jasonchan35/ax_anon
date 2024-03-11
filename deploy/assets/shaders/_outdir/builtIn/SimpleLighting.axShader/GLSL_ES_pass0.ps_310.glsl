#version 330 es
precision mediump float;
precision highp int;

layout(binding = 0, std140) uniform _Global
{
    layout(row_major) highp mat4 matMvp;
    highp vec4 color;
    highp vec3 cameraPos;
    highp vec3 lightPos;
    highp vec4 ambient;
    highp vec4 diffuse;
    highp vec4 specular;
    highp float shininess;
} _26;

layout(location = 0) in highp vec3 _axVsToPs_00;
layout(location = 1) in highp vec4 _axVsToPs_01;
layout(location = 2) in highp vec3 _axVsToPs_02;
layout(location = 0) out highp vec4 _entryPointOutput;

void main()
{
    highp vec3 _124 = normalize(_axVsToPs_02);
    highp vec3 _130 = normalize(_26.lightPos - _axVsToPs_00);
    _entryPointOutput = ((_26.ambient + (_26.diffuse * clamp(dot(_124, _130), 0.0, 1.0))) + (_26.specular * pow(clamp(dot(_124, normalize(_130 + normalize(_26.cameraPos - _axVsToPs_00))), 0.0, 1.0), _26.shininess))) * _axVsToPs_01;
}

