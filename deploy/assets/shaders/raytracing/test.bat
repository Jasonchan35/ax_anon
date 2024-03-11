set SDK_BIN_PATH=C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x64

"%SDK_BIN_PATH%\dxc.exe" -T lib_6_3 RayTracing.hlsl -Fo ..\outdir\raytracing\RayTracing.axShader\pass0.lib_6_3.dx_bin

@pause