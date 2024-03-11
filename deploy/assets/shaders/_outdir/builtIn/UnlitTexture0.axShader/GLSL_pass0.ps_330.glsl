#version 330
#extension GL_ARB_separate_shader_objects : require

in vec4 _axVsToPs_00;
layout(location = 0) out vec4 _entryPointOutput;

void main()
{
    _entryPointOutput = _axVsToPs_00;
}

