SCRIPT_PATH=${0%/*}
if [ "$0" != "$SCRIPT_PATH" ] && [ "$SCRIPT_PATH" != "" ]; then 
	# change working directry to script path
    cd $SCRIPT_PATH
fi

cmake -G "Unix Makefiles" -B _build/ax_anon-makefile
