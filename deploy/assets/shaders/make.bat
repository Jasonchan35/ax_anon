%~d0
cd %~dp0

@REM set gmake_exe=C:\GnuWin32\bin\make.exe
@set gmake_exe=..\..\..\libax5\tools\gmake_win32\make.exe
%gmake_exe% -j %*

@pause