@echo off
setlocal enabledelayedexpansion

python -m ci.ci
if errorlevel 1 exit /b %errorlevel%

if "%~1"=="" (
    echo Usage: %~nx0 ^<name^> ^<args^>
    exit /b 1
)

set "name=%~1"

set "script=.\tests\shell\%name%.bat"

if not exist "%script%" (
    echo Error: %script% not found
    exit /b 2
)

shift
set "args="
:loop
if "%~1"=="" goto done
set "args=!args! %~1"
shift
goto loop
:done

cmd /V:ON /E:ON /C "@echo off & call ""%script%"" !args!"
