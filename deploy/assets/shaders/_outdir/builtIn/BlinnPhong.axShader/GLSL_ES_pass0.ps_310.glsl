#version 330 es
precision mediump float;
precision highp int;

layout(binding = 0, std140) uniform _Global
{
    layout(row_major) highp mat4 matMvp;
    layout(row_major) highp mat4 matObjectToWorld;
    highp vec3 cameraPos;
    highp vec3 lightPos;
    highp float lightPower;
    highp vec3 lightColor;
    highp vec3 ambient;
    highp vec3 diffuse;
    highp vec3 specular;
    highp float shininess;
} _79;

layout(location = 0) in highp vec3 _axVsToPs_00;
layout(location = 1) in highp vec4 _axVsToPs_01;
layout(location = 2) in highp vec3 _axVsToPs_02;
layout(location = 0) out highp vec4 _entryPointOutput;

void main()
{
    highp vec3 _240 = normalize(_axVsToPs_02);
    highp vec3 _245 = _axVsToPs_00 - _79.lightPos;
    highp float _247 = length(_245);
    highp vec3 _249 = normalize(_245);
    highp float _254 = max(dot(_240, -_249), 0.0);
    highp float _365;
    if (_254 > 0.0)
    {
        _365 = pow(clamp(dot(normalize(normalize(_79.cameraPos - _axVsToPs_00) - _249), _240), 0.0, 1.0), _79.shininess);
    }
    else
    {
        _365 = 0.0;
    }
    highp vec3 _288 = (_79.lightColor * _79.lightPower) / vec3(_247 * _247);
    highp vec4 _314 = vec4((_79.ambient + ((_79.diffuse * _254) * _288)) + ((_79.specular * _365) * _288), 1.0) * _axVsToPs_01;
    highp vec4 _235 = _314;
    highp vec3 _322 = _314.xyz;
    _entryPointOutput = vec4(mix(_322 * 12.9200000762939453125, ((sqrt(_322 - vec3(0.00227999989874660968780517578125)) * 1.1300499439239501953125) - (_322 * 0.134479999542236328125)) + vec3(0.0057190000079572200775146484375), step(_322, vec3(0.003130800090730190277099609375))), _235.w);
}

