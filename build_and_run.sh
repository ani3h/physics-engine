#!/bin/bash

# Exit on error
set -e

# Get the absolute path to the project directory
PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Detect OS and set appropriate variables
OS_NAME=$(uname -s)
if [ "$OS_NAME" = "Darwin" ]; then
    SHARED_LIB_EXT=".dylib"
    COMPILER_FLAGS="-dynamiclib"
    OS_SPECIFIC_DIR="darwin"
elif [ "$OS_NAME" = "Linux" ]; then
    SHARED_LIB_EXT=".so"
    COMPILER_FLAGS="-shared -fPIC"
    OS_SPECIFIC_DIR="linux"
else
    SHARED_LIB_EXT=".dll"
    COMPILER_FLAGS="-shared"
    OS_SPECIFIC_DIR="win32"
fi

# Set paths
JAVAFX_PATH="$PROJECT_DIR/resources/javafx-sdk-23.0.1/lib"
CPP_SRC_DIR="$PROJECT_DIR/src/cpp"
JAVA_SRC_DIR="$PROJECT_DIR/src/JAVA"
INCLUDE_DIR="$PROJECT_DIR/include"
LIB_DIR="$PROJECT_DIR/lib"
BIN_DIR="$PROJECT_DIR/bin"

echo "Setting up directories..."
# Create necessary directories
mkdir -p "$BIN_DIR" "$INCLUDE_DIR" "$LIB_DIR"

echo "Compiling Java sources..."
# Compile Java sources first to generate JNI headers
javac -h "$INCLUDE_DIR" \
      -d "$BIN_DIR" \
      --module-path "$JAVAFX_PATH" \
      --add-modules javafx.controls,javafx.graphics \
      -cp "$JAVAFX_PATH/*" \
      $(find "$JAVA_SRC_DIR" -name "*.java")

echo "Compiling C++ sources..."
# Compile C++ sources
CPP_FILES=(
    "$CPP_SRC_DIR/object.cpp"
    "$CPP_SRC_DIR/forces.cpp"
    "$CPP_SRC_DIR/physics_world.cpp"
    "$CPP_SRC_DIR/collider.cpp"
)

OBJ_FILES=()

# Compile each C++ file
for cpp_file in "${CPP_FILES[@]}"; do
    obj_file="$BIN_DIR/$(basename "${cpp_file%.*}").o"
    OBJ_FILES+=("$obj_file")
    
    g++ -c -fPIC \
        -I"$JAVA_HOME/include" \
        -I"$JAVA_HOME/include/$OS_SPECIFIC_DIR" \
        -I"$INCLUDE_DIR" \
        -I"$CPP_SRC_DIR" \
        -O2 \
        "$cpp_file" \
        -o "$obj_file"
done

echo "Creating shared library..."
# Create the shared library
g++ $COMPILER_FLAGS \
    -o "$LIB_DIR/libphysics_native$SHARED_LIB_EXT" \
    "${OBJ_FILES[@]}" \
    -lc

# Set up library path for runtime
if [ "$OS_NAME" = "Darwin" ]; then
    export DYLD_LIBRARY_PATH="$LIB_DIR:$DYLD_LIBRARY_PATH"
else
    export LD_LIBRARY_PATH="$LIB_DIR:$LD_LIBRARY_PATH"
fi

echo "Starting application..."
# Run the application
java --module-path "$JAVAFX_PATH:$BIN_DIR" \
     --add-modules javafx.controls,javafx.graphics \
     -cp "$BIN_DIR:$JAVAFX_PATH/*" \
     -Djava.library.path="$LIB_DIR" \
     JAVA.SimulationApp