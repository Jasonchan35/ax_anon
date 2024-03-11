@REM #change current directory to this file
%~d0
cd %~dp0

set triplets=x64-windows

git clone https://github.com/microsoft/vcpkg.git externals\_vcpkg

call externals\_vcpkg\bootstrap-vcpkg.bat

externals\_vcpkg\vcpkg install vcpkg-tool-ninja

externals\_vcpkg\vcpkg install tracy:%triplets% --recurse

@rem externals\_vcpkg\vcpkg install glew:%triplets% --recurse

externals\_vcpkg\vcpkg install directx-dxc:%triplets% --recurse

externals\_vcpkg\vcpkg install mozjpeg:%triplets% --recurse
externals\_vcpkg\vcpkg install libpng:%triplets% --recurse
externals\_vcpkg\vcpkg install freetype:%triplets% --recurse

@pause