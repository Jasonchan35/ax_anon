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
} _79;

in vec3 _axVsToPs_00;
in vec4 _axVsToPs_01;
in vec3 _axVsToPs_02;
layout(location = 0) out vec4 _entryPointOutput;

void main()
{
    vec3 _239 = normalize(_axVsToPs_02);
    vec3 _244 = _axVsToPs_00 - _79.lightPos;
    float _246 = length(_244);
    vec3 _248 = normalize(_244);
    float _253 = max(dot(_239, -_248), 0.0);
    float _362;
    if (_253 > 0.0)
    {
        _362 = pow(clamp(dot(normalize(normalize(_79.cameraPos - _axVsToPs_00) - _248), _239), 0.0, 1.0), _79.shininess);
    }
    else
    {
        _362 = 0.0;
    }
    vec3 _287 = (_79.lightColor * _79.lightPower) / vec3(_246 * _246);
    vec4 _313 = vec4((_79.ambient + ((_79.diffuse * _253) * _287)) + ((_79.specular * _362) * _287), 1.0) * _axVsToPs_01;
    vec4 _234 = _313;
    vec3 _321 = _313.xyz;
    vec3 _340 = ((sqrt(_321 - vec3(0.00227999989874660968780517578125)) * 1.1300499439239501953125) - (_321 * 0.134479999542236328125)) + vec3(0.0057190000079572200775146484375);
    vec3 _342 = _321 * 12.9200000762939453125;
    bvec3 _344 = lessThan(_321, vec3(0.003130800090730190277099609375));
    _entryPointOutput = vec4(vec3(_344.x ? _342.x : _340.x, _344.y ? _342.y : _340.y, _344.z ? _342.z : _340.z), _234.w);
}

