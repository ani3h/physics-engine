#!/bin/bash

# Exit on any error
set -e

echo "Setting up directories..."
mkdir -p build
mkdir -p lib

# Set paths
JAVA_HOME=$(/usr/libexec/java_home)
PROJECT_ROOT=$(pwd)
JAVAFX_PATH="$PROJECT_ROOT/resources/javafx-sdk-23.0.1"

# Compile Java sources
echo "Compiling Java sources..."
javac -d build \
    -cp "$JAVAFX_PATH/lib/*" \
    --module-path "$JAVAFX_PATH/lib" \
    --add-modules javafx.controls,javafx.fxml \
    src/JAVA/*.java src/JAVA/jni/*.java

# Generate JNI headers
echo "Generating JNI headers..."
javac -h include -d build \
    -cp build \
    src/JAVA/jni/PhysicsEngineJNI.java

# Compile C++ sources for macOS
echo "Compiling C++ sources..."
clang++ -c -fPIC \
    -I"$JAVA_HOME/include" \
    -I"$JAVA_HOME/include/darwin" \
    -Iinclude \
    -std=c++17 \
    src/cpp/*.cpp

# Create shared library
echo "Creating shared library..."
clang++ -dynamiclib -o lib/libphysics_native.dylib \
    *.o \
    -framework JavaVM

# Clean up object files
rm *.o

# Run the application
echo "Running the application..."
java -cp build:$JAVAFX_PATH/lib/* \
    --module-path "$JAVAFX_PATH/lib" \
    --add-modules javafx.controls,javafx.fxml \
    -Djava.library.path=lib \
    JAVA.SimulationApp