#change current directory to this file
SCRIPT_PATH=${0%/*}
if [ "$0" != "$SCRIPT_PATH" ] && [ "$SCRIPT_PATH" != "" ]; then 
    cd $SCRIPT_PATH
fi

git clone https://github.com/microsoft/vcpkg.git externals/_vcpkg

# Mac
if [ "$(uname)" == "Darwin" ]; then
    brew install pkg-config cmake
    brew install nasm # needed by mozjpeg

fi

sh externals/_vcpkg/bootstrap-vcpkg.sh

externals/_vcpkg/vcpkg install vcpkg-tool-ninja
externals/_vcpkg/vcpkg install tracy --recurse
# externals/_vcpkg/vcpkg install directx-dxc --recurse
externals/_vcpkg/vcpkg install mozjpeg --recurse
externals/_vcpkg/vcpkg install libpng --recurse
externals/_vcpkg/vcpkg install freetype --recurse

externals/_vcpkg/vcpkg install vulkan --recurse


