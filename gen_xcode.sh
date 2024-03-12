#change current directory to this file
SCRIPT_PATH=${0%/*}
if [ "$0" != "$SCRIPT_PATH" ] && [ "$SCRIPT_PATH" != "" ]; then 
    cd $SCRIPT_PATH
fi

cmake -G "Xcode"  -B _build/ax_anon-x64-osx .

