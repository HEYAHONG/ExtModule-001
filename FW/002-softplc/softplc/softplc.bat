@echo off

set CURRENT_DIR=%~dp0

set PATH="%CURRENT_DIR%\..\..\3rdparty\matiec_bin\win32";%PATH%

PUSHD "%CURRENT_DIR%"

iec2c -p -l -n -r -R -a -b -c -I "%CURRENT_DIR%\..\..\3rdparty\HCppBox\master\hbox\h3rdparty\3rdparty\matiec_c_header" -T softplc softplc.st

POPD
