::@echo off
:: > Setup required Environment
:: -------------------------------------
set COMPILER_DIR=C:\raylib\w64devkit\bin
set PATH=%PATH%;%COMPILER_DIR%
cd %~dp0
:: .
:: > Compile simple .rc file
:: ----------------------------
cmd /c windres ..\..\src\maze_game.rc -o ..\..\src\maze_game.rc.data
:: .
:: > Generating project
:: --------------------------
cmd /c mingw32-make -f ..\..\src\Makefile ^
PROJECT_NAME=maze_game ^
PROJECT_VERSION=1.0 ^
PROJECT_DESCRIPTION="" ^
PROJECT_INTERNAL_NAME=maze_game ^
PROJECT_PLATFORM=PLATFORM_DESKTOP ^
PROJECT_SOURCE_FILES="maze_game.c" ^
BUILD_MODE="RELEASE" ^
BUILD_WEB_ASYNCIFY=FALSE ^
BUILD_WEB_MIN_SHELL=TRUE ^
BUILD_WEB_HEAP_SIZE=268435456 ^
RAYLIB_MODULE_AUDIO=TRUE ^
RAYLIB_MODULE_MODELS=TRUE
