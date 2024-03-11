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
    vec3 _240 = normalize(_axVsToPs_02);
    vec3 _245 = _axVsToPs_00 - _79.lightPos;
    float _247 = length(_245);
    vec3 _249 = normalize(_245);
    float _254 = max(dot(_240, -_249), 0.0);
    float _365;
    if (_254 > 0.0)
    {
        _365 = pow(clamp(dot(normalize(normalize(_79.cameraPos - _axVsToPs_00) - _249), _240), 0.0, 1.0), _79.shininess);
    }
    else
    {
        _365 = 0.0;
    }
    vec3 _288 = (_79.lightColor * _79.lightPower) / vec3(_247 * _247);
    vec4 _314 = vec4((_79.ambient + ((_79.diffuse * _254) * _288)) + ((_79.specular * _365) * _288), 1.0) * _axVsToPs_01;
    vec4 _235 = _314;
    vec3 _322 = _314.xyz;
    _entryPointOutput = vec4(mix(_322 * 12.9200000762939453125, ((sqrt(_322 - vec3(0.00227999989874660968780517578125)) * 1.1300499439239501953125) - (_322 * 0.134479999542236328125)) + vec3(0.0057190000079572200775146484375), step(vec3(0.003130800090730190277099609375), _322)), _235.w);
}

