#version 330 es
precision mediump float;
precision highp int;

layout(binding = 0, std140) uniform _Global
{
    layout(row_major) highp mat4 matMvp;
    highp vec4 color;
} _17;

layout(location = 0) out highp vec4 _entryPointOutput;

void main()
{
    _entryPointOutput = _17.color;
}

