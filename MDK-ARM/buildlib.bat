REM update cdd lib
set keil5=C:\Keil_v5\UV4\UV4.exe
set cddproject=%cd%\build_lib_cdd.uvprojx
%keil5% -j0 -b %cddproject%  -l build_cdd.log
echo F | xcopy  .\Objects\*.lib ..\Src\Cdd\cdd.lib /y /Q  /A 

REM update canstack lib
REM set keil5=C:\Keil_v5\UV4\UV4.exe
set canstackproject=%cd%\build_lib_canstack.uvprojx
%keil5% -j0 -b %canstackproject% -l build_canstack.log
echo F | xcopy  .\Objects\*.lib ..\Src\CanStack\canstack.lib /y   /Q /F


REM update driverif lib
REM set keil5=C:\Keil_v5\UV4\UV4.exe
set driverifproject=%cd%\build_lib_driverif.uvprojx
%keil5% -j0 -b %driverifproject% -l build_driverif.log
echo F | xcopy  .\Objects\*.lib ..\Src\DriverIf\driverif.lib  /y /Q /F


REM update system lib
REM set keil5=C:\Keil_v5\UV4\UV4.exe
set systemproject=%cd%\build_lib_system.uvprojx
%keil5% -j0 -b %systemproject% -l build_system.log
echo F | xcopy  .\Objects\*.lib ..\Src\System\System.lib /y /Q /F


REM update CanNm lib
REM set keil5=C:\Keil_v5\UV4\UV4.exe
set systemproject=%cd%\build_lib_CanNm.uvprojx
%keil5% -j0 -b %systemproject% -l build_system.log
echo F | xcopy  .\Objects\*.lib ..\Src\CanNm\CanNm.lib /y /Q /F


REM build debug project
REM set keil5=C:\Keil_v5\UV4\UV4.exe
set debuglibproject=%cd%\debug_lib.uvprojx
%keil5% -j0 -b %debuglibproject%  -l build_debuglib.log
 


pause 

REM clear all secrect *.c
del /f /s /q ..\src\CanStack\*.c
del /f /s /q ..\src\Cdd\*.c
del /f /s /q ..\src\Cdd\*.xlsx
del /f /s /q ..\src\Driverif\*.c
del /f /s /q ..\src\System\*.c
del /f /s /q ..\src\CanNm\*.c

del /f /s /q *.uvguix.*
del /f /s /q *.uvoptx

del /f /s /q *.log
del /f /s /q Objects\*.*
del /f /s /q Listings\*.*