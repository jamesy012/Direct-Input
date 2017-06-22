@echo off
echo seting up directorys
set STARTDIR=%CD%
set CONFIG="Debug"
set DLLDIR=%CD%"\Cpp\DirectInput\Builds\DllProject\%CONFIG%"
rem set VC="E:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
set VC="%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"
set PROJDIR="%STARTDIR%\Cpp\DirectInput\DirectInput.sln"

echo setting up visual studio
call %VC%

echo Kill unity task
taskkill /IM unity.exe

echo building dll'd
echo building 32 Bit project
devenv %PROJDIR% /build "Debug|x86"
echo building 64 Bit project
devenv %PROJDIR% /build "Debug|x64"

echo copying dll's
copy %DLLDIR%\Win32\ControllerInputDll.dll "%STARTDIR%\Unity\Assets\Plugins\DirectInput\32"
copy %DLLDIR%\x64\ControllerInputDll.dll "%STARTDIR%\Unity\Assets\Plugins\DirectInput\64"


echo opening unity
Unity\Assets\scene\scene.unity
