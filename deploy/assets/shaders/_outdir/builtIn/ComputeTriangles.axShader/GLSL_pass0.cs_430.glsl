#version 430
#extension GL_ARB_separate_shader_objects : require
layout(local_size_x = 6, local_size_y = 1, local_size_z = 1) in;

const vec3 _155[6] = vec3[](vec3(0.0), vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0));

layout(binding = 0, std140) uniform _Global
{
    float cellSize;
    ivec2 gridSize;
} _89;

layout(binding = 4, std430) buffer BufferOut
{
    uint _data[];
} BufferOut_1;

void main()
{
    uvec3 _361 = gl_GlobalInvocationID;
    uvec3 _363 = gl_LocalInvocationID;
    vec3 _217 = (_155[_363.x] + vec3(gl_WorkGroupID)) * _89.cellSize;
    int _242 = (int(_361.x) + (int(_361.y * 6u) * _89.gridSize.x)) * 20;
    int _244 = _242 >> 2;
    BufferOut_1._data[_244] = floatBitsToUint(_217).x;
    BufferOut_1._data[_244 + 1] = floatBitsToUint(_217).y;
    BufferOut_1._data[_244 + 2] = floatBitsToUint(_217).z;
    int _264 = (_242 + 12) >> 2;
    vec4 _232 = vec4(1.0, 0.0, 1.0, 1.0);
    uvec4 _276;
    _276.x = packHalf2x16(vec2(_232.x, 0.0));
    _276.y = packHalf2x16(vec2(_232.y, 0.0));
    _276.z = packHalf2x16(vec2(_232.z, 0.0));
    _276.w = packHalf2x16(vec2(_232.w, 0.0));
    uvec2 _277;
    _277.x = _276.x | (_276.y << uint(16));
    _277.y = _276.z | (_276.w << uint(16));
    BufferOut_1._data[_264] = _277.x;
    vec4 _233 = vec4(1.0, 0.0, 1.0, 1.0);
    uvec4 _315;
    _315.x = packHalf2x16(vec2(_233.x, 0.0));
    _315.y = packHalf2x16(vec2(_233.y, 0.0));
    _315.z = packHalf2x16(vec2(_233.z, 0.0));
    _315.w = packHalf2x16(vec2(_233.w, 0.0));
    uvec2 _316;
    _316.x = _315.x | (_315.y << uint(16));
    _316.y = _315.z | (_315.w << uint(16));
    BufferOut_1._data[_264 + 1] = _316.y;
}

