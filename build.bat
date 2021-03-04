:::: =============================================================================
::::
:::: Windows build file for Causatum
::::
::::
:::: =============================================================================

@echo off



:: change this ROOT_DIR and everything else should fix itself
set ROOT_DIR="X:\Projects\learnopenGL\LearnOpenGL"

set OPTS=/EHsc /I"%ROOT_DIR%\includes"
set LIBS=opengl32.lib msvcrt.lib vcruntime.lib libcmt.lib user32.lib gdi32.lib shell32.lib "%ROOT_DIR%\glfw3.lib"
:::


pushd "%ROOT_DIR%\builds\windows_10-x64"

cl /DROOT_DIR=%ROOT_DIR% %OPTS% %LIBS% ../../main.cpp ../../glad.c

popd
