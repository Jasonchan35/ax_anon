#version 330
#extension GL_ARB_separate_shader_objects : require

layout(std140) uniform _Global
{
    layout(row_major) mat4 matMvp;
    vec4 color;
} _17;

layout(location = 0) out vec4 _entryPointOutput;

void main()
{
    _entryPointOutput = _17.color;
}

