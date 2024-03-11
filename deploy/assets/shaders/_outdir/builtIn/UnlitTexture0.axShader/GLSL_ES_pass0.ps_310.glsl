#version 330 es
precision mediump float;
precision highp int;

layout(location = 0) in highp vec4 _axVsToPs_00;
layout(location = 0) out highp vec4 _entryPointOutput;

void main()
{
    _entryPointOutput = _axVsToPs_00;
}

