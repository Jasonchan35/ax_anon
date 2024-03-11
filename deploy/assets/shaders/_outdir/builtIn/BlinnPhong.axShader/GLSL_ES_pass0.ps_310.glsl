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
    highp vec3 _239 = normalize(_axVsToPs_02);
    highp vec3 _244 = _axVsToPs_00 - _79.lightPos;
    highp float _246 = length(_244);
    highp vec3 _248 = normalize(_244);
    highp float _253 = max(dot(_239, -_248), 0.0);
    highp float _362;
    if (_253 > 0.0)
    {
        _362 = pow(clamp(dot(normalize(normalize(_79.cameraPos - _axVsToPs_00) - _248), _239), 0.0, 1.0), _79.shininess);
    }
    else
    {
        _362 = 0.0;
    }
    highp vec3 _287 = (_79.lightColor * _79.lightPower) / vec3(_246 * _246);
    highp vec4 _313 = vec4((_79.ambient + ((_79.diffuse * _253) * _287)) + ((_79.specular * _362) * _287), 1.0) * _axVsToPs_01;
    highp vec4 _234 = _313;
    highp vec3 _321 = _313.xyz;
    _entryPointOutput = vec4(mix(((sqrt(_321 - vec3(0.00227999989874660968780517578125)) * 1.1300499439239501953125) - (_321 * 0.134479999542236328125)) + vec3(0.0057190000079572200775146484375), _321 * 12.9200000762939453125, lessThan(_321, vec3(0.003130800090730190277099609375))), _234.w);
}

