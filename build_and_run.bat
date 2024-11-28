@echo off
REM Exit on error
setlocal enabledelayedexpansion
set "ERRORLEVEL=0"
if errorlevel 1 exit /b

echo Setting up directories...
mkdir build
mkdir lib
mkdir include

REM Set paths
set "JAVA_HOME=C:\Program Files\Java\jdk-20"
set "PROJECT_ROOT=%cd%"
set "JAVAFX_PATH=%PROJECT_ROOT%\resources\javafx-sdk-23.0.1"

REM Debug: Print Java paths
echo JAVA_HOME: %JAVA_HOME%
echo PROJECT_ROOT: %PROJECT_ROOT%
echo JAVAFX_PATH: %JAVAFX_PATH%

REM Compile Java sources
echo Compiling Java sources...
javac -d build ^
    -cp "%JAVAFX_PATH%\lib\*" ^
    --module-path "%JAVAFX_PATH%\lib" ^
    --add-modules javafx.controls,javafx.fxml ^
    src\JAVA\*.java src\JAVA\jni\*.java

REM Generate JNI headers
echo Generating JNI headers...
javac -h include -d build ^
    -cp build ^
    src\JAVA\jni\PhysicsEngineJNI.java

REM Compile C++ sources for Windows
echo Compiling C++ sources...
for %%f in (src\cpp\*.cpp) do (
    echo Compiling %%f...
    g++ -c -fPIC ^
        -I"%JAVA_HOME%\include" ^
        -I"%JAVA_HOME%\include\win32" ^
        -I"%PROJECT_ROOT%\include" ^
        -std=c++17 ^
        %%f
)

REM Create shared library
echo Creating shared library...
g++ -shared -o lib\libphysics_native.dll ^
    *.o ^
    -L"%JAVA_HOME%\lib" ^
    -ljvm

REM Clean up object files
del *.o

REM Run the application
echo Running the application...
java -cp build;%JAVAFX_PATH%\lib\* ^
    --module-path "%JAVAFX_PATH%\lib" ^
    --add-modules javafx.controls,javafx.fxml ^
    -Djava.library.path=lib ^
    JAVA.SimulationApp