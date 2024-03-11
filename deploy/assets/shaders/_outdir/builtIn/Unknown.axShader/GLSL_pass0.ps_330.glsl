#version 330
#extension GL_ARB_separate_shader_objects : require

layout(location = 0) out vec4 _entryPointOutput;

void main()
{
    _entryPointOutput = vec4(1.0, 0.0, 1.0, 1.0);
}

