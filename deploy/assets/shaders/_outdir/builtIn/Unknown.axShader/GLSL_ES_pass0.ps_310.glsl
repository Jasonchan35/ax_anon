#version 330 es
precision mediump float;
precision highp int;

layout(location = 0) out highp vec4 _entryPointOutput;

void main()
{
    _entryPointOutput = vec4(1.0, 0.0, 1.0, 1.0);
}

